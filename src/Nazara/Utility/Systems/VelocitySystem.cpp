// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Systems/VelocitySystem.hpp>
#include <Nazara/Core/Components/DisabledComponent.hpp>
#include <Nazara/Utility/Components/NodeComponent.hpp>
#include <Nazara/Utility/Components/VelocityComponent.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	void VelocitySystem::Update(Time elapsedTime)
	{
		float delta = elapsedTime.AsSeconds();

		auto view = m_registry.view<NodeComponent, VelocityComponent>(entt::exclude<DisabledComponent>);
		for (auto [entity, nodeComponent, velocityComponent] : view.each())
		{
			NazaraUnused(entity);
			nodeComponent.Move(velocityComponent.GetLinearVelocity() * delta);
		}
	}
}
