// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/PluginLoader.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <Nazara/Core/EnvironmentVariables.hpp>
#include <Nazara/Core/Error.hpp>
#include <NazaraUtils/Algorithm.hpp>
#include <stdexcept>

namespace Nz
{
	void PluginLoader::AddDefaultSearchDirectories()
	{
		AddSearchDirectory(".");
		AddSearchDirectory("plugins");
		if (const char* path = GetEnvironmentVariable("NAZARA_PLUGIN_DIR"))
			AddSearchDirectory(Utf8Path(path));
	}

	void PluginLoader::AddSearchDirectory(const std::filesystem::path& directoryPath)
	{
		std::filesystem::path absoluteDirPath = std::filesystem::absolute(directoryPath);
		if (std::find(m_directories.begin(), m_directories.end(), absoluteDirPath) != m_directories.end())
			m_directories.push_back(std::filesystem::absolute(directoryPath));
	}

	GenericPlugin PluginLoader::Load(const std::filesystem::path& pluginPath, bool activate)
	{
		std::filesystem::path path = pluginPath;
		if (path.extension() != NAZARA_DYNLIB_EXTENSION)
			path += NAZARA_DYNLIB_EXTENSION;

		if (!path.is_absolute())
		{
			for (const std::filesystem::path& dir : m_directories)
			{
				std::filesystem::path testPath = dir / path;
				if (std::filesystem::exists(testPath))
				{
					path = testPath;
					break;
				}
			}
		}

		DynLib library;
		if (!library.Load(path))
			throw std::runtime_error("failed to load plugin");

		PluginLoadCallback loadFunc = reinterpret_cast<PluginLoadCallback>(library.GetSymbol("PluginLoad"));
		if (!loadFunc)
			throw std::runtime_error("failed to load plugin: PluginLoad symbol not found");

		std::unique_ptr<PluginInterface> pluginInterface = std::unique_ptr<PluginInterface>(loadFunc());
		if (!pluginInterface)
			throw std::runtime_error("plugin failed to load");

		if (activate && !pluginInterface->Activate())
			throw std::runtime_error("failed to activate plugin");

		return GenericPlugin(std::move(library), std::move(pluginInterface));
	}

	void PluginLoader::RemoveSearchDirectory(const std::filesystem::path& directoryPath)
	{
		auto it = std::find(m_directories.begin(), m_directories.end(), std::filesystem::absolute(directoryPath));
		if (it != m_directories.end())
			m_directories.erase(it);
	}

}
