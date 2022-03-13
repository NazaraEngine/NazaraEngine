// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/FilesystemModuleResolver.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/Shader/ShaderLangParser.hpp>
#include <cassert>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	void FilesystemModuleResolver::RegisterModule(std::filesystem::path realPath)
	{
		return RegisterModule(ShaderLang::ParseFromFile(realPath));
	}

	void FilesystemModuleResolver::RegisterModule(std::string_view moduleSource)
	{
		return RegisterModule(ShaderLang::Parse(moduleSource));
	}

	void FilesystemModuleResolver::RegisterModule(ShaderAst::ModulePtr module)
	{
		assert(module);

		std::string moduleName = module->metadata->moduleName;
		if (moduleName.empty())
			throw std::runtime_error("cannot register anonymous module");

		m_modules.emplace(std::move(moduleName), std::move(module));
	}

	void FilesystemModuleResolver::RegisterModuleDirectory(std::filesystem::path realPath)
	{
		for (const auto& entry : std::filesystem::recursive_directory_iterator(realPath))
		{
			if (entry.is_regular_file() && StringEqual(entry.path().extension().generic_u8string(), "nzsl", Nz::CaseIndependent{}))
			{
				try
				{
					RegisterModule(entry.path());
				}
				catch (const std::exception& e)
				{
					NazaraWarning("failed to register module " + entry.path().generic_u8string() + ": " + e.what());
				}
			}
		}
	}

	ShaderAst::ModulePtr FilesystemModuleResolver::Resolve(const std::string& moduleName)
	{
		auto it = m_modules.find(moduleName);
		if (it == m_modules.end())
			return {};

		return it->second;
	}
}
