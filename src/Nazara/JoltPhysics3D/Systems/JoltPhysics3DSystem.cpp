// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - JoltPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/JoltPhysics3D/Systems/JoltPhysics3DSystem.hpp>
#include <Nazara/Core/Components/DisabledComponent.hpp>
#include <Nazara/Utility/Components/NodeComponent.hpp>
#include <iostream>
#include <Nazara/JoltPhysics3D/Debug.hpp>

namespace Nz
{
	JoltPhysics3DSystem::JoltPhysics3DSystem(entt::registry& registry) :
	m_registry(registry),
	m_physicsConstructObserver(m_registry, entt::collector.group<JoltRigidBody3DComponent, NodeComponent>())
	{
		m_constructConnection = registry.on_construct<JoltRigidBody3DComponent>().connect<&JoltPhysics3DSystem::OnConstruct>(this);
		m_destructConnection = registry.on_destroy<JoltRigidBody3DComponent>().connect<&JoltPhysics3DSystem::OnDestruct>(this);

		m_stepCount = 0;
		m_physicsTime = Time::Zero();
		m_updateTime = Time::Zero();
	}

	JoltPhysics3DSystem::~JoltPhysics3DSystem()
	{
		m_physicsConstructObserver.disconnect();

		// Ensure every RigidBody3D is destroyed before world is
		auto rigidBodyView = m_registry.view<JoltRigidBody3DComponent>();
		for (auto [entity, rigidBodyComponent] : rigidBodyView.each())
			rigidBodyComponent.Destroy(true);
	}

	void JoltPhysics3DSystem::Dump()
	{
		if (m_stepCount == 0)
			m_stepCount = 1;

		std::cout << "Physics time: " << (m_physicsTime / Time::Nanoseconds(m_stepCount)) << std::endl;
		std::cout << "Replication time: " << (m_updateTime / Time::Nanoseconds(m_stepCount)) << std::endl;
		std::cout << "Active entity count: " << m_physWorld.GetActiveBodyCount() << std::endl;
		std::cout << "--" << std::endl;

		m_stepCount = 0;
		m_physicsTime = Time::Zero();
		m_updateTime = Time::Zero();
	}

	bool JoltPhysics3DSystem::RaycastQuery(const Vector3f& from, const Vector3f& to, const FunctionRef<std::optional<float>(const RaycastHit& hitInfo)>& callback)
	{
		return m_physWorld.RaycastQuery(from, to, [&](const JoltPhysWorld3D::RaycastHit& hitInfo)
		{
			RaycastHit extendedHitInfo;
			static_cast<JoltPhysWorld3D::RaycastHit&>(extendedHitInfo) = hitInfo;

			if (extendedHitInfo.hitBody)
			{
				std::size_t bodyIndex = extendedHitInfo.hitBody->GetBodyIndex();
				if (bodyIndex < m_physicsEntities.size())
					extendedHitInfo.hitEntity = entt::handle(m_registry, m_physicsEntities[bodyIndex]);
			}

			return callback(extendedHitInfo);
		});
	}

	bool JoltPhysics3DSystem::RaycastQueryFirst(const Vector3f& from, const Vector3f& to, const FunctionRef<void(const RaycastHit& hitInfo)>& callback)
	{
		return m_physWorld.RaycastQueryFirst(from, to, [&](const JoltPhysWorld3D::RaycastHit& hitInfo)
		{
			RaycastHit extendedHitInfo;
			static_cast<JoltPhysWorld3D::RaycastHit&>(extendedHitInfo) = hitInfo;

			if (extendedHitInfo.hitBody)
			{
				std::size_t bodyIndex = extendedHitInfo.hitBody->GetBodyIndex();
				if (bodyIndex < m_physicsEntities.size())
					extendedHitInfo.hitEntity = entt::handle(m_registry, m_physicsEntities[bodyIndex]);
			}

			callback(extendedHitInfo);
		});
	}

	void JoltPhysics3DSystem::Update(Time elapsedTime)
	{
		// Move newly-created physics entities to their node position/rotation
		m_physicsConstructObserver.each([&](entt::entity entity)
		{
			JoltRigidBody3DComponent& entityPhysics = m_registry.get<JoltRigidBody3DComponent>(entity);
			NodeComponent& entityNode = m_registry.get<NodeComponent>(entity);

			entityPhysics.TeleportTo(entityNode.GetPosition(), entityNode.GetRotation());
		});

		Time t1 = GetElapsedNanoseconds();

		// Update the physics world
		m_physWorld.Step(elapsedTime);
		m_stepCount++;

		Time t2 = GetElapsedNanoseconds();

		// Replicate active rigid body position to their node components
		auto view = m_registry.view<NodeComponent, const JoltRigidBody3DComponent>(entt::exclude<DisabledComponent>);
		for (auto entity : view)
		{
			auto& rigidBodyComponent = view.get<const JoltRigidBody3DComponent>(entity);
			if (!m_physWorld.IsBodyActive(rigidBodyComponent.GetBodyIndex()))
				continue;

			auto& nodeComponent = view.get<NodeComponent>(entity);

			auto [position, rotation] = rigidBodyComponent.GetPositionAndRotation();
			nodeComponent.SetTransform(position, rotation);
		}

		Time t3 = GetElapsedNanoseconds();

		m_physicsTime += (t2 - t1);
		m_updateTime += (t3 - t2);
	}

	void JoltPhysics3DSystem::OnConstruct(entt::registry& registry, entt::entity entity)
	{
		// Register rigid body owning entity
		JoltRigidBody3DComponent& rigidBody = registry.get<JoltRigidBody3DComponent>(entity);
		std::size_t uniqueIndex = rigidBody.GetBodyIndex();
		if (uniqueIndex >= m_physicsEntities.size())
			m_physicsEntities.resize(uniqueIndex + 1);

		m_physicsEntities[uniqueIndex] = entity;
	}

	void JoltPhysics3DSystem::OnDestruct(entt::registry& registry, entt::entity entity)
	{
		// Unregister owning entity
		JoltRigidBody3DComponent& rigidBody = registry.get<JoltRigidBody3DComponent>(entity);
		std::size_t uniqueIndex = rigidBody.GetBodyIndex();
		assert(uniqueIndex <= m_physicsEntities.size());

		m_physicsEntities[uniqueIndex] = entt::null;
	}
}
