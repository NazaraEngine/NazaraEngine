// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Components/CollisionComponent3D.hpp>
#include <Nazara/Physics3D/RigidBody3D.hpp>
#include <NDK/World.hpp>
#include <NDK/Components/PhysicsComponent3D.hpp>
#include <NDK/Systems/PhysicsSystem3D.hpp>

namespace Ndk
{
	/*!
	* \ingroup NDK
	* \class Ndk::CollisionComponent3D
	* \brief NDK class that represents the component for collision (meant for static objects)
	*/

	/*!
	* \brief Sets geometry for the entity
	*
	* \param geom Geometry used for collisions
	*
	* \remark Produces a NazaraAssert if the entity has no physics component and has no static body
	*/

	void CollisionComponent3D::SetGeom(Nz::Collider3DRef geom)
	{
		m_geom = std::move(geom);

		if (m_entity->HasComponent<PhysicsComponent3D>())
		{
			// We update the geometry of the PhysiscsObject linked to the PhysicsComponent3D
			PhysicsComponent3D& physComponent = m_entity->GetComponent<PhysicsComponent3D>();
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

	void CollisionComponent3D::InitializeStaticBody()
	{
		NazaraAssert(m_entity, "Invalid entity");
		World* entityWorld = m_entity->GetWorld();

		NazaraAssert(entityWorld, "Entity must have world");
		NazaraAssert(entityWorld->HasSystem<PhysicsSystem3D>(), "World must have a physics system");
		Nz::PhysWorld3D& physWorld = entityWorld->GetSystem<PhysicsSystem3D>().GetWorld();

		m_staticBody = std::make_unique<Nz::RigidBody3D>(&physWorld, m_geom);
		m_staticBody->EnableAutoSleep(false);
	}

	/*!
	* \brief Operation to perform when component is attached to an entity
	*/

	void CollisionComponent3D::OnAttached()
	{
		if (!m_entity->HasComponent<PhysicsComponent3D>())
			InitializeStaticBody();
	}

	/*!
	* \brief Operation to perform when component is attached to this component
	*
	* \param component Component being attached
	*/

	void CollisionComponent3D::OnComponentAttached(BaseComponent& component)
	{
		if (IsComponent<PhysicsComponent3D>(component))
			m_staticBody.reset();
	}

	/*!
	* \brief Operation to perform when component is detached from this component
	*
	* \param component Component being detached
	*/

	void CollisionComponent3D::OnComponentDetached(BaseComponent& component)
	{
		if (IsComponent<PhysicsComponent3D>(component))
			InitializeStaticBody();
	}

	/*!
	* \brief Operation to perform when component is detached from an entity
	*/

	void CollisionComponent3D::OnDetached()
	{
		m_staticBody.reset();
	}

	void CollisionComponent3D::OnEntityDisabled()
	{
		if (m_staticBody)
			m_staticBody->EnableSimulation(false);
	}

	void CollisionComponent3D::OnEntityEnabled()
	{
		if (m_staticBody)
			m_staticBody->EnableSimulation(true);
	}

	ComponentIndex CollisionComponent3D::componentIndex;
}
