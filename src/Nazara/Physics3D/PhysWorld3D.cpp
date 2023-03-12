// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics3D/PhysWorld3D.hpp>
#include <Nazara/Physics3D/BulletHelper.hpp>
#include <Nazara/Utils/MemoryPool.hpp>
#include <Nazara/Utils/StackVector.hpp>
#include <BulletCollision/BroadphaseCollision/btDbvtBroadphase.h>
#include <BulletCollision/CollisionDispatch/btCollisionDispatcher.h>
#include <BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h>
#include <BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <cassert>
#include <Nazara/Physics3D/Debug.hpp>

namespace Nz
{
	struct PhysWorld3D::BulletWorld
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

	PhysWorld3D::PhysWorld3D() :
	m_maxStepCount(50),
	m_gravity(Vector3f::Zero()),
	m_stepSize(Time::TickDuration(120)),
	m_timestepAccumulator(Time::Zero())
	{
		m_world = std::make_unique<BulletWorld>();
	}

	PhysWorld3D::~PhysWorld3D() = default;

	btDynamicsWorld* PhysWorld3D::GetDynamicsWorld()
	{
		return &m_world->dynamicWorld;
	}

	Vector3f PhysWorld3D::GetGravity() const
	{
		return FromBullet(m_world->dynamicWorld.getGravity());
	}

	std::size_t PhysWorld3D::GetMaxStepCount() const
	{
		return m_maxStepCount;
	}

	Time PhysWorld3D::GetStepSize() const
	{
		return m_stepSize;
	}

	void PhysWorld3D::SetGravity(const Vector3f& gravity)
	{
		m_world->dynamicWorld.setGravity(ToBullet(gravity));
	}

	void PhysWorld3D::SetMaxStepCount(std::size_t maxStepCount)
	{
		m_maxStepCount = maxStepCount;
	}

	void PhysWorld3D::SetStepSize(Time stepSize)
	{
		m_stepSize = stepSize;
	}

	void PhysWorld3D::Step(Time timestep)
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

	btRigidBody* PhysWorld3D::AddRigidBody(std::size_t& rigidBodyIndex, FunctionRef<void(btRigidBody* body)> constructor)
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

	void PhysWorld3D::RemoveRigidBody(btRigidBody* rigidBody, std::size_t rigidBodyIndex)
	{
		// TODO: Improve deletion (since rigid bodies are sorted)
		m_world->dynamicWorld.removeRigidBody(rigidBody); //< this does a linear search
		m_world->rigidBodyPool.Free(rigidBodyIndex);
	}
}
