// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - ChipmunkPhysics2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CHIPMUNKPHYSICS2D_SYSTEMS_CHIPMUNKPHYSICS2DSYSTEM_HPP
#define NAZARA_CHIPMUNKPHYSICS2D_SYSTEMS_CHIPMUNKPHYSICS2DSYSTEM_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/ChipmunkPhysics2D/ChipmunkPhysWorld2D.hpp>
#include <Nazara/ChipmunkPhysics2D/Components/ChipmunkRigidBody2DComponent.hpp>
#include <Nazara/Core/Time.hpp>
#include <NazaraUtils/TypeList.hpp>
#include <entt/entt.hpp>

namespace Nz
{
	class NAZARA_CHIPMUNKPHYSICS2D_API ChipmunkPhysics2DSystem
	{
		using ContactEndCallback = std::function<void(ChipmunkPhysWorld2D& world, ChipmunkArbiter2D& arbiter, entt::handle entityA, entt::handle entityB, void* userdata)>;
		using ContactPostSolveCallback = std::function<void(ChipmunkPhysWorld2D& world, ChipmunkArbiter2D& arbiter, entt::handle entityA, entt::handle entityB, void* userdata)>;
		using ContactPreSolveCallback = std::function<bool(ChipmunkPhysWorld2D& world, ChipmunkArbiter2D& arbiter, entt::handle entityA, entt::handle entityB, void* userdata)>;
		using ContactStartCallback = std::function<bool(ChipmunkPhysWorld2D& world, ChipmunkArbiter2D& arbiter, entt::handle entityA, entt::handle entityB, void* userdata)>;

		public:
			static constexpr Int64 ExecutionOrder = 0;
			using Components = TypeList<ChipmunkRigidBody2DComponent, class NodeComponent>;

			struct ContactCallbacks;
			struct NearestQueryResult;
			struct RaycastHit;

			ChipmunkPhysics2DSystem(entt::registry& registry);
			ChipmunkPhysics2DSystem(const ChipmunkPhysics2DSystem&) = delete;
			ChipmunkPhysics2DSystem(ChipmunkPhysics2DSystem&&) = delete;
			~ChipmunkPhysics2DSystem();

			inline ChipmunkPhysWorld2D& GetPhysWorld();
			inline const ChipmunkPhysWorld2D& GetPhysWorld() const;
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

			ChipmunkPhysics2DSystem& operator=(const ChipmunkPhysics2DSystem&) = delete;
			ChipmunkPhysics2DSystem& operator=(ChipmunkPhysics2DSystem&&) = delete;

			struct ContactCallbacks
			{
				ContactEndCallback endCallback = nullptr;
				ContactPostSolveCallback postSolveCallback = nullptr;
				ContactPreSolveCallback preSolveCallback = nullptr;
				ContactStartCallback startCallback = nullptr;
				void* userdata = nullptr;
			};

			struct NearestQueryResult : ChipmunkPhysWorld2D::NearestQueryResult
			{
				entt::handle nearestEntity;
			};

			struct RaycastHit : ChipmunkPhysWorld2D::RaycastHit
			{
				entt::handle nearestEntity;
			};

		private:
			void OnBodyConstruct(entt::registry& registry, entt::entity entity);
			void OnBodyDestruct(entt::registry& registry, entt::entity entity);
			ChipmunkPhysWorld2D::ContactCallbacks SetupContactCallbacks(ContactCallbacks callbacks);

			std::vector<entt::entity> m_bodyIndicesToEntity;
			entt::registry& m_registry;
			entt::observer m_physicsConstructObserver;
			entt::scoped_connection m_bodyConstructConnection;
			entt::scoped_connection m_bodyDestructConnection;
			ChipmunkPhysWorld2D m_physWorld;
	};
}

#include <Nazara/ChipmunkPhysics2D/Systems/ChipmunkPhysics2DSystem.inl>

#endif // NAZARA_CHIPMUNKPHYSICS2D_SYSTEMS_CHIPMUNKPHYSICS2DSYSTEM_HPP
