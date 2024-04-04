// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_PLUGINLOADER_HPP
#define NAZARA_CORE_PLUGINLOADER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/Export.hpp>
#include <Nazara/Core/Plugin.hpp>
#include <filesystem>
#include <vector>

namespace Nz
{
	class PluginInterface;

	class NAZARA_CORE_API PluginLoader
	{
		public:
			inline PluginLoader(bool addDefaultDirectories = true);
			PluginLoader(const PluginLoader&) = delete;
			PluginLoader(PluginLoader&&) = delete;
			~PluginLoader() = default;

			void AddDefaultSearchDirectories();
			void AddSearchDirectory(const std::filesystem::path& directoryPath);

			template<typename T> [[nodiscard]] Plugin<T> Load(bool activate = true);
			[[nodiscard]] GenericPlugin Load(const std::filesystem::path& pluginPath, bool activate = true);

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
