// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <NazaraUtils/PathUtils.hpp>

namespace Nz
{
	template<typename T>
	Plugin<T> PluginLoader::Load(bool activate)
	{
#ifdef NAZARA_PLUGINS_STATIC
		std::unique_ptr<T> pluginInterface = PluginProvider<T>::Instantiate();
		if (activate && !pluginInterface->Activate())
			throw std::runtime_error("failed to activate plugin");

		return Plugin<T>({}, std::move(pluginInterface), activate);
#else
		GenericPlugin plugin = Load(Utf8Path(T::Filename), activate);
		return std::move(plugin).Cast<T>();
#endif
	}
}

