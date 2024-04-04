// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	template<typename T>
	T& PluginStore::Load(PluginLoader& loader, bool activate)
	{
		GenericPlugin& plugin = m_loadedPlugins.emplace_back(loader.Load<T>(activate));
		return static_cast<T&>(plugin.GetInterface());
	}

	inline PluginStore::~PluginStore()
	{
		// std::vector does not guarantee order of destruction, do it ourselves
		while (!m_loadedPlugins.empty())
			m_loadedPlugins.pop_back();
	}

	inline PluginInterface& PluginStore::Load(PluginLoader& loader, const std::filesystem::path& pluginPath, bool activate)
	{
		GenericPlugin& plugin = m_loadedPlugins.emplace_back(loader.Load(pluginPath, activate));
		return plugin.GetInterface();
	}

	inline void PluginStore::Unload(PluginInterface& interface)
	{
		auto it = std::find_if(m_loadedPlugins.begin(), m_loadedPlugins.end(), [&](const GenericPlugin& plugin)
		{
			return &plugin.GetInterface() == &interface;
		});

		if (it != m_loadedPlugins.end())
			m_loadedPlugins.erase(it);
	}
}
