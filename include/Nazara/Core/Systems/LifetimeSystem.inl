// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Systems/LifetimeSystem.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	inline LifetimeSystem::LifetimeSystem(entt::registry& registry) :
	m_registry(registry)
	{
	}
}

#include <Nazara/Core/DebugOff.hpp>
