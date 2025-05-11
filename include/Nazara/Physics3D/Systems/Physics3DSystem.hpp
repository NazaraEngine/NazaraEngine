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
#include <flecs.h>
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

			Physics3DSystem(flecs::world& world, Settings&& settings = PhysWorld3D::BuildDefaultSettings());
			Physics3DSystem(const Physics3DSystem&) = delete;
			Physics3DSystem(Physics3DSystem&&) = delete;
			~Physics3DSystem();

			bool CollisionQuery(const Vector3f& point, const FunctionRef<std::optional<float>(const PointCollisionInfo& collisionInfo)>& callback, const PhysBroadphaseLayerFilter3D* broadphaseFilter = nullptr, const PhysObjectLayerFilter3D* objectLayerFilter = nullptr, const PhysBodyFilter3D* bodyFilter = nullptr);
			bool CollisionQuery(const Collider3D& collider, const Matrix4f& colliderTransform, const FunctionRef<std::optional<float>(const ShapeCollisionInfo& hitInfo)>& callback, const PhysBroadphaseLayerFilter3D* broadphaseFilter = nullptr, const PhysObjectLayerFilter3D* objectLayerFilter = nullptr, const PhysBodyFilter3D* bodyFilter = nullptr);
			bool CollisionQuery(const Collider3D& collider, const Matrix4f& colliderTransform, const Vector3f& colliderScale, const FunctionRef<std::optional<float>(const ShapeCollisionInfo& hitInfo)>& callback, const PhysBroadphaseLayerFilter3D* broadphaseFilter = nullptr, const PhysObjectLayerFilter3D* objectLayerFilter = nullptr, const PhysBodyFilter3D* bodyFilter = nullptr);

			inline PhysWorld3D& GetPhysWorld();
			inline const PhysWorld3D& GetPhysWorld() const;
			inline flecs::entity GetRigidBodyEntity(UInt32 bodyIndex) const;

			bool RaycastQuery(const Vector3f& from, const Vector3f& to, const FunctionRef<std::optional<float>(const RaycastHit& hitInfo)>& callback, const PhysBroadphaseLayerFilter3D* broadphaseFilter = nullptr, const PhysObjectLayerFilter3D* objectLayerFilter = nullptr, const PhysBodyFilter3D* bodyFilter = nullptr);
			bool RaycastQueryFirst(const Vector3f& from, const Vector3f& to, const FunctionRef<void(const RaycastHit& hitInfo)>& callback, const PhysBroadphaseLayerFilter3D* broadphaseFilter = nullptr, const PhysObjectLayerFilter3D* objectLayerFilter = nullptr, const PhysBodyFilter3D* bodyFilter = nullptr);

			void SetContactListener(std::unique_ptr<ContactListener> contactListener);

			void Update(Time elapsedTime);

			Physics3DSystem& operator=(const Physics3DSystem&) = delete;
			Physics3DSystem& operator=(Physics3DSystem&&) = delete;

			struct NAZARA_PHYSICS3D_API ContactListener
			{
				virtual ~ContactListener();

				virtual PhysContactValidateResult3D ValidateContact(flecs::entity entity1, const PhysBody3D* body1, flecs::entity entity2, const PhysBody3D* body2, const Vector3f& baseOffset, const ShapeCollisionInfo& collisionResult) = 0;

				virtual void OnContactAdded(flecs::entity entity1, const PhysBody3D* body1, flecs::entity entity2, const PhysBody3D* body2, const PhysContact3D& physContact, PhysContactResponse3D& physContactResponse) = 0;
				virtual void OnContactPersisted(flecs::entity entity1, const PhysBody3D* body1, flecs::entity entity2, const PhysBody3D* body2, const PhysContact3D& physContact, PhysContactResponse3D& physContactResponse) = 0;
				virtual void OnContactRemoved(flecs::entity entity1, UInt32 body1Index, const PhysBody3D* body1, UInt32 subShapeID1, flecs::entity entity2, UInt32 body2Index, const PhysBody3D* body2, UInt32 subShapeID2) = 0;
			};

			struct PointCollisionInfo : PhysWorld3D::PointCollisionInfo
			{
				flecs::entity hitEntity;
			};

			struct RaycastHit : PhysWorld3D::RaycastHit
			{
				flecs::entity hitEntity;
			};

			struct ShapeCollisionInfo : PhysWorld3D::ShapeCollisionInfo
			{
				flecs::entity hitEntity;
			};

		private:
			template<typename T> void ReplicateEntities();

			std::vector<flecs::entity> m_bodyIndicesToEntity;
			flecs::world& m_world;
			flecs::entity m_characterConstructObserver;
			flecs::entity m_rigidBodyConstructObserver;
			flecs::entity m_bodyObserver;
			flecs::entity m_characterObserver;
			PhysWorld3D m_physWorld;
	};
}

#include <Nazara/Physics3D/Systems/Physics3DSystem.inl>

#endif // NAZARA_PHYSICS3D_SYSTEMS_PHYSICS3DSYSTEM_HPP
