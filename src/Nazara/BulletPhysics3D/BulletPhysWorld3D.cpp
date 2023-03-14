// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - BulletPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/BulletPhysics3D/BulletPhysWorld3D.hpp>
#include <Nazara/BulletPhysics3D/BulletHelper.hpp>
#include <Nazara/Utils/MemoryPool.hpp>
#include <Nazara/Utils/StackVector.hpp>
#include <BulletCollision/BroadphaseCollision/btDbvtBroadphase.h>
#include <BulletCollision/CollisionDispatch/btCollisionDispatcher.h>
#include <BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h>
#include <BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <cassert>
#include <Nazara/BulletPhysics3D/Debug.hpp>

namespace Nz
{
	struct BulletPhysWorld3D::BulletWorld
	{
		btDefaultCollisionConfiguration collisionConfiguration;
		btCollisionDispatcher dispatcher;
		btDbvtBroadphase broadphase;
		btSequentialImpulseConstraintSolver constraintSolver;
		btDiscreteDynamicsWorld dynamicWorld;
		MemoryPool<btRigidBody> rigidBodyPool;

		BulletWorld() :
		dispatcher(&collisionConfiguration),
		dynamicWorld(&dispatcher, &broadphase, &constraintSolver, &collisionConfiguration),
		rigidBodyPool(256)
		{
		}

		BulletWorld(const BulletWorld&) = delete;
		BulletWorld(BulletWorld&&) = delete;

		BulletWorld& operator=(const BulletWorld&) = delete;
		BulletWorld& operator=(BulletWorld&&) = delete;
	};

	BulletPhysWorld3D::BulletPhysWorld3D() :
	m_maxStepCount(50),
	m_gravity(Vector3f::Zero()),
	m_stepSize(Time::TickDuration(120)),
	m_timestepAccumulator(Time::Zero())
	{
		m_world = std::make_unique<BulletWorld>();
	}

	BulletPhysWorld3D::~BulletPhysWorld3D() = default;

	btDynamicsWorld* BulletPhysWorld3D::GetDynamicsWorld()
	{
		return &m_world->dynamicWorld;
	}

	Vector3f BulletPhysWorld3D::GetGravity() const
	{
		return FromBullet(m_world->dynamicWorld.getGravity());
	}

	std::size_t BulletPhysWorld3D::GetMaxStepCount() const
	{
		return m_maxStepCount;
	}

	Time BulletPhysWorld3D::GetStepSize() const
	{
		return m_stepSize;
	}

	bool BulletPhysWorld3D::RaycastQueryFirst(const Vector3f& from, const Vector3f& to, RaycastHit* hitInfo)
	{
		btCollisionWorld::ClosestRayResultCallback callback(ToBullet(from), ToBullet(to));
		m_world->dynamicWorld.rayTest(ToBullet(from), ToBullet(to), callback);

		if (!callback.hasHit())
			return false;

		if (hitInfo)
		{
			hitInfo->fraction = callback.m_closestHitFraction;
			hitInfo->hitNormal = FromBullet(callback.m_hitNormalWorld);
			hitInfo->hitPosition = FromBullet(callback.m_hitPointWorld);

			if (const btRigidBody* body = btRigidBody::upcast(callback.m_collisionObject))
				hitInfo->hitBody = static_cast<BulletRigidBody3D*>(body->getUserPointer());
		}

		return true;
	}

	void BulletPhysWorld3D::SetGravity(const Vector3f& gravity)
	{
		m_world->dynamicWorld.setGravity(ToBullet(gravity));
	}

	void BulletPhysWorld3D::SetMaxStepCount(std::size_t maxStepCount)
	{
		m_maxStepCount = maxStepCount;
	}

	void BulletPhysWorld3D::SetStepSize(Time stepSize)
	{
		m_stepSize = stepSize;
	}

	void BulletPhysWorld3D::Step(Time timestep)
	{
		m_timestepAccumulator += timestep;

		btScalar stepSize = m_stepSize.AsSeconds<btScalar>();

		std::size_t stepCount = 0;
		while (m_timestepAccumulator >= m_stepSize && stepCount < m_maxStepCount)
		{
			m_world->dynamicWorld.stepSimulation(stepSize, 0, stepSize);
			m_timestepAccumulator -= m_stepSize;
			stepCount++;
		}
	}

	btRigidBody* BulletPhysWorld3D::AddRigidBody(std::size_t& rigidBodyIndex, FunctionRef<void(btRigidBody* body)> constructor)
	{
		btRigidBody* rigidBody = m_world->rigidBodyPool.Allocate(m_world->rigidBodyPool.DeferConstruct, rigidBodyIndex);
		constructor(rigidBody);

		m_world->dynamicWorld.addRigidBody(rigidBody);

		// Small hack to order rigid bodies to make it cache friendly
		auto& rigidBodies = m_world->dynamicWorld.getNonStaticRigidBodies();
		if (rigidBodies.size() >= 2 && rigidBodies[rigidBodies.size() - 1] == rigidBody)
		{
			// Sort rigid bodies
			btRigidBody** startPtr = &rigidBodies[0];
			btRigidBody** endPtr = startPtr + rigidBodies.size();
			btRigidBody** lastPtr = endPtr - 1;

			auto it = std::lower_bound(startPtr, endPtr, rigidBody);
			if (it != lastPtr)
			{
				std::move_backward(it, lastPtr, endPtr);
				*it = rigidBody;
			}
		}

		return rigidBody;
	}

	void BulletPhysWorld3D::RemoveRigidBody(btRigidBody* rigidBody, std::size_t rigidBodyIndex)
	{
		// TODO: Improve deletion (since rigid bodies are sorted)
		m_world->dynamicWorld.removeRigidBody(rigidBody); //< this does a linear search
		m_world->rigidBodyPool.Free(rigidBodyIndex);
	}
}
