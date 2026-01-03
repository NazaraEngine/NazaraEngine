// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <NazaraUtils/PathUtils.hpp>
#include <NazaraUtils/TypeTraits.hpp>

namespace Nz
{
	inline PluginLoader::PluginLoader(bool addDefaultDirectories)
	{
		if (addDefaultDirectories)
			AddDefaultSearchDirectories();
	}

	template<typename T>
	Plugin<T> PluginLoader::Load(bool activate)
	{
		if constexpr (IsComplete_v<StaticPluginProvider<T>>)
		{
			std::unique_ptr<T> pluginInterface = StaticPluginProvider<T>::Instantiate();
			if (activate && !pluginInterface->Activate())
				throw std::runtime_error("failed to activate plugin");

			return Plugin<T>({}, std::move(pluginInterface));
		}
		else
		{
			GenericPlugin plugin = Load(Utf8Path(T::Filename), activate);
			return std::move(plugin).Cast<T>();
		}
	}
}
