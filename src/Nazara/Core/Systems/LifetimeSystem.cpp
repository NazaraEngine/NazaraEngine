// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Systems/LifetimeSystem.hpp>
#include <Nazara/Core/Components/DisabledComponent.hpp>
#include <Nazara/Core/Components/LifetimeComponent.hpp>

namespace Nz
{
	void LifetimeSystem::Update(Time elapsedTime)
	{
		m_world.defer_begin();
		m_world.each([&](flecs::entity entity, LifetimeComponent& lifetimeComponent)
		{
			lifetimeComponent.DecreaseLifetime(elapsedTime);
			if (!lifetimeComponent.IsAlive())
				entity.destruct();
		});
		m_world.defer_end();
	}
}
