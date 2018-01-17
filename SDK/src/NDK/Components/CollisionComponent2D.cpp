// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NDK/Components/CollisionComponent2D.hpp>
#include <Nazara/Physics2D/RigidBody2D.hpp>
#include <NDK/World.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/Components/PhysicsComponent2D.hpp>
#include <NDK/Systems/PhysicsSystem2D.hpp>

namespace Ndk
{
	/*!
	* \ingroup NDK
	* \class Ndk::CollisionComponent2D
	* \brief NDK class that represents a two-dimensional collision geometry
	*/

	/*!
	* \brief Sets geometry for the entity
	*
	* \param geom Geometry used for collisions
	*
	* \remark Produces a NazaraAssert if the entity has no physics component and has no static body
	*/

	void CollisionComponent2D::SetGeom(Nz::Collider2DRef geom)
	{
		m_geom = std::move(geom);

		if (m_entity->HasComponent<PhysicsComponent2D>())
		{
			// We update the geometry of the PhysiscsObject linked to the PhysicsComponent2D
			PhysicsComponent2D& physComponent = m_entity->GetComponent<PhysicsComponent2D>();
			physComponent.GetRigidBody()->SetGeom(m_geom);
		}
		else
		{
			NazaraAssert(m_staticBody, "An entity without physics component should have a static body");
			m_staticBody->SetGeom(m_geom);
		}
	}

	/*!
	* \brief Initializes the static body
	*
	* \remark Produces a NazaraAssert if entity is invalid
	* \remark Produces a NazaraAssert if entity is not linked to a world, or the world has no physics system
	*/

	void CollisionComponent2D::InitializeStaticBody()
	{
		NazaraAssert(m_entity, "Invalid entity");
		World* entityWorld = m_entity->GetWorld();

		NazaraAssert(entityWorld, "Entity must have world");
		NazaraAssert(entityWorld->HasSystem<PhysicsSystem2D>(), "World must have a physics system");
		Nz::PhysWorld2D& physWorld = entityWorld->GetSystem<PhysicsSystem2D>().GetWorld();

		m_staticBody = std::make_unique<Nz::RigidBody2D>(&physWorld, 0.f, m_geom);

		Nz::Matrix4f matrix;
		if (m_entity->HasComponent<NodeComponent>())
			matrix = m_entity->GetComponent<NodeComponent>().GetTransformMatrix();
		else
			matrix.MakeIdentity();

		m_staticBody->SetPosition(Nz::Vector2f(matrix.GetTranslation()));

	}

	/*!
	* \brief Operation to perform when component is attached to an entity
	*/

	void CollisionComponent2D::OnAttached()
	{
		if (!m_entity->HasComponent<PhysicsComponent2D>())
			InitializeStaticBody();
	}

	/*!
	* \brief Operation to perform when component is attached to this component
	*
	* \param component Component being attached
	*/

	void CollisionComponent2D::OnComponentAttached(BaseComponent& component)
	{
		if (IsComponent<PhysicsComponent2D>(component))
			m_staticBody.reset();
	}

	/*!
	* \brief Operation to perform when component is detached from this component
	*
	* \param component Component being detached
	*/

	void CollisionComponent2D::OnComponentDetached(BaseComponent& component)
	{
		if (IsComponent<PhysicsComponent2D>(component))
			InitializeStaticBody();
	}

	/*!
	* \brief Operation to perform when component is detached from an entity
	*/

	void CollisionComponent2D::OnDetached()
	{
		m_staticBody.reset();
	}

	ComponentIndex CollisionComponent2D::componentIndex;
}
