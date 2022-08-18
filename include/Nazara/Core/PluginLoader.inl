// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/PluginLoader.hpp>
#include <Nazara/Utils/Algorithm.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	template<typename T>
	Plugin<T> PluginLoader::Load(bool activate)
	{
		GenericPlugin plugin = Load(Utf8Path(T::Filename), activate);
		return std::move(plugin).Cast<T>();
	}
}

#include <Nazara/Core/DebugOff.hpp>
