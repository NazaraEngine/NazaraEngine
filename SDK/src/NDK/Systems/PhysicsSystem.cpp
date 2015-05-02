// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Systems/PhysicsSystem.hpp>
#include <Nazara/Physics/PhysObject.hpp>
#include <NDK/Components/CollisionComponent.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/Components/PhysicsComponent.hpp>

namespace Ndk
{
	PhysicsSystem::PhysicsSystem()
	{
		Requires<NodeComponent, PhysicsComponent>();
	}

	PhysicsSystem::PhysicsSystem(const PhysicsSystem& system) :
	System(system),
	m_world()
	{
	}

	void PhysicsSystem::Update(float elapsedTime)
	{
		m_world.Step(elapsedTime);

		for (const Ndk::EntityHandle& entity : GetEntities())
		{
			NodeComponent& node = entity->GetComponent<NodeComponent>();
			PhysicsComponent& phys = entity->GetComponent<PhysicsComponent>();

			NzPhysObject& physObj = phys.GetPhysObject();
			node.SetRotation(physObj.GetRotation(), nzCoordSys_Global);
			node.SetPosition(physObj.GetPosition(), nzCoordSys_Global);
		}
	}

	SystemIndex PhysicsSystem::systemIndex;
}
