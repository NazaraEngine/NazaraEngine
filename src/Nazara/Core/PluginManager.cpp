// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/PluginManager.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <Nazara/Core/Error.hpp>
#include <memory>
#include <Nazara/Core/Debug.hpp>

#ifdef NAZARA_PLATFORM_WINDOWS
#define NAZARA_DYNLIB_PREFIX ""
#else
#define NAZARA_DYNLIB_PREFIX "lib"
#endif

namespace Nz
{
	namespace NAZARA_ANONYMOUS_NAMESPACE
	{
		using PluginLoad = int (*)();
		using PluginUnload = void (*)();

		const char* s_pluginFiles[] =
		{
			NAZARA_DYNLIB_PREFIX "PluginAssimp",  // Plugin::Assimp
			NAZARA_DYNLIB_PREFIX "PluginFFmpeg",  // Plugin::FFmpeg
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

	void PluginManager::AddDirectory(const std::filesystem::path& directoryPath)
	{
		if (!Initialize())
		{
			NazaraError("Failed to initialize PluginManager");
			return;
		}

		s_directories.insert(std::filesystem::absolute(directoryPath));
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
		NAZARA_USE_ANONYMOUS_NAMESPACE

		std::filesystem::path pluginName = s_pluginFiles[UnderlyingCast(plugin)];

		#ifdef NAZARA_DEBUG
		std::filesystem::path debugPath = pluginName;
		debugPath += "-d";

		if (Mount(debugPath, true))
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

	bool PluginManager::Mount(const std::filesystem::path& pluginPath, bool appendExtension)
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		if (!Initialize())
		{
			NazaraError("Failed to initialize PluginManager");
			return false;
		}

		std::filesystem::path path = pluginPath;
		if (appendExtension && path.extension() != NAZARA_DYNLIB_EXTENSION)
			path += NAZARA_DYNLIB_EXTENSION;

		bool exists = false;
		if (!path.is_absolute())
		{
			for (const std::filesystem::path& dir : s_directories)
			{
				std::filesystem::path testPath = dir / path;
				if (std::filesystem::exists(testPath))
				{
					path = testPath;
					exists = true;
					break;
				}
			}
		}
		else if (std::filesystem::exists(path))
			exists = true;

		if (!exists)
		{
			NazaraError("Failed to find plugin file");
			return false;
		}

		std::unique_ptr<DynLib> library = std::make_unique<DynLib>();
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

		std::filesystem::path canonicalPath = std::filesystem::canonical(path);
		s_plugins[canonicalPath] = std::move(library);

		return true;
	}

	/*!
	* \brief Removes a directory
	*
	* \param directoryPath Path to the directory
	*
	* \remark Produces a NazaraError if not initialized
	*/

	void PluginManager::RemoveDirectory(const std::filesystem::path& directoryPath)
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		if (!Initialize())
		{
			NazaraError("Failed to initialize PluginManager");
			return;
		}

		s_directories.erase(std::filesystem::canonical(directoryPath));
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
		NAZARA_USE_ANONYMOUS_NAMESPACE

		std::filesystem::path pluginName = s_pluginFiles[UnderlyingCast(plugin)];

#ifdef NAZARA_DEBUG
		std::filesystem::path debugPath = pluginName;
		debugPath += "-d";

		Unmount(debugPath);
#endif

		Unmount(pluginName);
	}

	/*!
	* \brief Unmounts the plugin with a path
	*
	* \param pluginPath Path to the plugin
	*
	* \remark Produces a NazaraError if not initialized
	* \remark Produces a NazaraError if plugin is not loaded
	*/

	void PluginManager::Unmount(const std::filesystem::path& pluginPath)
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		if (!Initialize())
		{
			NazaraError("Failed to initialize PluginManager");
			return;
		}

		std::filesystem::path path = pluginPath;
		if (path.extension() != NAZARA_DYNLIB_EXTENSION)
			path += NAZARA_DYNLIB_EXTENSION;

		if (!std::filesystem::exists(path))
			return;

		std::filesystem::path canonicalPath = std::filesystem::canonical(path);
		auto it = s_plugins.find(canonicalPath);
		if (it == s_plugins.end())
		{
			NazaraError("Plugin not loaded");
			return;
		}

		PluginUnload func = reinterpret_cast<PluginUnload>(it->second->GetSymbol("PluginUnload"));
		if (func)
			func();

		s_plugins.erase(it);
	}

	/*!
	* \brief Uninitializes the plugin manager
	*/

	void PluginManager::Uninitialize()
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		if (!s_initialized)
			return;

		s_initialized = false;

		s_directories.clear();

		for (auto& pair : s_plugins)
		{
			PluginUnload func = reinterpret_cast<PluginUnload>(pair.second->GetSymbol("PluginUnload"));
			if (func)
				func();
		}

		s_plugins.clear();
	}

	std::set<std::filesystem::path> PluginManager::s_directories;
	std::unordered_map<std::filesystem::path, std::unique_ptr<DynLib>, PluginManager::PathHash> PluginManager::s_plugins;
	bool PluginManager::s_initialized = false;
}
