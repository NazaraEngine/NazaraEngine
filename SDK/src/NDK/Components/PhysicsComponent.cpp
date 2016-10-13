// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Components/PhysicsComponent.hpp>
#include <Nazara/Physics3D/RigidBody3D.hpp>
#include <NDK/Algorithm.hpp>
#include <NDK/World.hpp>
#include <NDK/Components/CollisionComponent.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/Systems/PhysicsSystem.hpp>

namespace Ndk
{
	/*!
	* \ingroup NDK
	* \class Ndk::PhysicsComponent
	* \brief NDK class that represents the component for physics (meant for dynamic objects)
	*/

	/*!
	* \brief Operation to perform when component is attached to an entity
	*
	* \remark Produces a NazaraAssert if the world does not have a physics system
	*/

	void PhysicsComponent::OnAttached()
	{
		World* entityWorld = m_entity->GetWorld();
		NazaraAssert(entityWorld->HasSystem<PhysicsSystem>(), "World must have a physics system");

		Nz::PhysWorld3D& world = entityWorld->GetSystem<PhysicsSystem>().GetWorld();

		Nz::Collider3DRef geom;
		if (m_entity->HasComponent<CollisionComponent>())
			geom = m_entity->GetComponent<CollisionComponent>().GetGeom();

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

	void PhysicsComponent::OnComponentAttached(BaseComponent& component)
	{
		if (IsComponent<CollisionComponent>(component))
		{
			NazaraAssert(m_object, "Invalid object");
			m_object->SetGeom(static_cast<CollisionComponent&>(component).GetGeom());
		}
	}

	/*!
	* \brief Operation to perform when component is detached from this component
	*
	* \param component Component being detached
	*
	* \remark Produces a NazaraAssert if physical object is invalid
	*/

	void PhysicsComponent::OnComponentDetached(BaseComponent& component)
	{
		if (IsComponent<CollisionComponent>(component))
		{
			NazaraAssert(m_object, "Invalid object");
			m_object->SetGeom(Nz::NullCollider3D::New());
		}
	}

	/*!
	* \brief Operation to perform when component is detached from an entity
	*/

	void PhysicsComponent::OnDetached()
	{
		m_object.reset();
	}

	ComponentIndex PhysicsComponent::componentIndex;
}
