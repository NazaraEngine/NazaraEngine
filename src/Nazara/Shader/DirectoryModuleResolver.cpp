// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/DirectoryModuleResolver.hpp>
#include <Nazara/Shader/ShaderLangParser.hpp>
#include <filesystem>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	ShaderAst::ModulePtr DirectoryModuleResolver::Resolve(const std::vector<std::string>& modulePath)
	{
		if (modulePath.empty())
			return {};

		std::string fullPath;
		for (const auto& part : modulePath)
		{
			if (!fullPath.empty())
				fullPath += '/';

			fullPath += part;
		}

		if (auto it = m_knownModules.find(fullPath); it != m_knownModules.end())
			return it->second;

		VirtualDirectory::Entry entry;
		if (!m_searchDirectory->GetEntry(fullPath, &entry))
			return {};

		ShaderAst::ModulePtr shaderModule;
		if (std::holds_alternative<VirtualDirectory::DataPointerEntry>(entry))
		{
			const auto& content = std::get<VirtualDirectory::DataPointerEntry>(entry);
			shaderModule = ShaderLang::Parse(std::string_view(reinterpret_cast<const char*>(content.data), content.size));
		}
		else if (std::holds_alternative<VirtualDirectory::FileContentEntry>(entry))
		{
			const auto& content = std::get<VirtualDirectory::FileContentEntry>(entry);
			shaderModule = ShaderLang::Parse(std::string_view(reinterpret_cast<const char*>(content.data->data()), content.data->size()));
		}
		else if (std::holds_alternative<VirtualDirectory::PhysicalFileEntry>(entry))
		{
			const auto& filePath = std::get<VirtualDirectory::PhysicalFileEntry>(entry);
			shaderModule = ShaderLang::ParseFromFile(filePath);
		}

		if (!shaderModule)
			return {};

		m_knownModules.emplace(fullPath, shaderModule);

		return shaderModule;
	}
}
