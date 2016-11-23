// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Systems/PhysicsSystem2D.hpp>
#include <Nazara/Physics2D/RigidBody2D.hpp>
#include <NDK/Components/CollisionComponent2D.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/Components/PhysicsComponent2D.hpp>
#include <NDK/Components/PhysicsComponent3D.hpp>

namespace Ndk
{
	/*!
	* \ingroup NDK
	* \class Ndk::PhysicsSystem2D
	* \brief NDK class that represents a two-dimensional physics system
	*
	* \remark This system is enabled if the entity has the trait: NodeComponent and any of these two: CollisionComponent3D or PhysicsComponent3D
	* \remark Static objects do not have a velocity specified by the physical engine
	*/

	/*!
	* \brief Constructs an PhysicsSystem object by default
	*/

	PhysicsSystem2D::PhysicsSystem2D()
	{
		Requires<NodeComponent>();
		RequiresAny<CollisionComponent2D, PhysicsComponent2D>();
		Excludes<PhysicsComponent3D>();
	}

	/*!
	* \brief Constructs a PhysicsSystem object by copy semantic
	*
	* \param system PhysicsSystem to copy
	*/

	PhysicsSystem2D::PhysicsSystem2D(const PhysicsSystem2D& system) :
	System(system),
	m_world()
	{
	}

	void PhysicsSystem2D::CreatePhysWorld() const
	{
		NazaraAssert(!m_world, "Physics world should not be created twice");

		m_world = std::make_unique<Nz::PhysWorld2D>();
	}

	/*!
	* \brief Operation to perform when entity is validated for the system
	*
	* \param entity Pointer to the entity
	* \param justAdded Is the entity newly added
	*/

	void PhysicsSystem2D::OnEntityValidation(Entity* entity, bool justAdded)
	{
		// It's possible our entity got revalidated because of the addition/removal of a PhysicsComponent3D
		if (!justAdded)
		{
			// We take the opposite array from which the entity should belong to
			auto& entities = (entity->HasComponent<PhysicsComponent2D>()) ? m_staticObjects : m_dynamicObjects;
			entities.Remove(entity);
		}

		auto& entities = (entity->HasComponent<PhysicsComponent2D>()) ? m_dynamicObjects : m_staticObjects;
		entities.Insert(entity);

		if (!m_world)
			CreatePhysWorld();
	}

	/*!
	* \brief Operation to perform when system is updated
	*
	* \param elapsedTime Delta time used for the update
	*/

	void PhysicsSystem2D::OnUpdate(float elapsedTime)
	{
		if (!m_world)
			return;

		m_world->Step(elapsedTime);

		for (const Ndk::EntityHandle& entity : m_dynamicObjects)
		{
			NodeComponent& node = entity->GetComponent<NodeComponent>();
			PhysicsComponent2D& phys = entity->GetComponent<PhysicsComponent2D>();

			Nz::RigidBody2D& body = phys.GetRigidBody();
			node.SetRotation(Nz::EulerAnglesf(0.f, 0.f, body.GetRotation()), Nz::CoordSys_Global);
			node.SetPosition(Nz::Vector3f(body.GetPosition(), node.GetPosition(Nz::CoordSys_Global).z), Nz::CoordSys_Global);
		}

		float invElapsedTime = 1.f / elapsedTime;
		for (const Ndk::EntityHandle& entity : m_staticObjects)
		{
			CollisionComponent2D& collision = entity->GetComponent<CollisionComponent2D>();
			NodeComponent& node = entity->GetComponent<NodeComponent>();

			Nz::RigidBody2D* body = collision.GetStaticBody();

			Nz::Vector2f oldPosition = body->GetPosition();
			Nz::Vector2f newPosition = Nz::Vector2f(node.GetPosition(Nz::CoordSys_Global));

			// To move static objects and ensure their collisions, we have to specify them a velocity
			// (/!\: the physical motor does not apply the speed on static objects)
			if (newPosition != oldPosition)
			{
				body->SetPosition(newPosition);
				body->SetVelocity((newPosition - oldPosition) * invElapsedTime);
			}
			else
				body->SetVelocity(Nz::Vector2f::Zero());

/*
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
*/
		}
	}

	SystemIndex PhysicsSystem2D::systemIndex;
}
