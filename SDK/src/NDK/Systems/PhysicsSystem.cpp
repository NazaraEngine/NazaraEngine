// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Systems/PhysicsSystem.hpp>
#include <Nazara/Physics/PhysObject.hpp>
#include <NDK/Components/CollisionComponent.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/Components/PhysicsComponent.hpp>

namespace Ndk
{
	PhysicsSystem::PhysicsSystem()
	{
		Requires<NodeComponent>();
		RequiresAny<CollisionComponent, PhysicsComponent>();
	}

	PhysicsSystem::PhysicsSystem(const PhysicsSystem& system) :
	System(system),
	m_world()
	{
	}

	void PhysicsSystem::OnEntityValidation(Entity* entity, bool justAdded)
	{
		// Si l'entité ne vient pas d'être ajoutée au système, il est possible qu'elle fasse partie du mauvais tableau
		if (!justAdded)
		{
			// On prend le tableau inverse de celui dont l'entité devrait faire partie
			auto& entities = (entity->HasComponent<PhysicsComponent>()) ? m_staticObjects : m_dynamicObjects;
			entities.Remove(entity);
		}

		auto& entities = (entity->HasComponent<PhysicsComponent>()) ? m_dynamicObjects : m_staticObjects;
		entities.Insert(entity);
	}

	void PhysicsSystem::OnUpdate(float elapsedTime)
	{
		m_world.Step(elapsedTime);

		for (const Ndk::EntityHandle& entity : m_dynamicObjects)
		{
			NodeComponent& node = entity->GetComponent<NodeComponent>();
			PhysicsComponent& phys = entity->GetComponent<PhysicsComponent>();

			NzPhysObject& physObj = phys.GetPhysObject();
			node.SetRotation(physObj.GetRotation(), nzCoordSys_Global);
			node.SetPosition(physObj.GetPosition(), nzCoordSys_Global);
		}

		float invElapsedTime = 1.f / elapsedTime;
		for (const Ndk::EntityHandle& entity : m_staticObjects)
		{
			CollisionComponent& collision = entity->GetComponent<CollisionComponent>();
			NodeComponent& node = entity->GetComponent<NodeComponent>();

			NzPhysObject* physObj = collision.GetStaticBody();

			NzQuaternionf oldRotation = physObj->GetRotation();
			NzVector3f oldPosition = physObj->GetPosition();
			NzQuaternionf newRotation = node.GetRotation(nzCoordSys_Global);
			NzVector3f newPosition = node.GetPosition(nzCoordSys_Global);

			// Pour déplacer des objets statiques et assurer les collisions, il faut leur définir une vitesse
			// (note importante: le moteur physique n'applique pas la vitesse sur les objets statiques)
			if (newPosition != oldPosition)
			{
				physObj->SetPosition(newPosition);
				physObj->SetVelocity((newPosition - oldPosition) * invElapsedTime);
			}
			else
				physObj->SetVelocity(NzVector3f::Zero());

			if (newRotation != oldRotation)
			{
				NzQuaternionf transition = newRotation * oldRotation.GetConjugate();
				NzEulerAnglesf angles = transition.ToEulerAngles();
				NzVector3f angularVelocity(NzToRadians(angles.pitch * invElapsedTime),
				                           NzToRadians(angles.yaw * invElapsedTime),
				                           NzToRadians(angles.roll * invElapsedTime));

				physObj->SetRotation(oldRotation);
				physObj->SetAngularVelocity(angularVelocity);
			}
			else
				physObj->SetAngularVelocity(NzVector3f::Zero());
		}
	}

	SystemIndex PhysicsSystem::systemIndex;
}
