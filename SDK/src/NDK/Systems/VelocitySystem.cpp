// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Systems/VelocitySystem.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/Components/PhysicsComponent.hpp>
#include <NDK/Components/VelocityComponent.hpp>

namespace Ndk
{
	VelocitySystem::VelocitySystem()
	{
		Requires<NodeComponent, VelocityComponent>();
		Excludes<PhysicsComponent>();
	}

	void VelocitySystem::OnUpdate(float elapsedTime)
	{
		for (const Ndk::EntityHandle& entity : GetEntities())
		{
			NodeComponent& node = entity->GetComponent<NodeComponent>();
			const VelocityComponent& velocity = entity->GetComponent<VelocityComponent>();

			node.Move(velocity.linearVelocity * elapsedTime, Nz::CoordSys_Global);
		}
	}

	SystemIndex VelocitySystem::systemIndex;
}
