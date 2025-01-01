// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	inline PluginManagerAppComponent::PluginManagerAppComponent(ApplicationBase& app, bool addDefaultSearchDirectories) :
	ApplicationComponent(app),
	m_loader(addDefaultSearchDirectories)
	{
	}

	inline void PluginManagerAppComponent::AddDefaultSearchDirectories()
	{
		m_loader.AddDefaultSearchDirectories();
	}

	inline void Nz::PluginManagerAppComponent::AddSearchDirectory(const std::filesystem::path& directoryPath)
	{
		m_loader.AddSearchDirectory(directoryPath);
	}

	template<typename T>
	T& PluginManagerAppComponent::Load(bool activate)
	{
		return m_store.Load<T>(m_loader, activate);
	}

	inline PluginInterface& PluginManagerAppComponent::Load(const std::filesystem::path& pluginPath, bool activate)
	{
		return m_store.Load(m_loader, pluginPath, activate);
	}

	inline void PluginManagerAppComponent::RemoveSearchDirectory(const std::filesystem::path& directoryPath)
	{
		m_loader.RemoveSearchDirectory(directoryPath);
	}
}
