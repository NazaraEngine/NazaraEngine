// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Systems/VelocitySystem.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/Components/PhysicsComponent2D.hpp>
#include <NDK/Components/PhysicsComponent3D.hpp>
#include <NDK/Components/VelocityComponent.hpp>

namespace Ndk
{
	/*!
	* \ingroup NDK
	* \class Ndk::VelocitySystem
	* \brief NDK class that represents the velocity system
	*
	* \remark This system is enabled if the entity owns the traits NodeComponent and VelocityComponent
	* but it's disabled with the traits: PhysicsComponent2D, PhysicsComponent3D
	*/

	/*!
	* \brief Constructs an VelocitySystem object by default
	*/

	VelocitySystem::VelocitySystem()
	{
		Excludes<PhysicsComponent2D, PhysicsComponent3D>();
		Requires<NodeComponent, VelocityComponent>();
		SetUpdateOrder(10); //< Since some systems may want to stop us
	}

	/*!
	* \brief Operation to perform when system is updated
	*
	* \param elapsedTime Delta time used for the update
	*/

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
