// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Systems/LifetimeSystem.hpp>
#include <Nazara/Core/Components/DisabledComponent.hpp>
#include <Nazara/Core/Components/LifetimeComponent.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	void LifetimeSystem::Update(Time elapsedTime)
	{
		auto view = m_registry.view<LifetimeComponent>(entt::exclude<DisabledComponent>);
		for (auto [entity, lifetimeComponent] : view.each())
		{
			lifetimeComponent.DecreaseLifetime(elapsedTime);
			if (!lifetimeComponent.IsAlive())
				m_registry.destroy(entity);
		}
	}
}
