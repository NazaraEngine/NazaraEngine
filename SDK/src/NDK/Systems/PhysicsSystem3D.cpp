// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Systems/PhysicsSystem3D.hpp>
#include <Nazara/Physics3D/RigidBody3D.hpp>
#include <NDK/Components/CollisionComponent3D.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/Components/PhysicsComponent2D.hpp>
#include <NDK/Components/PhysicsComponent3D.hpp>

namespace Ndk
{
	/*!
	* \ingroup NDK
	* \class Ndk::PhysicsSystem
	* \brief NDK class that represents the physics system
	*
	* \remark This system is enabled if the entity has the trait: NodeComponent and any of these two: CollisionComponent3D or PhysicsComponent3D
	* \remark Static objects do not have a velocity specified by the physical engine
	*/

	/*!
	* \brief Constructs an PhysicsSystem object by default
	*/

	PhysicsSystem3D::PhysicsSystem3D()
	{
		Requires<NodeComponent>();
		RequiresAny<CollisionComponent3D, PhysicsComponent3D>();
		Excludes<PhysicsComponent2D>();
	}

	void PhysicsSystem3D::CreatePhysWorld() const
	{
		NazaraAssert(!m_world, "Physics world should not be created twice");

		m_world = std::make_unique<Nz::PhysWorld3D>();
	}

	/*!
	* \brief Operation to perform when entity is validated for the system
	*
	* \param entity Pointer to the entity
	* \param justAdded Is the entity newly added
	*/

	void PhysicsSystem3D::OnEntityValidation(Entity* entity, bool justAdded)
	{
		if (entity->HasComponent<PhysicsComponent3D>())
		{
			if (entity->GetComponent<PhysicsComponent3D>().IsNodeSynchronizationEnabled())
				m_dynamicObjects.Insert(entity);
			else
				m_dynamicObjects.Remove(entity);

			m_staticObjects.Remove(entity);
		}
		else
		{
			m_dynamicObjects.Remove(entity);
			m_staticObjects.Insert(entity);
		}

		if (!m_world)
			CreatePhysWorld();
	}

	/*!
	* \brief Operation to perform when system is updated
	*
	* \param elapsedTime Delta time used for the update
	*/

	void PhysicsSystem3D::OnUpdate(float elapsedTime)
	{
		if (!m_world)
			return;

		m_world->Step(elapsedTime);

		for (const Ndk::EntityHandle& entity : m_dynamicObjects)
		{
			NodeComponent& node = entity->GetComponent<NodeComponent>();
			PhysicsComponent3D& phys = entity->GetComponent<PhysicsComponent3D>();

			Nz::RigidBody3D* physObj = phys.GetRigidBody();
			node.SetRotation(physObj->GetRotation(), Nz::CoordSys_Global);
			node.SetPosition(physObj->GetPosition(), Nz::CoordSys_Global);
		}

		float invElapsedTime = 1.f / elapsedTime;
		for (const Ndk::EntityHandle& entity : m_staticObjects)
		{
			CollisionComponent3D& collision = entity->GetComponent<CollisionComponent3D>();
			NodeComponent& node = entity->GetComponent<NodeComponent>();

			Nz::RigidBody3D* physObj = collision.GetStaticBody();

			Nz::Quaternionf oldRotation = physObj->GetRotation();
			Nz::Vector3f oldPosition = physObj->GetPosition();
			Nz::Quaternionf newRotation = node.GetRotation(Nz::CoordSys_Global);
			Nz::Vector3f newPosition = node.GetPosition(Nz::CoordSys_Global);

			// To move static objects and ensure their collisions, we have to specify them a velocity
			// (/!\: the physical motor does not apply the speed on static objects)
			if (newPosition != oldPosition)
			{
				physObj->SetPosition(newPosition);
				physObj->SetLinearVelocity((newPosition - oldPosition) * invElapsedTime);
			}
			else
				physObj->SetLinearVelocity(Nz::Vector3f::Zero());

			if (newRotation != oldRotation)
			{
				Nz::Quaternionf transition = newRotation * oldRotation.GetConjugate();
				Nz::EulerAnglesf angles = transition.ToEulerAngles();
				Nz::Vector3f angularVelocity(Nz::ToRadians(angles.pitch * invElapsedTime),
				                             Nz::ToRadians(angles.yaw * invElapsedTime),
				                             Nz::ToRadians(angles.roll * invElapsedTime));

				physObj->SetRotation(oldRotation);
				physObj->SetAngularVelocity(angularVelocity);
			}
			else
				physObj->SetAngularVelocity(Nz::Vector3f::Zero());
		}
	}

	SystemIndex PhysicsSystem3D::systemIndex;
}
