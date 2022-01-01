// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_PLUGINMANAGER_HPP
#define NAZARA_CORE_PLUGINMANAGER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <filesystem>
#include <set>
#include <unordered_map>

///TODO: Revision
namespace Nz
{
	class DynLib;

	class NAZARA_CORE_API PluginManager
	{
		public:
			PluginManager() = delete;
			~PluginManager() = delete;

			static void AddDirectory(const std::filesystem::path& directoryPath);

			static bool Initialize();

			static bool Mount(Plugin plugin);
			static bool Mount(const std::filesystem::path& pluginPath, bool appendExtension = true);

			static void RemoveDirectory(const std::filesystem::path& directoryPath);

			static void Unmount(Plugin plugin);
			static void Unmount(const std::filesystem::path& pluginPath);

			static void Uninitialize();

		private:
			// https://stackoverflow.com/questions/51065244/is-there-no-standard-hash-for-stdfilesystempath
			struct PathHash
			{
				std::size_t operator()(const std::filesystem::path& p) const
				{
					return hash_value(p);
				}
			};

			static std::set<std::filesystem::path> s_directories;
			static std::unordered_map<std::filesystem::path, std::unique_ptr<DynLib>, PathHash> s_plugins;
			static bool s_initialized;
	};
}

#endif // NAZARA_CORE_PLUGINMANAGER_HPP
