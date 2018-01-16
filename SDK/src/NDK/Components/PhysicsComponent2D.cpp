// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NDK/Components/PhysicsComponent2D.hpp>
#include <Nazara/Physics2D/RigidBody2D.hpp>
#include <NDK/World.hpp>
#include <NDK/Components/CollisionComponent2D.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/Systems/PhysicsSystem2D.hpp>

namespace Ndk
{
	/*!
	* \ingroup NDK
	* \class Ndk::PhysicsComponent2D
	* \brief NDK class that represents a physics point, without any collision
	*/

	/*!
	* \brief Operation to perform when component is attached to an entity
	*
	* \remark Produces a NazaraAssert if the world does not have a physics system
	*/

	void PhysicsComponent2D::OnAttached()
	{
		World* entityWorld = m_entity->GetWorld();
		NazaraAssert(entityWorld->HasSystem<PhysicsSystem2D>(), "World must have a 2D physics system");

		Nz::PhysWorld2D& world = entityWorld->GetSystem<PhysicsSystem2D>().GetWorld();

		Nz::Collider2DRef geom;
		if (m_entity->HasComponent<CollisionComponent2D>())
			geom = m_entity->GetComponent<CollisionComponent2D>().GetGeom();

		Nz::Matrix4f matrix;
		if (m_entity->HasComponent<NodeComponent>())
			matrix = m_entity->GetComponent<NodeComponent>().GetTransformMatrix();
		else
			matrix.MakeIdentity();

		m_object.reset(new Nz::RigidBody2D(&world, 1.f, geom));
		m_object->SetPosition(Nz::Vector2f(matrix.GetTranslation()));
	}

	/*!
	* \brief Operation to perform when component is attached to this component
	*
	* \param component Component being attached
	*
	* \remark Produces a NazaraAssert if physical object is invalid
	*/

	void PhysicsComponent2D::OnComponentAttached(BaseComponent& component)
	{
		if (IsComponent<CollisionComponent2D>(component))
		{
			NazaraAssert(m_object, "Invalid object");
			m_object->SetGeom(static_cast<CollisionComponent2D&>(component).GetGeom());
		}
	}

	/*!
	* \brief Operation to perform when component is detached from this component
	*
	* \param component Component being detached
	*
	* \remark Produces a NazaraAssert if physical object is invalid
	*/

	void PhysicsComponent2D::OnComponentDetached(BaseComponent& component)
	{
		if (IsComponent<CollisionComponent2D>(component))
		{
			NazaraAssert(m_object, "Invalid object");
			m_object->SetGeom(Nz::NullCollider2D::New());
		}
	}

	/*!
	* \brief Operation to perform when component is detached from an entity
	*/

	void PhysicsComponent2D::OnDetached()
	{
		m_object.reset();
	}

	void PhysicsComponent2D::OnEntityDestruction()
	{
		// Kill rigidbody before entity destruction to force contact callbacks to be called while the entity is still valid
		m_object.reset();
	}

	ComponentIndex PhysicsComponent2D::componentIndex;
}
