// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_FILESYSTEMMODULERESOLVER_HPP
#define NAZARA_SHADER_FILESYSTEMMODULERESOLVER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/MovablePtr.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/ShaderModuleResolver.hpp>
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>

namespace Nz
{
	class NAZARA_SHADER_API FilesystemModuleResolver : public ShaderModuleResolver
	{
		public:
			FilesystemModuleResolver();
			FilesystemModuleResolver(const FilesystemModuleResolver&) = delete;
			FilesystemModuleResolver(FilesystemModuleResolver&&) noexcept = delete;
			~FilesystemModuleResolver();

			void RegisterModule(const std::filesystem::path& realPath);
			void RegisterModule(std::string_view moduleSource);
			void RegisterModule(ShaderAst::ModulePtr module);
			void RegisterModuleDirectory(const std::filesystem::path& realPath, bool watchDirectory = true);

			ShaderAst::ModulePtr Resolve(const std::string& moduleName) override;

			FilesystemModuleResolver& operator=(const FilesystemModuleResolver&) = delete;
			FilesystemModuleResolver& operator=(FilesystemModuleResolver&&) noexcept = delete;

			static constexpr const char* ModuleExtension = ".nzsl";

		private:
			void OnFileAdded(std::string_view directory, std::string_view filename);
			void OnFileRemoved(std::string_view directory, std::string_view filename);
			void OnFileMoved(std::string_view directory, std::string_view filename, std::string_view oldFilename);
			void OnFileUpdated(std::string_view directory, std::string_view filename);

			std::unordered_map<std::string, std::string> m_moduleByFilepath;
			std::unordered_map<std::string, ShaderAst::ModulePtr> m_modules;
			MovablePtr<void> m_fileWatcher;
	};
}

#include <Nazara/Shader/FilesystemModuleResolver.inl>

#endif // NAZARA_SHADER_FILESYSTEMMODULERESOLVER_HPP
