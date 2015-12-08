// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/PluginManager.hpp>
#include <Nazara/Core/Directory.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/File.hpp>
#include <memory>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	namespace
	{
		using PluginLoad = int (*)();
		using PluginUnload = void (*)();

		String s_pluginFiles[] =
		{
			"NazaraAssimp",  // Plugin_Assimp
			"NazaraFreetype" // Plugin_FreeType
		};
	}

	void PluginManager::AddDirectory(const String& directoryPath)
	{
		if (!Initialize())
		{
			NazaraError("Failed to initialize PluginManager");
			return;
		}

		s_directories.insert(File::AbsolutePath(directoryPath));
	}

	bool PluginManager::Initialize()
	{
		if (s_initialized)
			return true;

		s_initialized = true;

		AddDirectory(".");
		AddDirectory("plugins");

		return true;
	}

	bool PluginManager::Mount(Plugin plugin)
	{
		return Mount(s_pluginFiles[plugin]);
	}

	bool PluginManager::Mount(const String& pluginPath, bool appendExtension)
	{
		if (!Initialize())
		{
			NazaraError("Failed to initialize PluginManager");
			return false;
		}

		String path = pluginPath;
		if (appendExtension && !path.EndsWith(NAZARA_DYNLIB_EXTENSION))
			path += NAZARA_DYNLIB_EXTENSION;

		bool exists = false;
		if (!File::IsAbsolute(path))
		{
			for (const String& dir : s_directories)
			{
				String testPath;
				testPath.Reserve(dir.GetSize() + path.GetSize() + 10);

				testPath = dir;
				testPath += NAZARA_DIRECTORY_SEPARATOR;
				testPath += path;

				if (File::Exists(testPath))
				{
					path = testPath;
					exists = true;
					break;
				}
			}
		}
		else if (File::Exists(path))
			exists = true;

		if (!exists)
		{
			NazaraError("Failed to find plugin file");
			return false;
		}

		std::unique_ptr<DynLib> library(new DynLib);
		if (!library->Load(path))
		{
			NazaraError("Failed to load plugin");
			return false;
		}

		PluginLoad func = reinterpret_cast<PluginLoad>(library->GetSymbol("PluginLoad"));
		if (!func)
		{
			NazaraError("Failed to get symbol PluginLoad");
			return false;
		}

		if (!func())
		{
			NazaraError("Plugin failed to load");
			return false;
		}

		s_plugins[pluginPath] = library.release();

		return true;
	}

	void PluginManager::RemoveDirectory(const String& directoryPath)
	{
		if (!Initialize())
		{
			NazaraError("Failed to initialize PluginManager");
			return;
		}

		s_directories.erase(File::AbsolutePath(directoryPath));
	}

	void PluginManager::Unmount(Plugin plugin)
	{
		Unmount(s_pluginFiles[plugin]);
	}

	void PluginManager::Unmount(const String& pluginPath)
	{
		if (!Initialize())
		{
			NazaraError("Failed to initialize PluginManager");
			return;
		}

		auto it = s_plugins.find(pluginPath);
		if (it == s_plugins.end())
		{
			NazaraError("Plugin not loaded");
			return;
		}

		PluginUnload func = reinterpret_cast<PluginUnload>(it->second->GetSymbol("PluginUnload"));
		if (func)
			func();

		it->second->Unload();
		delete it->second;

		s_plugins.erase(it);
	}

	void PluginManager::Uninitialize()
	{
		if (!s_initialized)
			return;

		s_initialized = false;

		s_directories.clear();

		for (auto& pair : s_plugins)
		{
			PluginUnload func = reinterpret_cast<PluginUnload>(pair.second->GetSymbol("PluginUnload"));
			if (func)
				func();

			pair.second->Unload();
			delete pair.second;
		}

		s_plugins.clear();
	}

	std::set<String> PluginManager::s_directories;
	std::unordered_map<String, DynLib*> PluginManager::s_plugins;
	bool PluginManager::s_initialized = false;
}
