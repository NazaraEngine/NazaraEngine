// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_PLUGINMANAGERAPPCOMPONENT_HPP
#define NAZARA_CORE_PLUGINMANAGERAPPCOMPONENT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/ApplicationComponent.hpp>
#include <Nazara/Core/PluginLoader.hpp>
#include <Nazara/Core/PluginStore.hpp>
#include <vector>

namespace Nz
{
	class PluginManagerAppComponent final : public ApplicationComponent
	{
		public:
			inline PluginManagerAppComponent(ApplicationBase& app, bool addDefaultSearchDirectories = true);
			PluginManagerAppComponent(const PluginManagerAppComponent&) = delete;
			PluginManagerAppComponent(PluginManagerAppComponent&&) = delete;
			~PluginManagerAppComponent() = default;

			void AddDefaultSearchDirectories();
			void AddSearchDirectory(const std::filesystem::path& directoryPath);

			template<typename T> T& Load(bool activate = true);
			inline PluginInterface& Load(const std::filesystem::path& pluginPath, bool activate = true);

			void RemoveSearchDirectory(const std::filesystem::path& directoryPath);

			PluginManagerAppComponent& operator=(const PluginManagerAppComponent&) = delete;
			PluginManagerAppComponent& operator=(PluginManagerAppComponent&&) = delete;

		private:
			PluginLoader m_loader;
			PluginStore m_store;
	};
}

#include <Nazara/Core/PluginManagerAppComponent.inl>

#endif // NAZARA_CORE_PLUGINMANAGERAPPCOMPONENT_HPP
