// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Systems/VelocitySystem.hpp>
#include <Nazara/Core/Components/NodeComponent.hpp>
#include <Nazara/Core/Components/VelocityComponent.hpp>

namespace Nz
{
	void VelocitySystem::Update(Time elapsedTime)
	{
		float delta = elapsedTime.AsSeconds();

		m_world.each([&](NodeComponent& nodeComponent, VelocityComponent& velocityComponent)
		{
			nodeComponent.Move(velocityComponent.GetLinearVelocity() * delta);
		});
	}
}
