// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_PHYSICS3D_SYSTEMS_PHYSICS3DSYSTEM_HPP
#define NAZARA_PHYSICS3D_SYSTEMS_PHYSICS3DSYSTEM_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Core/Time.hpp>
#include <Nazara/Physics3D/PhysWorld3D.hpp>
#include <Nazara/Physics3D/Components/PhysCharacter3DComponent.hpp>
#include <Nazara/Physics3D/Components/RigidBody3DComponent.hpp>
#include <NazaraUtils/TypeList.hpp>
#include <entt/entt.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_PHYSICS3D_API Physics3DSystem
	{
		public:
			static constexpr Int64 ExecutionOrder = 0;
			using Components = TypeList<PhysCharacter3DComponent, RigidBody3DComponent, class NodeComponent>;

			struct ContactListener;
			struct PointCollisionInfo;
			struct RaycastHit;
			struct ShapeCollisionInfo;
			using Settings = PhysWorld3D::Settings;

			Physics3DSystem(entt::registry& registry, Settings&& settings = PhysWorld3D::BuildDefaultSettings());
			Physics3DSystem(const Physics3DSystem&) = delete;
			Physics3DSystem(Physics3DSystem&&) = delete;
			~Physics3DSystem();

			bool CollisionQuery(const Vector3f& point, const FunctionRef<std::optional<float>(const PointCollisionInfo& collisionInfo)>& callback, const PhysBroadphaseLayerFilter3D* broadphaseFilter = nullptr, const PhysObjectLayerFilter3D* objectLayerFilter = nullptr, const PhysBodyFilter3D* bodyFilter = nullptr);
			bool CollisionQuery(const Collider3D& collider, const Matrix4f& colliderTransform, const FunctionRef<std::optional<float>(const ShapeCollisionInfo& hitInfo)>& callback, const PhysBroadphaseLayerFilter3D* broadphaseFilter = nullptr, const PhysObjectLayerFilter3D* objectLayerFilter = nullptr, const PhysBodyFilter3D* bodyFilter = nullptr);
			bool CollisionQuery(const Collider3D& collider, const Matrix4f& colliderTransform, const Vector3f& colliderScale, const FunctionRef<std::optional<float>(const ShapeCollisionInfo& hitInfo)>& callback, const PhysBroadphaseLayerFilter3D* broadphaseFilter = nullptr, const PhysObjectLayerFilter3D* objectLayerFilter = nullptr, const PhysBodyFilter3D* bodyFilter = nullptr);

			inline PhysWorld3D& GetPhysWorld();
			inline const PhysWorld3D& GetPhysWorld() const;
			inline entt::handle GetRigidBodyEntity(UInt32 bodyIndex) const;

			bool RaycastQuery(const Vector3f& from, const Vector3f& to, const FunctionRef<std::optional<float>(const RaycastHit& hitInfo)>& callback, const PhysBroadphaseLayerFilter3D* broadphaseFilter = nullptr, const PhysObjectLayerFilter3D* objectLayerFilter = nullptr, const PhysBodyFilter3D* bodyFilter = nullptr);
			bool RaycastQueryFirst(const Vector3f& from, const Vector3f& to, const FunctionRef<void(const RaycastHit& hitInfo)>& callback, const PhysBroadphaseLayerFilter3D* broadphaseFilter = nullptr, const PhysObjectLayerFilter3D* objectLayerFilter = nullptr, const PhysBodyFilter3D* bodyFilter = nullptr);

			void SetContactListener(std::unique_ptr<ContactListener> contactListener);

			void Update(Time elapsedTime);

			Physics3DSystem& operator=(const Physics3DSystem&) = delete;
			Physics3DSystem& operator=(Physics3DSystem&&) = delete;

			struct NAZARA_PHYSICS3D_API ContactListener
			{
				virtual ~ContactListener();

				virtual PhysContactValidateResult3D ValidateContact(entt::handle entity1, const PhysBody3D* body1, entt::handle entity2, const PhysBody3D* body2, const Vector3f& baseOffset, const ShapeCollisionInfo& collisionResult) = 0;

				virtual void OnContactAdded(entt::handle entity1, const PhysBody3D* body1, entt::handle entity2, const PhysBody3D* body2, const PhysContact3D& physContact, PhysContactResponse3D& physContactResponse) = 0;
				virtual void OnContactPersisted(entt::handle entity1, const PhysBody3D* body1, entt::handle entity2, const PhysBody3D* body2, const PhysContact3D& physContact, PhysContactResponse3D& physContactResponse) = 0;
				virtual void OnContactRemoved(entt::handle entity1, UInt32 body1Index, const PhysBody3D* body1, UInt32 subShapeID1, entt::handle entity2, UInt32 body2Index, const PhysBody3D* body2, UInt32 subShapeID2) = 0;
			};

			struct PointCollisionInfo : PhysWorld3D::PointCollisionInfo
			{
				entt::handle hitEntity;
			};

			struct RaycastHit : PhysWorld3D::RaycastHit
			{
				entt::handle hitEntity;
			};

			struct ShapeCollisionInfo : PhysWorld3D::ShapeCollisionInfo
			{
				entt::handle hitEntity;
			};

		private:
			void OnBodyConstruct(entt::registry& registry, entt::entity entity);
			void OnBodyDestruct(entt::registry& registry, entt::entity entity);
			void OnCharacterConstruct(entt::registry& registry, entt::entity entity);
			void OnCharacterDestruct(entt::registry& registry, entt::entity entity);

			template<typename T> void ReplicateEntities();

			std::size_t m_stepCount;
			std::vector<entt::entity> m_bodyIndicesToEntity;
			entt::registry& m_registry;
			entt::observer m_characterConstructObserver;
			entt::observer m_rigidBodyConstructObserver;
			entt::scoped_connection m_bodyConstructConnection;
			entt::scoped_connection m_bodyDestructConnection;
			entt::scoped_connection m_characterConstructConnection;
			entt::scoped_connection m_characterDestructConnection;
			PhysWorld3D m_physWorld;
	};
}

#include <Nazara/Physics3D/Systems/Physics3DSystem.inl>

#endif // NAZARA_PHYSICS3D_SYSTEMS_PHYSICS3DSYSTEM_HPP
