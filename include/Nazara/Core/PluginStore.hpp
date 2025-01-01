// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_PLUGINSTORE_HPP
#define NAZARA_CORE_PLUGINSTORE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/PluginLoader.hpp>
#include <vector>

namespace Nz
{
	class PluginStore
	{
		public:
			PluginStore() = default;
			PluginStore(const PluginStore&) = delete;
			PluginStore(PluginStore&&) = default;
			~PluginStore();

			template<typename T> T& Load(PluginLoader& loader, bool activate = true);
			inline PluginInterface& Load(PluginLoader& loader, const std::filesystem::path& pluginPath, bool activate = true);

			inline void Unload(PluginInterface& interface);

			PluginStore& operator=(const PluginStore&) = delete;
			PluginStore& operator=(PluginStore&&) noexcept = default;

		private:
			std::vector<GenericPlugin> m_loadedPlugins;
	};
}

#include <Nazara/Core/PluginStore.inl>

#endif // NAZARA_CORE_PLUGINSTORE_HPP
