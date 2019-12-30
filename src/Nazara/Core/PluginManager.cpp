// Copyright (C) 2017 Jérôme Leclercq
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

		std::string s_pluginFiles[] =
		{
			"PluginAssimp",  // Plugin_Assimp
		};
	}

	/*!
	* \ingroup core
	* \class Nz::PluginManager
	* \brief Core class that represents a manager for plugin
	*/

	/*!
	* \brief Adds a directory
	*
	* \param directoryPath Path to the directory
	*
	* \remark Produces a NazaraError if not initialized
	*/

	void PluginManager::AddDirectory(const std::string& directoryPath)
	{
		if (!Initialize())
		{
			NazaraError("Failed to initialize PluginManager");
			return;
		}

		s_directories.insert(File::AbsolutePath(directoryPath).ToStdString());
	}

	/*!
	* \brief Initializes the plugin manager
	* \return true if everything is ok
	*/

	bool PluginManager::Initialize()
	{
		if (s_initialized)
			return true;

		s_initialized = true;

		AddDirectory(".");
		AddDirectory("plugins");

		return true;
	}

	/*!
	* \brief Mounts the plugin
	* \return true if mounting was a success
	*
	* \remark Produces a NazaraError if not initialized
	* \remark Produces a NazaraError if plugin is not found
	* \remark Produces a NazaraError if fail to load plugin
	* \remark Produces a NazaraError if fail to get symbol PluginLoad
	* \remark Produces a NazaraError if fail to initialize the plugin with PluginLoad
	*/

	bool PluginManager::Mount(Plugin plugin)
	{
		const std::string &pluginName = s_pluginFiles[plugin];
		#ifdef NAZARA_DEBUG
		if (Mount(pluginName + "-d", true))
			return true;
		#endif

		return Mount(pluginName, true);
	}

	/*!
	* \brief Mounts the plugin with a path
	* \return true if mounting was a success
	*
	* \param pluginPath Path to the plugin
	* \param appendExtension Adds the extension to the path or not
	*
	* \remark Produces a NazaraError if not initialized
	* \remark Produces a NazaraError if plugin is not found
	* \remark Produces a NazaraError if fail to load plugin
	* \remark Produces a NazaraError if fail to get symbol PluginLoad
	* \remark Produces a NazaraError if fail to initialize the plugin with PluginLoad
	*/

	bool PluginManager::Mount(const std::string& pluginPath, bool appendExtension)
	{
		if (!Initialize())
		{
			NazaraError("Failed to initialize PluginManager");
			return false;
		}

		std::string path = pluginPath;
		if (appendExtension && !!path.compare(path.length() - sizeof(NAZARA_DYNLIB_EXTENSION) - 1, sizeof(NAZARA_DYNLIB_EXTENSION) - 1, NAZARA_DYNLIB_EXTENSION))
			path += NAZARA_DYNLIB_EXTENSION;

		bool exists = false;
		if (!File::IsAbsolute(path))
		{
			for (const std::string& dir : s_directories)
			{
				std::string testPath;
				testPath.reserve(dir.size() + path.size() + 10);

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

	/*!
	* \brief Removes a directory
	*
	* \param directoryPath Path to the directory
	*
	* \remark Produces a NazaraError if not initialized
	*/

	void PluginManager::RemoveDirectory(const std::string& directoryPath)
	{
		if (!Initialize())
		{
			NazaraError("Failed to initialize PluginManager");
			return;
		}

		s_directories.erase(File::AbsolutePath(directoryPath).ToStdString());
	}

	/*!
	* \brief Unmounts the plugin with a path
	*
	* \param plugin Path to the plugin
	*
	* \remark Produces a NazaraError if not initialized
	* \remark Produces a NazaraError if plugin is not loaded
	*/

	void PluginManager::Unmount(Plugin plugin)
	{
		Unmount(s_pluginFiles[plugin]);
	}

	/*!
	* \brief Unmounts the plugin with a path
	*
	* \param pluginPath Path to the plugin
	*
	* \remark Produces a NazaraError if not initialized
	* \remark Produces a NazaraError if plugin is not loaded
	*/

	void PluginManager::Unmount(const std::string& pluginPath)
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

	/*!
	* \brief Uninitializes the plugin manager
	*/

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

	std::set<std::string> PluginManager::s_directories;
	std::unordered_map<std::string, DynLib*> PluginManager::s_plugins;
	bool PluginManager::s_initialized = false;
}
