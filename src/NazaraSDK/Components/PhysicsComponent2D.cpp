// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NazaraSDK/Components/PhysicsComponent2D.hpp>
#include <Nazara/Physics2D/RigidBody2D.hpp>
#include <NazaraSDK/World.hpp>
#include <NazaraSDK/Components/CollisionComponent2D.hpp>
#include <NazaraSDK/Components/NodeComponent.hpp>
#include <NazaraSDK/Systems/PhysicsSystem2D.hpp>
#include <memory>

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

		Nz::PhysWorld2D& world = entityWorld->GetSystem<PhysicsSystem2D>().GetPhysWorld();

		Nz::Vector2f positionOffset;

		Nz::Collider2DRef geom;
		if (m_entity->HasComponent<CollisionComponent2D>())
		{
			const CollisionComponent2D& entityCollision = m_entity->GetComponent<CollisionComponent2D>();
			geom = entityCollision.GetGeom();
			positionOffset = entityCollision.GetStaticBody()->GetPositionOffset(); //< Calling GetGeomOffset would retrieve current component which is not yet initialized
		}
		else
			positionOffset = Nz::Vector2f::Zero();

		Nz::Matrix4f matrix;
		if (m_entity->HasComponent<NodeComponent>())
			matrix = m_entity->GetComponent<NodeComponent>().GetTransformMatrix();
		else
			matrix.MakeIdentity();

		m_object = std::make_unique<Nz::RigidBody2D>(&world, 1.f, geom);
		m_object->SetPositionOffset(positionOffset);
		m_object->SetUserdata(reinterpret_cast<void*>(static_cast<std::ptrdiff_t>(m_entity->GetId())));

		if (m_entity->HasComponent<NodeComponent>())
		{
			auto& entityNode = m_entity->GetComponent<NodeComponent>();
			m_object->SetPosition(Nz::Vector2f(entityNode.GetPosition()));
			m_object->SetRotation(entityNode.GetRotation().To2DAngle());
		}

		if (m_pendingStates.valid)
			ApplyPhysicsState(*m_object);
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
			m_object->SetGeom(static_cast<CollisionComponent2D&>(component).GetGeom(), false, false);
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
			m_object->SetGeom(Nz::NullCollider2D::New(), false, false);
		}
	}

	/*!
	* \brief Operation to perform when component is detached from an entity
	*/

	void PhysicsComponent2D::OnDetached()
	{
		if (m_object)
		{
			CopyPhysicsState(*m_object);
			m_object.reset();
		}
	}

	void PhysicsComponent2D::OnEntityDestruction()
	{
		// Kill rigidbody before entity destruction to force contact callbacks to be called while the entity is still valid
		m_object.reset();
	}

	void PhysicsComponent2D::OnEntityDisabled()
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->EnableSimulation(false);
	}

	void PhysicsComponent2D::OnEntityEnabled()
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->EnableSimulation(true);
	}

	ComponentIndex PhysicsComponent2D::componentIndex;
}
