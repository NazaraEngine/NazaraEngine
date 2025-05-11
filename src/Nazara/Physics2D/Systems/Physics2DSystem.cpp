// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics2D module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Physics2D/Systems/Physics2DSystem.hpp>
#include <Nazara/Core/Components/NodeComponent.hpp>

namespace Nz
{
	namespace
	{
		inline RadianAnglef AngleFromQuaternion(const Quaternionf& quat)
		{
			float siny_cosp = 2.f * (quat.w * quat.z + quat.x * quat.y);
			float cosy_cosp = 1.f - 2.f * (quat.y * quat.y + quat.z * quat.z);

			return std::atan2(siny_cosp, cosy_cosp); //<FIXME: not very efficient
		}
	}

	Physics2DSystem::Physics2DSystem(flecs::world& world) :
	m_world(world)
	{
		m_bodyObserver = m_world.observer<RigidBody2DComponent>()
			.event(flecs::OnAdd)
			.event(flecs::OnRemove)
			.yield_existing()
			.each([&](flecs::iter& it, std::size_t i, RigidBody2DComponent& rigidBody)
			{
				if (it.event() == flecs::OnAdd)
				{
					rigidBody.Construct(m_physWorld);

					UInt32 uniqueIndex = rigidBody.GetBodyIndex();
					if (uniqueIndex >= m_bodyIndicesToEntity.size())
						m_bodyIndicesToEntity.resize(uniqueIndex + 1);

					m_bodyIndicesToEntity[uniqueIndex] = it.entity(i);
				}
				else
				{
					// Unregister owning entity
					UInt32 uniqueIndex = rigidBody.GetBodyIndex();
					assert(uniqueIndex <= m_bodyIndicesToEntity.size());

					m_bodyIndicesToEntity[uniqueIndex] = flecs::entity{};
				}
			});

		// Move newly-created physics entities to their node position/rotation
		m_physicsConstructObserver = m_world.observer<RigidBody2DComponent, const NodeComponent>()
			.event(flecs::OnAdd)
			.yield_existing()
			.each([&](RigidBody2DComponent& entityPhysics, const NodeComponent& entityNode)
			{
				entityPhysics.TeleportTo(Vector2f(entityNode.GetPosition()), AngleFromQuaternion(entityNode.GetRotation()));
			});
	}

	Physics2DSystem::~Physics2DSystem()
	{
		m_bodyObserver.destruct();
		m_physicsConstructObserver.destruct();

		// Ensure every body is destroyed before world is
		m_world.each([](RigidBody2DComponent& rigidBodyComponent)
		{
			rigidBodyComponent.Destroy();
		});
	}

	void Physics2DSystem::Update(Time elapsedTime)
	{
		m_physWorld.Step(elapsedTime);

		// Replicate rigid body position to their node components
		m_world.each([&](NodeComponent& nodeComponent, const RigidBody2DComponent& rigidBodyComponent)
		{
			if (rigidBodyComponent.IsSleeping())
				return;

			nodeComponent.SetTransform(rigidBodyComponent.GetPosition(), rigidBodyComponent.GetRotation());
		});
	}

	PhysWorld2D::ContactCallbacks Physics2DSystem::SetupContactCallbacks(ContactCallbacks callbacks)
	{
		PhysWorld2D::ContactCallbacks trampolineCallbacks;
		trampolineCallbacks.userdata = callbacks.userdata;

		if (callbacks.endCallback)
		{
			trampolineCallbacks.endCallback = [this, cb = std::move(callbacks.endCallback)](PhysWorld2D& world, PhysArbiter2D& arbiter, RigidBody2D& bodyA, RigidBody2D& bodyB, void* userdata)
			{
				return cb(world, arbiter, GetRigidBodyEntity(bodyA.GetBodyIndex()), GetRigidBodyEntity(bodyB.GetBodyIndex()), userdata);
			};
		}

		if (callbacks.preSolveCallback)
		{
			trampolineCallbacks.preSolveCallback = [this, cb = std::move(callbacks.preSolveCallback)](PhysWorld2D& world, PhysArbiter2D& arbiter, RigidBody2D& bodyA, RigidBody2D& bodyB, void* userdata)
			{
				return cb(world, arbiter, GetRigidBodyEntity(bodyA.GetBodyIndex()), GetRigidBodyEntity(bodyB.GetBodyIndex()), userdata);
			};
		}

		if (callbacks.postSolveCallback)
		{
			trampolineCallbacks.postSolveCallback = [this, cb = std::move(callbacks.postSolveCallback)](PhysWorld2D& world, PhysArbiter2D& arbiter, RigidBody2D& bodyA, RigidBody2D& bodyB, void* userdata)
			{
				return cb(world, arbiter, GetRigidBodyEntity(bodyA.GetBodyIndex()), GetRigidBodyEntity(bodyB.GetBodyIndex()), userdata);
			};
		}

		if (callbacks.startCallback)
		{
			trampolineCallbacks.startCallback = [this, cb = std::move(callbacks.startCallback)](PhysWorld2D& world, PhysArbiter2D& arbiter, RigidBody2D& bodyA, RigidBody2D& bodyB, void* userdata)
			{
				return cb(world, arbiter, GetRigidBodyEntity(bodyA.GetBodyIndex()), GetRigidBodyEntity(bodyB.GetBodyIndex()), userdata);
			};
		}

		return trampolineCallbacks;
	}
}
