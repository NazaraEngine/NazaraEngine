// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PLUGINMANAGER_HPP
#define NAZARA_PLUGINMANAGER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/String.hpp>
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

			static void AddDirectory(const String& directoryPath);

			static bool Initialize();

			static bool Mount(Plugin plugin);
			static bool Mount(const String& pluginPath, bool appendExtension = true);

			static void RemoveDirectory(const String& directoryPath);

			static void Unmount(Plugin plugin);
			static void Unmount(const String& pluginPath);

			static void Uninitialize();

		private:
			static std::set<String> s_directories;
			static std::unordered_map<String, DynLib*> s_plugins;
			static bool s_initialized;
	};
}

#endif // NAZARA_PLUGINMANAGER_HPP
