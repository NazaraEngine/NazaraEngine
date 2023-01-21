// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Application.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	template<typename... ModuleList>
	Application<ModuleList...>::~Application()
	{
		// Clear components before releasing modules
		ClearComponents();
	}
}

#include <Nazara/Core/DebugOff.hpp>
