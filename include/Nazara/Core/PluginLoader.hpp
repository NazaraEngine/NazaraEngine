// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_PLUGINLOADER_HPP
#define NAZARA_CORE_PLUGINLOADER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/Plugin.hpp>
#include <filesystem>
#include <vector>

namespace Nz
{
	class PluginInterface;

	class NAZARA_CORE_API PluginLoader
	{
		public:
			PluginLoader();
			PluginLoader(const PluginLoader&) = delete;
			PluginLoader(PluginLoader&&) = delete;
			~PluginLoader() = default;

			void AddSearchDirectory(const std::filesystem::path& directoryPath);

			template<typename T> Plugin<T> Load(bool activate = true);
			GenericPlugin Load(const std::filesystem::path& pluginPath, bool activate = true);

			void RemoveSearchDirectory(const std::filesystem::path& directoryPath);

			PluginLoader& operator=(const PluginLoader&) = delete;
			PluginLoader& operator=(PluginLoader&&) = delete;

		private:
			using PluginLoadCallback = PluginInterface* (*)();

			std::vector<std::filesystem::path> m_directories;
	};
}

#include <Nazara/Core/PluginLoader.inl>

#endif // NAZARA_CORE_PLUGINLOADER_HPP
