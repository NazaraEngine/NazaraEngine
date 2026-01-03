// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics2D module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_PHYSICS2D_SYSTEMS_PHYSICS2DSYSTEM_HPP
#define NAZARA_PHYSICS2D_SYSTEMS_PHYSICS2DSYSTEM_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/EnttObserver.hpp>
#include <Nazara/Core/Time.hpp>
#include <Nazara/Physics2D/PhysWorld2D.hpp>
#include <Nazara/Physics2D/Components/RigidBody2DComponent.hpp>
#include <NazaraUtils/TypeList.hpp>
#include <entt/entt.hpp>

namespace Nz
{
	class NAZARA_PHYSICS2D_API Physics2DSystem
	{
		using ContactEndCallback = std::function<void(PhysWorld2D& world, PhysArbiter2D& arbiter, entt::handle entityA, entt::handle entityB, void* userdata)>;
		using ContactPostSolveCallback = std::function<void(PhysWorld2D& world, PhysArbiter2D& arbiter, entt::handle entityA, entt::handle entityB, void* userdata)>;
		using ContactPreSolveCallback = std::function<bool(PhysWorld2D& world, PhysArbiter2D& arbiter, entt::handle entityA, entt::handle entityB, void* userdata)>;
		using ContactStartCallback = std::function<bool(PhysWorld2D& world, PhysArbiter2D& arbiter, entt::handle entityA, entt::handle entityB, void* userdata)>;

		public:
			static constexpr Int64 ExecutionOrder = 0;
			using Components = TypeList<RigidBody2DComponent, class NodeComponent>;

			struct ContactCallbacks;
			struct NearestQueryResult;
			struct RaycastHit;

			Physics2DSystem(entt::registry& registry);
			Physics2DSystem(const Physics2DSystem&) = delete;
			Physics2DSystem(Physics2DSystem&&) = delete;
			~Physics2DSystem();

			inline PhysWorld2D& GetPhysWorld();
			inline const PhysWorld2D& GetPhysWorld() const;
			inline entt::handle GetRigidBodyEntity(UInt32 bodyIndex) const;

			inline bool NearestBodyQuery(const Vector2f& from, float maxDistance, UInt32 collisionGroup, UInt32 categoryMask, UInt32 collisionMask, entt::handle* nearestEntity = nullptr);
			inline bool NearestBodyQuery(const Vector2f& from, float maxDistance, UInt32 collisionGroup, UInt32 categoryMask, UInt32 collisionMask, NearestQueryResult* result);

			inline void RaycastQuery(const Vector2f& from, const Vector2f& to, float radius, UInt32 collisionGroup, UInt32 categoryMask, UInt32 collisionMask, const FunctionRef<void(const RaycastHit&)>& callback);
			inline bool RaycastQuery(const Vector2f& from, const Vector2f& to, float radius, UInt32 collisionGroup, UInt32 categoryMask, UInt32 collisionMask, std::vector<RaycastHit>* hitInfos);
			inline bool RaycastQueryFirst(const Vector2f& from, const Vector2f& to, float radius, UInt32 collisionGroup, UInt32 categoryMask, UInt32 collisionMask, RaycastHit* hitInfo = nullptr);

			inline void RegionQuery(const Rectf& boundingBox, UInt32 collisionGroup, UInt32 categoryMask, UInt32 collisionMask, const FunctionRef<void(entt::handle)>& callback);
			inline void RegionQuery(const Rectf& boundingBox, UInt32 collisionGroup, UInt32 categoryMask, UInt32 collisionMask, std::vector<entt::handle>* bodies);

			inline void RegisterCallbacks(unsigned int collisionId, ContactCallbacks callbacks);
			inline void RegisterCallbacks(unsigned int collisionIdA, unsigned int collisionIdB, ContactCallbacks callbacks);

			void Update(Time elapsedTime);

			Physics2DSystem& operator=(const Physics2DSystem&) = delete;
			Physics2DSystem& operator=(Physics2DSystem&&) = delete;

			struct ContactCallbacks
			{
				ContactEndCallback endCallback = nullptr;
				ContactPostSolveCallback postSolveCallback = nullptr;
				ContactPreSolveCallback preSolveCallback = nullptr;
				ContactStartCallback startCallback = nullptr;
				void* userdata = nullptr;
			};

			struct NearestQueryResult : PhysWorld2D::NearestQueryResult
			{
				entt::handle nearestEntity;
			};

			struct RaycastHit : PhysWorld2D::RaycastHit
			{
				entt::handle nearestEntity;
			};

		private:
			void OnBodyConstruct(entt::registry& registry, entt::entity entity);
			void OnBodyDestruct(entt::registry& registry, entt::entity entity);
			PhysWorld2D::ContactCallbacks SetupContactCallbacks(ContactCallbacks callbacks);

			std::vector<entt::entity> m_bodyIndicesToEntity;
			entt::registry& m_registry;
			entt::scoped_connection m_bodyConstructConnection;
			entt::scoped_connection m_bodyDestructConnection;
			EnttObserver<TypeList<class RigidBody2DComponent, class NodeComponent>, TypeList<class DisabledComponent>> m_physicsObserver;
			PhysWorld2D m_physWorld;
	};
}

#include <Nazara/Physics2D/Systems/Physics2DSystem.inl>

#endif // NAZARA_PHYSICS2D_SYSTEMS_PHYSICS2DSYSTEM_HPP
