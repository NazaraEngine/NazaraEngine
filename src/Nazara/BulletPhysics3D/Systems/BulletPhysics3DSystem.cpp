// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - BulletPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/BulletPhysics3D/Systems/BulletPhysics3DSystem.hpp>
#include <Nazara/Core/Components/DisabledComponent.hpp>
#include <Nazara/Utility/Components/NodeComponent.hpp>
#include <Nazara/BulletPhysics3D/Debug.hpp>

namespace Nz
{
	BulletPhysics3DSystem::BulletPhysics3DSystem(entt::registry& registry) :
	m_registry(registry),
	m_physicsConstructObserver(m_registry, entt::collector.group<BulletRigidBody3DComponent, NodeComponent>())
	{
		m_constructConnection = registry.on_construct<BulletRigidBody3DComponent>().connect<&BulletPhysics3DSystem::OnConstruct>(this);
		m_destructConnection = registry.on_destroy<BulletRigidBody3DComponent>().connect<&BulletPhysics3DSystem::OnDestruct>(this);

		m_stepCount = 0;
		m_physicsTime = Time::Zero();
		m_updateTime = Time::Zero();
	}

	BulletPhysics3DSystem::~BulletPhysics3DSystem()
	{
		m_physicsConstructObserver.disconnect();

		// Ensure every RigidBody3D is destroyed before world is
		auto rigidBodyView = m_registry.view<BulletRigidBody3DComponent>();
		for (auto [entity, rigidBodyComponent] : rigidBodyView.each())
			rigidBodyComponent.Destroy();
	}

	void BulletPhysics3DSystem::Dump()
	{
		if (m_stepCount == 0)
			m_stepCount = 1;

/*
		std::cout << "Physics time: " << (m_physicsTime / Time::Nanoseconds(m_stepCount)) << std::endl;
		std::cout << "Update time: " << (m_updateTime / Time::Nanoseconds(m_stepCount)) << std::endl;
		std::cout << "Active body count: " << m_activeObjectCount << std::endl;
		std::cout << "--" << std::endl;

*/
		m_stepCount = 0;
		m_physicsTime = Time::Zero();
		m_updateTime = Time::Zero();
	}

	bool BulletPhysics3DSystem::RaycastQuery(const Vector3f& from, const Vector3f& to, const FunctionRef<std::optional<float>(const RaycastHit& hitInfo)>& callback)
	{
		return m_physWorld.RaycastQuery(from, to, [&](const BulletPhysWorld3D::RaycastHit& hitInfo)
		{
			RaycastHit hitWithEntity;
			static_cast<BulletPhysWorld3D::RaycastHit&>(hitWithEntity) = hitInfo;

			if (hitWithEntity.hitBody)
			{
				std::size_t uniqueIndex = hitWithEntity.hitBody->GetUniqueIndex();
				if (uniqueIndex < m_physicsEntities.size())
					hitWithEntity.hitEntity = entt::handle(m_registry, m_physicsEntities[uniqueIndex]);
			}

			return callback(hitWithEntity);
		});
	}

	bool BulletPhysics3DSystem::RaycastQueryFirst(const Vector3f& from, const Vector3f& to, RaycastHit* hitInfo)
	{
		if (!m_physWorld.RaycastQueryFirst(from, to, hitInfo))
			return false;

		if (hitInfo->hitBody)
		{
			std::size_t uniqueIndex = hitInfo->hitBody->GetUniqueIndex();
			if (uniqueIndex < m_physicsEntities.size())
				hitInfo->hitEntity = entt::handle(m_registry, m_physicsEntities[uniqueIndex]);
		}

		return true;
	}

	void BulletPhysics3DSystem::Update(Time elapsedTime)
	{
		// Move newly-created physics entities to their node position/rotation
		m_physicsConstructObserver.each([&](entt::entity entity)
		{
			BulletRigidBody3DComponent& entityPhysics = m_registry.get<BulletRigidBody3DComponent>(entity);
			NodeComponent& entityNode = m_registry.get<NodeComponent>(entity);

			entityPhysics.SetPositionAndRotation(entityNode.GetPosition(CoordSys::Global), entityNode.GetRotation(CoordSys::Global));
		});

		Time t1 = GetElapsedNanoseconds();

		// Update the physics world
		m_physWorld.Step(elapsedTime);
		m_stepCount++;

		Time t2 = GetElapsedNanoseconds();

		// Replicate rigid body position to their node components
		// TODO: Only replicate active entities
		m_activeObjectCount = 0;

		auto view = m_registry.view<NodeComponent, const BulletRigidBody3DComponent>(entt::exclude<DisabledComponent>);
		for (auto entity : view)
		{
			auto& rigidBodyComponent = view.get<const BulletRigidBody3DComponent>(entity);
			if (rigidBodyComponent.IsSleeping())
				continue;

			auto& nodeComponent = view.get<NodeComponent>(entity);
			nodeComponent.SetTransform(rigidBodyComponent.GetPosition(), rigidBodyComponent.GetRotation());
			m_activeObjectCount++;
		}

		Time t3 = GetElapsedNanoseconds();

		m_physicsTime += (t2 - t1);
		m_updateTime += (t3 - t2);
	}

	void BulletPhysics3DSystem::OnConstruct(entt::registry& registry, entt::entity entity)
	{
		// Register rigid body owning entity
		BulletRigidBody3DComponent& rigidBody = registry.get<BulletRigidBody3DComponent>(entity);
		std::size_t uniqueIndex = rigidBody.GetUniqueIndex();
		if (uniqueIndex >= m_physicsEntities.size())
			m_physicsEntities.resize(uniqueIndex + 1);

		m_physicsEntities[uniqueIndex] = entity;
	}

	void BulletPhysics3DSystem::OnDestruct(entt::registry& registry, entt::entity entity)
	{
		// Unregister owning entity
		BulletRigidBody3DComponent& rigidBody = registry.get<BulletRigidBody3DComponent>(entity);
		std::size_t uniqueIndex = rigidBody.GetUniqueIndex();
		assert(uniqueIndex <= m_physicsEntities.size());

		m_physicsEntities[uniqueIndex] = entt::null;
	}
}
