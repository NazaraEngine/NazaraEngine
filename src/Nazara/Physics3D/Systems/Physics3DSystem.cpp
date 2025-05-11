// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Physics3D/Systems/Physics3DSystem.hpp>
#include <Nazara/Core/Components/NodeComponent.hpp>
#include <Nazara/Physics3D/PhysBody3D.hpp>

namespace Nz
{
	Physics3DSystem::Physics3DSystem(flecs::world& world, Settings&& settings) :
	m_world(world),
	m_physWorld(std::move(settings))
	{
		m_bodyObserver = m_world.observer<RigidBody3DComponent>()
			.event(flecs::OnAdd)
			.event(flecs::OnRemove)
			.yield_existing()
			.each([&](flecs::iter& it, std::size_t i, RigidBody3DComponent& rigidBody)
			{
				if (it.event() == flecs::OnAdd)
				{
					// Register rigid body owning entity
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

		m_characterObserver = m_world.observer<PhysCharacter3DComponent>()
			.event(flecs::OnAdd)
			.event(flecs::OnRemove)
			.yield_existing()
			.each([&](flecs::iter& it, std::size_t i, PhysCharacter3DComponent& character)
			{
				if (it.event() == flecs::OnAdd)
				{
					character.Construct(m_physWorld);

					UInt32 uniqueIndex = character.GetBodyIndex();
					if (uniqueIndex >= m_bodyIndicesToEntity.size())
						m_bodyIndicesToEntity.resize(uniqueIndex + 1);

					m_bodyIndicesToEntity[uniqueIndex] = it.entity(i);
				}
				else
				{
					// Unregister owning entity
					UInt32 uniqueIndex = character.GetBodyIndex();
					assert(uniqueIndex <= m_bodyIndicesToEntity.size());

					m_bodyIndicesToEntity[uniqueIndex] = flecs::entity{};
				}
			});

		// Move newly-created physics entities to their node position/rotation
		m_characterConstructObserver = m_world.observer<PhysCharacter3DComponent, const NodeComponent>()
			.event(flecs::OnAdd)
			.yield_existing()
			.each([&](PhysCharacter3DComponent& entityPhysics, const NodeComponent& entityNode)
			{
				entityPhysics.TeleportTo(entityNode.GetGlobalPosition(), entityNode.GetGlobalRotation());
			});

		m_rigidBodyConstructObserver = m_world.observer<RigidBody3DComponent, const NodeComponent>()
			.event(flecs::OnAdd)
			.yield_existing()
			.each([&](RigidBody3DComponent& entityPhysics, const NodeComponent& entityNode)
			{
				entityPhysics.TeleportTo(entityNode.GetGlobalPosition(), entityNode.GetGlobalRotation());
			});
	}

	Physics3DSystem::~Physics3DSystem()
	{
		m_bodyObserver.destruct();
		m_characterObserver.destruct();
		m_characterConstructObserver.destruct();
		m_rigidBodyConstructObserver.destruct();

		// Ensure every RigidBody3D is destroyed before world is
		m_world.each([&](PhysCharacter3DComponent& characterComponent)
		{
			characterComponent.Destroy();
		});

		m_world.each([&](RigidBody3DComponent& rigidBodyComponent)
		{
			rigidBodyComponent.Destroy(true);
		});
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
					extendedHitInfo.hitEntity = m_bodyIndicesToEntity[bodyIndex];
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
					extendedHitInfo.hitEntity = m_bodyIndicesToEntity[bodyIndex];
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
					extendedHitInfo.hitEntity = m_bodyIndicesToEntity[bodyIndex];
			}

			return callback(extendedHitInfo);
		}, broadphaseFilter, objectLayerFilter, bodyFilter);
	}

	bool Physics3DSystem::RaycastQueryFirst(const Vector3f& from, const Vector3f& to, const FunctionRef<void(const RaycastHit& hitInfo)>& callback, const PhysBroadphaseLayerFilter3D* broadphaseFilter, const PhysObjectLayerFilter3D* objectLayerFilter, const PhysBodyFilter3D* bodyFilter)
	{
		return m_physWorld.RaycastQueryFirst(from, to, [&](const PhysWorld3D::RaycastHit& hitInfo)
		{
			RaycastHit extendedHitInfo;
			static_cast<PhysWorld3D::RaycastHit&>(extendedHitInfo) = hitInfo;

			if (extendedHitInfo.hitBody)
			{
				std::size_t bodyIndex = extendedHitInfo.hitBody->GetBodyIndex();
				if (bodyIndex < m_bodyIndicesToEntity.size())
					extendedHitInfo.hitEntity = m_bodyIndicesToEntity[bodyIndex];
			}

			callback(extendedHitInfo);
		}, broadphaseFilter, objectLayerFilter, bodyFilter);
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
					flecs::entity entity1 = m_physSystem.GetRigidBodyEntity(body1Index);
					flecs::entity entity2 = m_physSystem.GetRigidBodyEntity(body2Index);

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

		ReplicateEntities<PhysCharacter3DComponent>();
		ReplicateEntities<RigidBody3DComponent>();
	}

	template<typename T>
	void Physics3DSystem::ReplicateEntities()
	{
		m_world.each([&](flecs::entity entity, NodeComponent& nodeComponent, T& bodyComponent)
		{
			if (bodyComponent.GetReplicationMode() == PhysicsReplication3D::None || !m_physWorld.IsBodyActive(bodyComponent.GetBodyIndex()))
				return;

			auto [position, rotation] = bodyComponent.GetPositionAndRotation();

			switch (bodyComponent.GetReplicationMode())
			{
				case PhysicsReplication3D::Custom:
				case PhysicsReplication3D::CustomOnce:
				{
					const auto& replicationCallback = bodyComponent.GetReplicationCallback();
					if NAZARA_LIKELY(replicationCallback)
						replicationCallback(entity, bodyComponent);
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
		});
	}

	Physics3DSystem::ContactListener::~ContactListener() = default;
}
