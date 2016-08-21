// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Components/CollisionComponent.hpp>
#include <Nazara/Physics/PhysObject.hpp>
#include <NDK/Algorithm.hpp>
#include <NDK/World.hpp>
#include <NDK/Components/PhysicsComponent.hpp>
#include <NDK/Systems/PhysicsSystem.hpp>

namespace Ndk
{
	/*!
	* \ingroup NDK
	* \class Ndk::CollisionComponent
	* \brief NDK class that represents the component for collision (meant for static objects)
	*/

	/*!
	* \brief Sets geometry for the entity
	*
	* \param geom Geometry used for collisions
	*
	* \remark Produces a NazaraAssert if the entity has no physics component and has no static body
	*/

	void CollisionComponent::SetGeom(Nz::PhysGeomRef geom)
	{
		m_geom = std::move(geom);

		if (m_entity->HasComponent<PhysicsComponent>())
		{
			// We update the geometry of the PhysiscsObject linked to the PhysicsComponent
			PhysicsComponent& physComponent = m_entity->GetComponent<PhysicsComponent>();
			physComponent.GetPhysObject().SetGeom(m_geom);
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

	void CollisionComponent::InitializeStaticBody()
	{
		NazaraAssert(m_entity, "Invalid entity");
		World* entityWorld = m_entity->GetWorld();

		NazaraAssert(entityWorld, "Entity must have world");
		NazaraAssert(entityWorld->HasSystem<PhysicsSystem>(), "World must have a physics system");
		Nz::PhysWorld& physWorld = entityWorld->GetSystem<PhysicsSystem>().GetWorld();

		m_staticBody.reset(new Nz::PhysObject(&physWorld, m_geom));
		m_staticBody->EnableAutoSleep(false);
	}

	/*!
	* \brief Operation to perform when component is attached to an entity
	*/

	void CollisionComponent::OnAttached()
	{
		if (!m_entity->HasComponent<PhysicsComponent>())
			InitializeStaticBody();
	}

	/*!
	* \brief Operation to perform when component is attached to this component
	*
	* \param component Component being attached
	*/

	void CollisionComponent::OnComponentAttached(BaseComponent& component)
	{
		if (IsComponent<PhysicsComponent>(component))
			m_staticBody.reset();
	}

	/*!
	* \brief Operation to perform when component is detached from this component
	*
	* \param component Component being detached
	*/

	void CollisionComponent::OnComponentDetached(BaseComponent& component)
	{
		if (IsComponent<PhysicsComponent>(component))
			InitializeStaticBody();
	}

	/*!
	* \brief Operation to perform when component is detached from an entity
	*/

	void CollisionComponent::OnDetached()
	{
		m_staticBody.reset();
	}

	ComponentIndex CollisionComponent::componentIndex;
}
