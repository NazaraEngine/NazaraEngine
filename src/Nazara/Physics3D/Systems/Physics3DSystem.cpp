// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Physics3D/Systems/Physics3DSystem.hpp>
#include <Nazara/Core/Components/DisabledComponent.hpp>
#include <Nazara/Core/Components/NodeComponent.hpp>
#include <Nazara/Physics3D/PhysBody3D.hpp>

namespace Nz
{
	Physics3DSystem::Physics3DSystem(entt::registry& registry, Settings&& settings) :
	m_registry(registry),
	m_characterObserver(m_registry),
	m_rigidBodyObserver(m_registry),
	m_physWorld(std::move(settings))
	{
		m_physWorld.RegisterBodyListener(this);

		m_bodyConstructConnection = registry.on_construct<RigidBody3DComponent>().connect<&Physics3DSystem::OnBodyConstruct>(this);
		m_bodyDestructConnection = registry.on_destroy<RigidBody3DComponent>().connect<&Physics3DSystem::OnBodyDestruct>(this);
		m_characterConstructConnection = registry.on_construct<PhysCharacter3DComponent>().connect<&Physics3DSystem::OnCharacterConstruct>(this);
		m_characterDestructConnection = registry.on_destroy<PhysCharacter3DComponent>().connect<&Physics3DSystem::OnCharacterDestruct>(this);

		// Move newly-created physics entities to their node position/rotation
		m_characterObserver.OnEntityAdded.Connect([this](entt::entity entity)
		{
			PhysCharacter3DComponent& entityCharacter = m_registry.get<PhysCharacter3DComponent>(entity);
			entityCharacter.EnableSimulation();

			NodeComponent& entityNode = m_registry.get<NodeComponent>(entity);
			entityCharacter.TeleportTo(entityNode.GetGlobalPosition(), entityNode.GetGlobalRotation());
		});
		m_characterObserver.OnEntityRemove.Connect([this](entt::entity entity)
		{
			if (m_activeCharacterEntities.contains(entity))
			{
				PhysCharacter3DComponent& entityCharacter = m_registry.get<PhysCharacter3DComponent>(entity);
				entityCharacter.DisableSimulation();

				OnEntityDeactivated(entt::handle(m_registry, entity), entityCharacter.GetBodyIndex());
				m_activeCharacterEntities.erase(entity);
			}
		});
		m_characterObserver.SignalExisting();

		m_rigidBodyObserver.OnEntityAdded.Connect([this](entt::entity entity)
		{
			RigidBody3DComponent& entityBody = m_registry.get<RigidBody3DComponent>(entity);
			entityBody.EnableSimulation();

			NodeComponent& entityNode = m_registry.get<NodeComponent>(entity);
			entityBody.TeleportTo(entityNode.GetGlobalPosition(), entityNode.GetGlobalRotation());

			if (entityBody.IsStatic())
			{
				// Teleport static bodies when moved
				auto& rigidBodyData = m_rigidBodyObserver.Get(entity);
				rigidBodyData.onNodeInvalidation.Connect(entityNode.OnNodeInvalidation, [this, entity](const Nz::Node* node)
				{
					RigidBody3DComponent& entityBody = m_registry.get<RigidBody3DComponent>(entity);
					entityBody.TeleportTo(node->GetGlobalPosition(), node->GetGlobalRotation());
				});
			}
		});
		m_rigidBodyObserver.OnEntityRemove.Connect([this](entt::entity entity)
		{
			if (m_activeBodyEntities.contains(entity))
			{
				RigidBody3DComponent& entityBody = m_registry.get<RigidBody3DComponent>(entity);
				entityBody.DisableSimulation();

				OnEntityDeactivated(entt::handle(m_registry, entity), entityBody.GetBodyIndex());
				m_activeBodyEntities.erase(entity);
			}
		});
		m_rigidBodyObserver.SignalExisting();
	}

	Physics3DSystem::~Physics3DSystem()
	{
		// Ensure every RigidBody3D is destroyed before world is
		auto characterView = m_registry.view<PhysCharacter3DComponent>();
		for (auto [entity, characterComponent] : characterView.each())
			characterComponent.Destroy();

		auto rigidBodyView = m_registry.view<RigidBody3DComponent>();
		for (auto [entity, rigidBodyComponent] : rigidBodyView.each())
			rigidBodyComponent.Destroy(true);
	}

	bool Physics3DSystem::CollisionQuery(const Vector3f& point, const FunctionRef<std::optional<float>(const PointCollisionInfo& collisionInfo)>& callback, const PhysBroadphaseLayerFilter3D* broadphaseFilter, const PhysObjectLayerFilter3D* objectLayerFilter, const PhysBodyFilter3D* bodyFilter)
	{
		return m_physWorld.CollisionQuery(point, [&](const PhysWorld3D::PointCollisionInfo& hitInfo)
		{
			PointCollisionInfo extendedHitInfo;
			static_cast<PhysWorld3D::PointCollisionInfo&>(extendedHitInfo) = hitInfo;

			if (extendedHitInfo.hitBody)
			{
				std::size_t bodyIndex = extendedHitInfo.hitBody->GetBodyIndex();
				if (bodyIndex < m_bodyIndicesToEntity.size())
					extendedHitInfo.hitEntity = entt::handle(m_registry, m_bodyIndicesToEntity[bodyIndex]);
			}

			return callback(extendedHitInfo);
		}, broadphaseFilter, objectLayerFilter, bodyFilter);
	}

	bool Physics3DSystem::CollisionQuery(const Collider3D& collider, const Matrix4f& shapeTransform, const FunctionRef<std::optional<float>(const ShapeCollisionInfo& hitInfo)>& callback, const PhysBroadphaseLayerFilter3D* broadphaseFilter, const PhysObjectLayerFilter3D* objectLayerFilter, const PhysBodyFilter3D* bodyFilter)
	{
		return CollisionQuery(collider, shapeTransform, Vector3f::Unit(), callback, broadphaseFilter, objectLayerFilter, bodyFilter);
	}

	bool Physics3DSystem::CollisionQuery(const Collider3D& collider, const Matrix4f& colliderTransform, const Vector3f& colliderScale, const FunctionRef<std::optional<float>(const ShapeCollisionInfo& hitInfo)>& callback, const PhysBroadphaseLayerFilter3D* broadphaseFilter, const PhysObjectLayerFilter3D* objectLayerFilter, const PhysBodyFilter3D* bodyFilter)
	{
		return m_physWorld.CollisionQuery(collider, colliderTransform, colliderScale, [&](const PhysWorld3D::ShapeCollisionInfo& hitInfo)
		{
			ShapeCollisionInfo extendedHitInfo;
			static_cast<PhysWorld3D::ShapeCollisionInfo&>(extendedHitInfo) = hitInfo;

			if (extendedHitInfo.hitBody)
			{
				std::size_t bodyIndex = extendedHitInfo.hitBody->GetBodyIndex();
				if (bodyIndex < m_bodyIndicesToEntity.size())
					extendedHitInfo.hitEntity = entt::handle(m_registry, m_bodyIndicesToEntity[bodyIndex]);
			}

			return callback(extendedHitInfo);
		}, broadphaseFilter, objectLayerFilter, bodyFilter);
	}

	bool Physics3DSystem::RaycastQuery(const Vector3f& from, const Vector3f& to, const FunctionRef<std::optional<float>(const RaycastHit& hitInfo)>& callback, const PhysBroadphaseLayerFilter3D* broadphaseFilter, const PhysObjectLayerFilter3D* objectLayerFilter, const PhysBodyFilter3D* bodyFilter)
	{
		return m_physWorld.RaycastQuery(from, to, [&](const PhysWorld3D::RaycastHit& hitInfo)
		{
			RaycastHit extendedHitInfo;
			static_cast<PhysWorld3D::RaycastHit&>(extendedHitInfo) = hitInfo;

			if (extendedHitInfo.hitBody)
			{
				std::size_t bodyIndex = extendedHitInfo.hitBody->GetBodyIndex();
				if (bodyIndex < m_bodyIndicesToEntity.size())
					extendedHitInfo.hitEntity = entt::handle(m_registry, m_bodyIndicesToEntity[bodyIndex]);
			}

			return callback(extendedHitInfo);
		}, broadphaseFilter, objectLayerFilter, bodyFilter);
	}

	bool Physics3DSystem::RaycastQueryClosest(const Vector3f& from, const Vector3f& to, RaycastHit* hitInfo, const PhysBroadphaseLayerFilter3D* broadphaseFilter, const PhysObjectLayerFilter3D* objectLayerFilter, const PhysBodyFilter3D* bodyFilter)
	{
		if (!m_physWorld.RaycastQueryClosest(from, to, hitInfo, broadphaseFilter, objectLayerFilter, bodyFilter))
			return false;

		if (hitInfo->hitBody)
		{
			std::size_t bodyIndex = hitInfo->hitBody->GetBodyIndex();
			if (bodyIndex < m_bodyIndicesToEntity.size())
				hitInfo->hitEntity = entt::handle(m_registry, m_bodyIndicesToEntity[bodyIndex]);
		}

		return true;
	}

	void Physics3DSystem::SetContactListener(std::unique_ptr<ContactListener> contactListener)
	{
		class ContactListenerBridge : public PhysWorld3D::ContactListener
		{
			public:
				ContactListenerBridge(Physics3DSystem& physSystem, std::unique_ptr<Physics3DSystem::ContactListener> physContactListener) :
				m_contactListener(std::move(physContactListener)),
				m_physSystem(physSystem)
				{
				}

				PhysContactValidateResult3D ValidateContact(const PhysBody3D* body1, const PhysBody3D* body2, const Vector3f& baseOffset, const PhysWorld3D::ShapeCollisionInfo& collisionResult) override
				{
					ShapeCollisionInfo collisionInfo;
					collisionInfo.collisionPosition1 = collisionResult.collisionPosition1;
					collisionInfo.collisionPosition2 = collisionResult.collisionPosition2;
					collisionInfo.penetrationAxis = collisionResult.penetrationAxis;
					collisionInfo.penetrationDepth = collisionResult.penetrationDepth;

					return m_contactListener->ValidateContact(m_physSystem.GetRigidBodyEntity(body1->GetBodyIndex()), body1, m_physSystem.GetRigidBodyEntity(body2->GetBodyIndex()), body2, baseOffset, collisionInfo);
				}

				void OnContactAdded(const PhysBody3D* body1, const PhysBody3D* body2, const PhysContact3D& physContact, PhysContactResponse3D& physContactResponse) override
				{
					return m_contactListener->OnContactAdded(m_physSystem.GetRigidBodyEntity(body1->GetBodyIndex()), body1, m_physSystem.GetRigidBodyEntity(body2->GetBodyIndex()), body2, physContact, physContactResponse);
				}

				void OnContactPersisted(const PhysBody3D* body1, const PhysBody3D* body2, const PhysContact3D& physContact, PhysContactResponse3D& physContactResponse) override
				{
					return m_contactListener->OnContactPersisted(m_physSystem.GetRigidBodyEntity(body1->GetBodyIndex()), body1, m_physSystem.GetRigidBodyEntity(body2->GetBodyIndex()), body2, physContact, physContactResponse);
				}

				void OnContactRemoved(UInt32 body1Index, const PhysBody3D* body1, UInt32 subShapeID1, UInt32 body2Index, const PhysBody3D* body2, UInt32 subShapeID2) override
				{
					entt::handle entity1 = m_physSystem.GetRigidBodyEntity(body1Index);
					entt::handle entity2 = m_physSystem.GetRigidBodyEntity(body2Index);

					return m_contactListener->OnContactRemoved(entity1, body1Index, body1, subShapeID1, entity2, body2Index, body2, subShapeID2);
				}

			private:
				std::unique_ptr<Physics3DSystem::ContactListener> m_contactListener;
				Physics3DSystem& m_physSystem;
		};

		m_physWorld.SetContactListener(std::make_unique<ContactListenerBridge>(*this, std::move(contactListener)));
	}

	void Physics3DSystem::Update(Time elapsedTime)
	{
		// Update the physics world
		if (!m_physWorld.Step(elapsedTime))
			return; // No physics step took place

		ReplicateEntities<PhysCharacter3DComponent>(m_activeCharacterEntities);
		ReplicateEntities<RigidBody3DComponent>(m_activeBodyEntities);
	}

	void Physics3DSystem::OnBodyActivated(UInt32 bodyIndex)
	{
		assert(bodyIndex <= m_bodyIndicesToEntity.size());
		entt::entity entity = m_bodyIndicesToEntity[bodyIndex];

		if (m_rigidBodyObserver.Contains(entity))
		{
			if (!m_activeBodyEntities.contains(entity))
				m_activeBodyEntities.emplace(entity);
		}
		else if (m_characterObserver.Contains(entity))
		{
			if (!m_activeCharacterEntities.contains(entity))
				m_activeCharacterEntities.emplace(entity);
		}
		else
			return; //< ??

		OnEntityActivated(entt::handle(m_registry, entity), bodyIndex);
	}

	void Physics3DSystem::OnBodyDeactivated(UInt32 bodyIndex)
	{
		assert(bodyIndex <= m_bodyIndicesToEntity.size());
		entt::entity entity = m_bodyIndicesToEntity[bodyIndex];

		if (m_rigidBodyObserver.Contains(entity))
			m_activeBodyEntities.remove(entity);
		else if (m_characterObserver.Contains(entity))
			m_activeCharacterEntities.remove(entity);
		else
			return; //< ??

		OnEntityDeactivated(entt::handle(m_registry, entity), bodyIndex);
	}

	void Physics3DSystem::OnBodyConstruct(entt::registry& registry, entt::entity entity)
	{
		// Register rigid body owning entity
		RigidBody3DComponent& rigidBody = registry.get<RigidBody3DComponent>(entity);
		rigidBody.Construct(m_physWorld);

		UInt32 uniqueIndex = rigidBody.GetBodyIndex();
		if (uniqueIndex >= m_bodyIndicesToEntity.size())
			m_bodyIndicesToEntity.resize(uniqueIndex + 1);

		m_bodyIndicesToEntity[uniqueIndex] = entity;
	}

	void Physics3DSystem::OnBodyDestruct(entt::registry& registry, entt::entity entity)
	{
		// Unregister owning entity
		RigidBody3DComponent& rigidBody = registry.get<RigidBody3DComponent>(entity);
		UInt32 uniqueIndex = rigidBody.GetBodyIndex();
		assert(uniqueIndex <= m_bodyIndicesToEntity.size());

		m_bodyIndicesToEntity[uniqueIndex] = entt::null;
	}

	void Physics3DSystem::OnCharacterConstruct(entt::registry& registry, entt::entity entity)
	{
		PhysCharacter3DComponent& character = registry.get<PhysCharacter3DComponent>(entity);
		character.Construct(m_physWorld);

		UInt32 uniqueIndex = character.GetBodyIndex();
		if (uniqueIndex >= m_bodyIndicesToEntity.size())
			m_bodyIndicesToEntity.resize(uniqueIndex + 1);

		m_bodyIndicesToEntity[uniqueIndex] = entity;
	}

	void Physics3DSystem::OnCharacterDestruct(entt::registry& registry, entt::entity entity)
	{
		// Unregister owning entity
		PhysCharacter3DComponent& character = registry.get<PhysCharacter3DComponent>(entity);
		UInt32 uniqueIndex = character.GetBodyIndex();
		assert(uniqueIndex <= m_bodyIndicesToEntity.size());

		m_bodyIndicesToEntity[uniqueIndex] = entt::null;
	}

	template<typename T>
	void Physics3DSystem::ReplicateEntities(entt::storage<void>& activeEntities)
	{
		for (entt::entity entity : activeEntities)
		{
			auto& bodyComponent = m_registry.template get<T>(entity);
			if (bodyComponent.GetReplicationMode() == PhysicsReplication3D::None)
				continue;

			auto& nodeComponent = m_registry.template get<NodeComponent>(entity);
			auto [position, rotation] = bodyComponent.GetPositionAndRotation();

			switch (bodyComponent.GetReplicationMode())
			{
				case PhysicsReplication3D::Custom:
				case PhysicsReplication3D::CustomOnce:
				{
					const auto& replicationCallback = bodyComponent.GetReplicationCallback();
					if NAZARA_LIKELY(replicationCallback)
						replicationCallback(entt::handle(m_registry, entity), bodyComponent);
					else
						NazaraError("physics component has custom replication mode but no callback");

					if (bodyComponent.GetReplicationMode() == PhysicsReplication3D::CustomOnce)
						bodyComponent.SetReplicationMode(PhysicsReplication3D::None);

					break;
				}

				case PhysicsReplication3D::Global:
				case PhysicsReplication3D::GlobalOnce:
				{
					nodeComponent.SetGlobalTransform(position, rotation);
					if (bodyComponent.GetReplicationMode() == PhysicsReplication3D::GlobalOnce)
						bodyComponent.SetReplicationMode(PhysicsReplication3D::None);

					break;
				}

				case PhysicsReplication3D::Local:
				case PhysicsReplication3D::LocalOnce:
				{
					nodeComponent.SetTransform(position, rotation);
					if (bodyComponent.GetReplicationMode() == PhysicsReplication3D::LocalOnce)
						bodyComponent.SetReplicationMode(PhysicsReplication3D::None);

					break;
				}

				case PhysicsReplication3D::None:
					NAZARA_UNREACHABLE();
			}
		}
	}

	Physics3DSystem::ContactListener::~ContactListener() = default;
}
