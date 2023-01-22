// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Application.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	template<typename... ModuleList>
	template<typename... ModuleConfig>
	Application<ModuleList...>::Application(ModuleConfig&&... configs) :
	m_modules(std::forward<ModuleConfig>(configs)...)
	{
	}

	template<typename... ModuleList>
	template<typename... ModuleConfig>
	Application<ModuleList...>::Application(int argc, char** argv, ModuleConfig&&... configs) :
	ApplicationBase(argc, argv),
	m_modules(std::forward<ModuleConfig>(configs)...)
	{
	}
	
	template<typename... ModuleList>
	template<typename... ModuleConfig>
	Application<ModuleList...>::Application(int argc, const Pointer<const char>* argv, ModuleConfig&&... configs) :
	ApplicationBase(argc, argv),
	m_modules(std::forward<ModuleConfig>(configs)...)
	{
	}

	template<typename... ModuleList>
	Application<ModuleList...>::~Application()
	{
		// Clear components before releasing modules
		ClearComponents();
	}
}

#include <Nazara/Core/DebugOff.hpp>
