// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_FILESYSTEMMODULERESOLVER_HPP
#define NAZARA_SHADER_FILESYSTEMMODULERESOLVER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/ShaderModuleResolver.hpp>
#include <filesystem>
#include <string>
#include <unordered_map>

namespace Nz
{
	class NAZARA_SHADER_API FilesystemModuleResolver : public ShaderModuleResolver
	{
		public:
			FilesystemModuleResolver() = default;
			FilesystemModuleResolver(const FilesystemModuleResolver&) = default;
			FilesystemModuleResolver(FilesystemModuleResolver&&) = default;
			~FilesystemModuleResolver() = default;

			void RegisterModule(const std::filesystem::path& realPath);
			void RegisterModule(std::string_view moduleSource);
			void RegisterModule(ShaderAst::ModulePtr module);
			void RegisterModuleDirectory(const std::filesystem::path& realPath);

			ShaderAst::ModulePtr Resolve(const std::string& moduleName) override;

			FilesystemModuleResolver& operator=(const FilesystemModuleResolver&) = default;
			FilesystemModuleResolver& operator=(FilesystemModuleResolver&&) = default;

		private:
			std::unordered_map<std::string, ShaderAst::ModulePtr> m_modules;
	};
}

#include <Nazara/Shader/FilesystemModuleResolver.inl>

#endif // NAZARA_SHADER_FILESYSTEMMODULERESOLVER_HPP
