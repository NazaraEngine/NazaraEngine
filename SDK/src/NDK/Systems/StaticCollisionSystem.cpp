// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Systems/StaticCollisionSystem.hpp>
#include <Nazara/Physics/PhysObject.hpp>
#include <NDK/Components/CollisionComponent.hpp>
#include <NDK/Components/NodeComponent.hpp>

namespace Ndk
{
	StaticCollisionSystem::StaticCollisionSystem()
	{
		Requires<CollisionComponent, NodeComponent>();
		Excludes<PhysicsComponent>();
	}

	void StaticCollisionSystem::Update(float elapsedTime)
	{
		float invElapsedTime = 1.f / elapsedTime;
		for (const Ndk::EntityHandle& entity : GetEntities())
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

	SystemIndex StaticCollisionSystem::systemIndex;
}
