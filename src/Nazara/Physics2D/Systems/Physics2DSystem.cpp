// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics2D module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Physics2D/Systems/Physics2DSystem.hpp>
#include <Nazara/Core/Components/DisabledComponent.hpp>
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

	Physics2DSystem::Physics2DSystem(entt::registry& registry) :
	m_registry(registry),
	m_physicsConstructObserver(m_registry, entt::collector.group<RigidBody2DComponent, NodeComponent>())
	{
		m_bodyConstructConnection = registry.on_construct<RigidBody2DComponent>().connect<&Physics2DSystem::OnBodyConstruct>(this);
		m_bodyDestructConnection = registry.on_destroy<RigidBody2DComponent>().connect<&Physics2DSystem::OnBodyDestruct>(this);
	}

	Physics2DSystem::~Physics2DSystem()
	{
		m_physicsConstructObserver.disconnect();

		// Ensure every body is destroyed before world is
		auto rigidBodyView = m_registry.view<RigidBody2DComponent>();
		for (auto [entity, rigidBodyComponent] : rigidBodyView.each())
			rigidBodyComponent.Destroy();
	}

	void Physics2DSystem::Update(Time elapsedTime)
	{
		// Move newly-created physics entities to their node position/rotation
		m_physicsConstructObserver.each([&](entt::entity entity)
		{
			RigidBody2DComponent& entityPhysics = m_registry.get<RigidBody2DComponent>(entity);
			NodeComponent& entityNode = m_registry.get<NodeComponent>(entity);

			entityPhysics.TeleportTo(Vector2f(entityNode.GetPosition()), AngleFromQuaternion(entityNode.GetRotation()));
		});

		m_physWorld.Step(elapsedTime);

		// Replicate rigid body position to their node components
		auto view = m_registry.view<NodeComponent, const RigidBody2DComponent>(entt::exclude<DisabledComponent>);
		for (auto [entity, nodeComponent, rigidBodyComponent] : view.each())
		{
			if (rigidBodyComponent.IsSleeping())
				continue;

			nodeComponent.SetTransform(rigidBodyComponent.GetPosition(), rigidBodyComponent.GetRotation());
		}
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

	void Physics2DSystem::OnBodyConstruct(entt::registry& registry, entt::entity entity)
	{
		RigidBody2DComponent& rigidBody = registry.get<RigidBody2DComponent>(entity);
		rigidBody.Construct(m_physWorld);

		UInt32 uniqueIndex = rigidBody.GetBodyIndex();
		if (uniqueIndex >= m_bodyIndicesToEntity.size())
			m_bodyIndicesToEntity.resize(uniqueIndex + 1);

		m_bodyIndicesToEntity[uniqueIndex] = entity;
	}

	void Physics2DSystem::OnBodyDestruct(entt::registry& registry, entt::entity entity)
	{
		// Unregister owning entity
		RigidBody2DComponent& rigidBody = registry.get<RigidBody2DComponent>(entity);
		UInt32 uniqueIndex = rigidBody.GetBodyIndex();
		assert(uniqueIndex <= m_bodyIndicesToEntity.size());

		m_bodyIndicesToEntity[uniqueIndex] = entt::null;
	}
}
