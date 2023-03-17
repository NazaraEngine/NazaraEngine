// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics3D/Systems/Physics3DSystem.hpp>
#include <Nazara/Utility/Components/NodeComponent.hpp>
#include <Nazara/Physics3D/Debug.hpp>

namespace Nz
{
	Physics3DSystem::Physics3DSystem(entt::registry& registry) :
	m_registry(registry),
	m_physicsConstructObserver(m_registry, entt::collector.group<RigidBody3DComponent, NodeComponent>())
	{
		m_constructConnection = registry.on_construct<RigidBody3DComponent>().connect<&Physics3DSystem::OnConstruct>(this);
		m_destructConnection = registry.on_destroy<RigidBody3DComponent>().connect<&Physics3DSystem::OnDestruct>(this);
	}

	Physics3DSystem::~Physics3DSystem()
	{
		m_physicsConstructObserver.disconnect();

		// Ensure every RigidBody3D is destroyed before world is
		auto rigidBodyView = m_registry.view<RigidBody3DComponent>();
		for (auto [entity, rigidBodyComponent] : rigidBodyView.each())
			rigidBodyComponent.Destroy();
	}

	bool Physics3DSystem::RaycastQuery(const Vector3f& from, const Vector3f& to, const FunctionRef<std::optional<float>(const RaycastHit& hitInfo)>& callback)
	{
		return m_physWorld.RaycastQuery(from, to, [&](const PhysWorld3D::RaycastHit& hitInfo)
		{
			RaycastHit hitWithEntity;
			static_cast<PhysWorld3D::RaycastHit&>(hitWithEntity) = hitInfo;

			if (hitWithEntity.hitBody)
			{
				std::size_t uniqueIndex = hitWithEntity.hitBody->GetUniqueIndex();
				if (uniqueIndex < m_physicsEntities.size())
					hitWithEntity.hitEntity = entt::handle(m_registry, m_physicsEntities[uniqueIndex]);
			}

			return callback(hitWithEntity);
		});
	}

	bool Physics3DSystem::RaycastQueryFirst(const Vector3f& from, const Vector3f& to, RaycastHit* hitInfo)
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

	void Physics3DSystem::Update(Time elapsedTime)
	{
		// Move newly-created physics entities to their node position/rotation
		m_physicsConstructObserver.each([&](entt::entity entity)
		{
			RigidBody3DComponent& entityPhysics = m_registry.get<RigidBody3DComponent>(entity);
			NodeComponent& entityNode = m_registry.get<NodeComponent>(entity);

			entityPhysics.SetPosition(entityNode.GetPosition(CoordSys::Global));
			entityPhysics.SetRotation(entityNode.GetRotation(CoordSys::Global));
		});

		// Update the physics world
		m_physWorld.Step(elapsedTime);

		// Replicate rigid body position to their node components
		// TODO: Only replicate active entities
		auto view = m_registry.view<NodeComponent, const RigidBody3DComponent>();
		for (auto [entity, nodeComponent, rigidBodyComponent] : view.each())
		{
			if (rigidBodyComponent.IsSleeping())
				continue;

			nodeComponent.SetPosition(rigidBodyComponent.GetPosition(), CoordSys::Global);
			nodeComponent.SetRotation(rigidBodyComponent.GetRotation(), CoordSys::Global);
		}
	}

	void Physics3DSystem::OnConstruct(entt::registry& registry, entt::entity entity)
	{
		// Register rigid body owning entity
		RigidBody3DComponent& rigidBody = registry.get<RigidBody3DComponent>(entity);
		std::size_t uniqueIndex = rigidBody.GetUniqueIndex();
		if (uniqueIndex >= m_physicsEntities.size())
			m_physicsEntities.resize(uniqueIndex + 1);

		m_physicsEntities[uniqueIndex] = entity;
	}

	void Physics3DSystem::OnDestruct(entt::registry& registry, entt::entity entity)
	{
		// Unregister owning entity
		RigidBody3DComponent& rigidBody = registry.get<RigidBody3DComponent>(entity);
		std::size_t uniqueIndex = rigidBody.GetUniqueIndex();
		assert(uniqueIndex <= m_physicsEntities.size());

		m_physicsEntities[uniqueIndex] = entt::null;
	}
}
