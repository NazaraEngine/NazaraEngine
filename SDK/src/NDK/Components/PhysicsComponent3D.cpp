// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Components/PhysicsComponent3D.hpp>
#include <Nazara/Physics3D/RigidBody3D.hpp>
#include <NDK/World.hpp>
#include <NDK/Components/CollisionComponent3D.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/Systems/PhysicsSystem3D.hpp>

namespace Ndk
{
	/*!
	* \ingroup NDK
	* \class Ndk::PhysicsComponent3D
	* \brief NDK class that represents the component for physics (meant for dynamic objects)
	*/

	/*!
	* \brief Operation to perform when component is attached to an entity
	*
	* \remark Produces a NazaraAssert if the world does not have a physics system
	*/

	void PhysicsComponent3D::OnAttached()
	{
		World* entityWorld = m_entity->GetWorld();
		NazaraAssert(entityWorld->HasSystem<PhysicsSystem3D>(), "World must have a physics system");

		Nz::PhysWorld3D& world = entityWorld->GetSystem<PhysicsSystem3D>().GetWorld();

		Nz::Collider3DRef geom;
		if (m_entity->HasComponent<CollisionComponent3D>())
			geom = m_entity->GetComponent<CollisionComponent3D>().GetGeom();

		Nz::Matrix4f matrix;
		if (m_entity->HasComponent<NodeComponent>())
			matrix = m_entity->GetComponent<NodeComponent>().GetTransformMatrix();
		else
			matrix.MakeIdentity();

		m_object.reset(new Nz::RigidBody3D(&world, geom, matrix));
		m_object->SetMass(1.f);
	}

	/*!
	* \brief Operation to perform when component is attached to this component
	*
	* \param component Component being attached
	*
	* \remark Produces a NazaraAssert if physical object is invalid
	*/

	void PhysicsComponent3D::OnComponentAttached(BaseComponent& component)
	{
		if (IsComponent<CollisionComponent3D>(component))
		{
			NazaraAssert(m_object, "Invalid object");
			m_object->SetGeom(static_cast<CollisionComponent3D&>(component).GetGeom());
		}
	}

	/*!
	* \brief Operation to perform when component is detached from this component
	*
	* \param component Component being detached
	*
	* \remark Produces a NazaraAssert if physical object is invalid
	*/

	void PhysicsComponent3D::OnComponentDetached(BaseComponent& component)
	{
		if (IsComponent<CollisionComponent3D>(component))
		{
			NazaraAssert(m_object, "Invalid object");
			m_object->SetGeom(Nz::NullCollider3D::New());
		}
	}

	/*!
	* \brief Operation to perform when component is detached from an entity
	*/

	void PhysicsComponent3D::OnDetached()
	{
		m_object.reset();
	}

	void PhysicsComponent3D::OnEntityDestruction()
	{
		// Kill rigid body before entity destruction to force contact callbacks to be called while the entity is still valid
		m_object.reset();
	}

	ComponentIndex PhysicsComponent3D::componentIndex;
}
