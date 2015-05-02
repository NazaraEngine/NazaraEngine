// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Components/CollisionComponent.hpp>
#include <Nazara/Physics/PhysObject.hpp>
#include <NDK/World.hpp>
#include <NDK/Components/PhysicsComponent.hpp>
#include <NDK/Systems/PhysicsSystem.hpp>

namespace Ndk
{
	void CollisionComponent::SetGeom(NzPhysGeomRef geom)
	{
		m_geom = std::move(geom);

		if (m_entity->HasComponent<PhysicsComponent>())
		{
			// On met à jour la géométrie du PhysObject associé au PhysicsComponent
			PhysicsComponent& physComponent = m_entity->GetComponent<PhysicsComponent>();
			physComponent.GetPhysObject().SetGeom(m_geom);
		}
		else
		{
			NazaraAssert(m_staticBody, "An entity without physics component should have a static body");
			m_staticBody->SetGeom(m_geom);
		}
	}

	void CollisionComponent::InitializeStaticBody()
	{
		NazaraAssert(m_entity, "Invalid entity");
		World* entityWorld = m_entity->GetWorld();

		NazaraAssert(entityWorld, "Entity must have world");
		NazaraAssert(entityWorld->HasSystem<PhysicsSystem>(), "World must have a physics system");
		NzPhysWorld& physWorld = entityWorld->GetSystem<PhysicsSystem>().GetWorld();

		m_staticBody.reset(new NzPhysObject(&physWorld, m_geom));
		m_staticBody->EnableAutoSleep(false);
	}

	void CollisionComponent::OnAttached()
	{
		if (!m_entity->HasComponent<PhysicsComponent>())
			InitializeStaticBody();
	}

	void CollisionComponent::OnComponentAttached(BaseComponent& component)
	{
		if (component.GetIndex() == GetComponentIndex<PhysicsComponent>())
			m_staticBody.reset();
	}

	void CollisionComponent::OnComponentDetached(BaseComponent& component)
	{
		if (component.GetIndex() == GetComponentIndex<PhysicsComponent>())
			InitializeStaticBody();
	}

	void CollisionComponent::OnDetached()
	{
		m_staticBody.reset();
	}

	ComponentIndex CollisionComponent::componentIndex;
}
