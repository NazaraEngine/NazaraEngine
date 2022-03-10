// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_DIRECTORYMODULERESOLVER_HPP
#define NAZARA_SHADER_DIRECTORYMODULERESOLVER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/VirtualDirectory.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/ShaderModuleResolver.hpp>
#include <string>
#include <unordered_map>

namespace Nz
{
	class NAZARA_SHADER_API DirectoryModuleResolver : public ShaderModuleResolver
	{
		public:
			inline DirectoryModuleResolver();
			DirectoryModuleResolver(const DirectoryModuleResolver&) = delete;
			DirectoryModuleResolver(DirectoryModuleResolver&&) = delete;
			~DirectoryModuleResolver() = default;

			inline void RegisterModuleDirectory(std::string_view path, std::filesystem::path realPath);
			inline void RegisterModuleFile(std::string_view path, std::filesystem::path realPath);
			inline void RegisterModuleFile(std::string_view path, std::vector<UInt8> fileContent);
			inline void RegisterModuleFile(std::string_view path, const void* staticData, std::size_t size);

			ShaderAst::ModulePtr Resolve(const std::vector<std::string>& modulePath) override;

			DirectoryModuleResolver& operator=(const DirectoryModuleResolver&) = delete;
			DirectoryModuleResolver& operator=(DirectoryModuleResolver&&) = delete;

		private:
			std::shared_ptr<VirtualDirectory> m_searchDirectory;
			std::unordered_map<std::string, ShaderAst::ModulePtr> m_knownModules;
	};
}

#include <Nazara/Shader/DirectoryModuleResolver.inl>

#endif // NAZARA_SHADER_DIRECTORYMODULERESOLVER_HPP
