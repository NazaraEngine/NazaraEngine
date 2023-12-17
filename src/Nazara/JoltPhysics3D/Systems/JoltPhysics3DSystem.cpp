// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - JoltPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/JoltPhysics3D/Systems/JoltPhysics3DSystem.hpp>
#include <Nazara/Core/Components/DisabledComponent.hpp>
#include <Nazara/JoltPhysics3D/JoltAbstractBody.hpp>
#include <Nazara/Utility/Components/NodeComponent.hpp>
#include <Nazara/JoltPhysics3D/Debug.hpp>

namespace Nz
{
	JoltPhysics3DSystem::JoltPhysics3DSystem(entt::registry& registry) :
	m_registry(registry),
	m_characterConstructObserver(m_registry, entt::collector.group<JoltCharacterComponent,   NodeComponent>(entt::exclude<DisabledComponent, JoltRigidBody3DComponent>)),
	m_rigidBodyConstructObserver(m_registry, entt::collector.group<JoltRigidBody3DComponent, NodeComponent>(entt::exclude<DisabledComponent, JoltCharacterComponent>))
	{
		m_bodyConstructConnection = registry.on_construct<JoltRigidBody3DComponent>().connect<&JoltPhysics3DSystem::OnBodyConstruct>(this);
		m_bodyDestructConnection = registry.on_destroy<JoltRigidBody3DComponent>().connect<&JoltPhysics3DSystem::OnBodyDestruct>(this);
		m_characterConstructConnection = registry.on_construct<JoltCharacterComponent>().connect<&JoltPhysics3DSystem::OnCharacterConstruct>(this);
		m_characterDestructConnection = registry.on_destroy<JoltCharacterComponent>().connect<&JoltPhysics3DSystem::OnCharacterDestruct>(this);
	}

	JoltPhysics3DSystem::~JoltPhysics3DSystem()
	{
		m_characterConstructObserver.disconnect();
		m_rigidBodyConstructObserver.disconnect();

		// Ensure every RigidBody3D is destroyed before world is
		auto characterView = m_registry.view<JoltCharacterComponent>();
		for (auto [entity, characterComponent] : characterView.each())
			characterComponent.Destroy();

		auto rigidBodyView = m_registry.view<JoltRigidBody3DComponent>();
		for (auto [entity, rigidBodyComponent] : rigidBodyView.each())
			rigidBodyComponent.Destroy(true);
	}

	bool JoltPhysics3DSystem::CollisionQuery(const Vector3f& point, const FunctionRef<std::optional<float>(const PointCollisionInfo& collisionInfo)>& callback)
	{
		return m_physWorld.CollisionQuery(point, [&](const JoltPhysWorld3D::PointCollisionInfo& hitInfo)
		{
			PointCollisionInfo extendedHitInfo;
			static_cast<JoltPhysWorld3D::PointCollisionInfo&>(extendedHitInfo) = hitInfo;

			if (extendedHitInfo.hitBody)
			{
				std::size_t bodyIndex = extendedHitInfo.hitBody->GetBodyIndex();
				if (bodyIndex < m_bodyIndicesToEntity.size())
					extendedHitInfo.hitEntity = entt::handle(m_registry, m_bodyIndicesToEntity[bodyIndex]);
			}

			return callback(extendedHitInfo);
		});
	}

	bool JoltPhysics3DSystem::CollisionQuery(const JoltCollider3D& collider, const Matrix4f& shapeTransform, const FunctionRef<std::optional<float>(const ShapeCollisionInfo& hitInfo)>& callback)
	{
		return CollisionQuery(collider, shapeTransform, Vector3f::Unit(), callback);
	}

	bool JoltPhysics3DSystem::CollisionQuery(const JoltCollider3D& collider, const Matrix4f& colliderTransform, const Vector3f& colliderScale, const FunctionRef<std::optional<float>(const ShapeCollisionInfo& hitInfo)>& callback)
	{
		return m_physWorld.CollisionQuery(collider, colliderTransform, colliderScale, [&](const JoltPhysWorld3D::ShapeCollisionInfo& hitInfo)
		{
			ShapeCollisionInfo extendedHitInfo;
			static_cast<JoltPhysWorld3D::ShapeCollisionInfo&>(extendedHitInfo) = hitInfo;

			if (extendedHitInfo.hitBody)
			{
				std::size_t bodyIndex = extendedHitInfo.hitBody->GetBodyIndex();
				if (bodyIndex < m_bodyIndicesToEntity.size())
					extendedHitInfo.hitEntity = entt::handle(m_registry, m_bodyIndicesToEntity[bodyIndex]);
			}

			return callback(extendedHitInfo);
		});
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
				if (bodyIndex < m_bodyIndicesToEntity.size())
					extendedHitInfo.hitEntity = entt::handle(m_registry, m_bodyIndicesToEntity[bodyIndex]);
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
				if (bodyIndex < m_bodyIndicesToEntity.size())
					extendedHitInfo.hitEntity = entt::handle(m_registry, m_bodyIndicesToEntity[bodyIndex]);
			}

			callback(extendedHitInfo);
		});
	}

	void JoltPhysics3DSystem::Update(Time elapsedTime)
	{
		// Move newly-created physics entities to their node position/rotation
		m_characterConstructObserver.each([this](entt::entity entity)
		{
			JoltCharacterComponent& entityCharacter = m_registry.get<JoltCharacterComponent>(entity);
			NodeComponent& entityNode = m_registry.get<NodeComponent>(entity);

			entityCharacter.TeleportTo(entityNode.GetPosition(), entityNode.GetRotation());
		});

		m_rigidBodyConstructObserver.each([this](entt::entity entity)
		{
			JoltRigidBody3DComponent& entityBody = m_registry.get<JoltRigidBody3DComponent>(entity);
			NodeComponent& entityNode = m_registry.get<NodeComponent>(entity);

			entityBody.TeleportTo(entityNode.GetPosition(), entityNode.GetRotation());
		});

		// Update the physics world
		if (!m_physWorld.Step(elapsedTime))
			return; // No physics step took place

		// Replicate characters to their NodeComponent
		{
			auto view = m_registry.view<NodeComponent, const JoltCharacterComponent>(entt::exclude<DisabledComponent>);
			for (auto entity : view)
			{
				auto& characterComponent = view.get<const JoltCharacterComponent>(entity);
				if (!m_physWorld.IsBodyActive(characterComponent.GetBodyIndex()))
					continue;

				auto& nodeComponent = view.get<NodeComponent>(entity);

				auto [position, rotation] = characterComponent.GetPositionAndRotation();
				nodeComponent.SetTransform(position, rotation);
			}
		}

		// Replicate active rigid body position to their node components
		{
			auto view = m_registry.view<NodeComponent, const JoltRigidBody3DComponent>(entt::exclude<DisabledComponent>);
			for (auto entity : m_registry.view<NodeComponent, const JoltRigidBody3DComponent>(entt::exclude<DisabledComponent>))
			{
				auto& rigidBodyComponent = view.get<const JoltRigidBody3DComponent>(entity);
				if (!m_physWorld.IsBodyActive(rigidBodyComponent.GetBodyIndex()))
					continue;

				auto& nodeComponent = view.get<NodeComponent>(entity);

				auto [position, rotation] = rigidBodyComponent.GetPositionAndRotation();
				nodeComponent.SetTransform(position, rotation);
			}
		}
	}

	void JoltPhysics3DSystem::OnBodyConstruct(entt::registry& registry, entt::entity entity)
	{
		// Register rigid body owning entity
		JoltRigidBody3DComponent& rigidBody = registry.get<JoltRigidBody3DComponent>(entity);
		rigidBody.Construct(m_physWorld);

		UInt32 uniqueIndex = rigidBody.GetBodyIndex();
		if (uniqueIndex >= m_bodyIndicesToEntity.size())
			m_bodyIndicesToEntity.resize(uniqueIndex + 1);

		m_bodyIndicesToEntity[uniqueIndex] = entity;
	}

	void JoltPhysics3DSystem::OnBodyDestruct(entt::registry& registry, entt::entity entity)
	{
		// Unregister owning entity
		JoltRigidBody3DComponent& rigidBody = registry.get<JoltRigidBody3DComponent>(entity);
		UInt32 uniqueIndex = rigidBody.GetBodyIndex();
		assert(uniqueIndex <= m_bodyIndicesToEntity.size());

		m_bodyIndicesToEntity[uniqueIndex] = entt::null;
	}

	void JoltPhysics3DSystem::OnCharacterConstruct(entt::registry& registry, entt::entity entity)
	{
		JoltCharacterComponent& character = registry.get<JoltCharacterComponent>(entity);
		character.Construct(m_physWorld);

		UInt32 uniqueIndex = character.GetBodyIndex();
		if (uniqueIndex >= m_bodyIndicesToEntity.size())
			m_bodyIndicesToEntity.resize(uniqueIndex + 1);

		m_bodyIndicesToEntity[uniqueIndex] = entity;
	}

	void JoltPhysics3DSystem::OnCharacterDestruct(entt::registry& registry, entt::entity entity)
	{
		// Unregister owning entity
		JoltCharacterComponent& character = registry.get<JoltCharacterComponent>(entity);
		UInt32 uniqueIndex = character.GetBodyIndex();
		assert(uniqueIndex <= m_bodyIndicesToEntity.size());

		m_bodyIndicesToEntity[uniqueIndex] = entt::null;
	}
}
