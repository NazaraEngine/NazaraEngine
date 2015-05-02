// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Components/PhysicsComponent.hpp>
#include <Nazara/Physics/PhysObject.hpp>
#include <NDK/World.hpp>
#include <NDK/Components/CollisionComponent.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/Systems/PhysicsSystem.hpp>

namespace Ndk
{
	void PhysicsComponent::OnAttached()
	{
		World* entityWorld = m_entity->GetWorld();
		NazaraAssert(entityWorld->HasSystem<PhysicsSystem>(), "World must have a physics system");

		NzPhysWorld& world = entityWorld->GetSystem<PhysicsSystem>().GetWorld();

		NzPhysGeomRef geom;
		if (m_entity->HasComponent<CollisionComponent>())
			geom = m_entity->GetComponent<CollisionComponent>().GetGeom();

		NzMatrix4f matrix;
		if (m_entity->HasComponent<NodeComponent>())
			matrix = m_entity->GetComponent<NodeComponent>().GetTransformMatrix();
		else
			matrix.MakeIdentity();

		m_object.reset(new NzPhysObject(&world, geom, matrix));
		m_object->SetMass(1.f);
	}

	void PhysicsComponent::OnComponentAttached(BaseComponent& component)
	{
		if (component.GetIndex() == GetComponentIndex<CollisionComponent>())
		{
			NazaraAssert(m_object, "Invalid object");
			m_object->SetGeom(static_cast<CollisionComponent&>(component).GetGeom());
		}
	}

	void PhysicsComponent::OnComponentDetached(BaseComponent& component)
	{
		if (component.GetIndex() == GetComponentIndex<CollisionComponent>())
		{
			NazaraAssert(m_object, "Invalid object");
			m_object->SetGeom(NzNullGeom::New());
		}
	}

	void PhysicsComponent::OnDetached()
	{
		m_object.reset();
	}

	ComponentIndex PhysicsComponent::componentIndex;
}
