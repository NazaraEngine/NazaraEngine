// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NDK_SYSTEMS_PHYSICSSYSTEM2D_HPP
#define NDK_SYSTEMS_PHYSICSSYSTEM2D_HPP

#include <Nazara/Physics2D/PhysWorld2D.hpp>
#include <NDK/EntityList.hpp>
#include <NDK/System.hpp>
#include <memory>

namespace Ndk
{
	class NDK_API PhysicsSystem2D : public System<PhysicsSystem2D>
	{
		friend class CollisionComponent2D;
		friend class PhysicsComponent2D;
		
		using ContactEndCallback = std::function<void(PhysicsSystem2D& world, Nz::Arbiter2D& arbiter, const EntityHandle& bodyA, const EntityHandle& bodyB, void* userdata)>;
		using ContactPreSolveCallback = std::function<bool(PhysicsSystem2D& world, Nz::Arbiter2D& arbiter, const EntityHandle& bodyA, const EntityHandle& bodyB, void* userdata)>;
		using ContactPostSolveCallback = std::function<void(PhysicsSystem2D& world, Nz::Arbiter2D& arbiter, const EntityHandle& bodyA, const EntityHandle& bodyB, void* userdata)>;
		using ContactStartCallback = std::function<bool(PhysicsSystem2D& world, Nz::Arbiter2D& arbiter, const EntityHandle& bodyA, const EntityHandle& bodyB, void* userdata)>;

		using DebugDrawCircleCallback = std::function<void(const Nz::Vector2f& origin, const Nz::RadianAnglef& rotation, float radius, Nz::Color outlineColor, Nz::Color fillColor, void* userdata)>;
		using DebugDrawDotCallback = std::function<void(const Nz::Vector2f& origin, float radius, Nz::Color color, void* userdata)>;
		using DebugDrawPolygonCallback = std::function<void(const Nz::Vector2f* vertices, std::size_t vertexCount, float radius, Nz::Color outlineColor, Nz::Color fillColor, void* userdata)>;
		using DebugDrawSegmentCallback = std::function<void(const Nz::Vector2f& first, const Nz::Vector2f& second, Nz::Color color, void* userdata)>;
		using DebugDrawTickSegmentCallback = std::function<void(const Nz::Vector2f& first, const Nz::Vector2f& second, float thickness, Nz::Color outlineColor, Nz::Color fillColor, void* userdata)>;
		using DebugDrawGetColorCallback = std::function<Nz::Color(const EntityHandle& body, std::size_t shapeIndex, void* userdata)>;

		public:
			struct Callback;
			struct DebugDrawOptions;
			struct NearestQueryResult;
			struct RaycastHit;

			PhysicsSystem2D();
			~PhysicsSystem2D() = default;

			void DebugDraw(const DebugDrawOptions& options, bool drawShapes = true, bool drawConstraints = true, bool drawCollisions = true);

			inline float GetDamping() const;
			inline Nz::Vector2f GetGravity() const;
			inline std::size_t GetIterationCount() const;
			inline std::size_t GetMaxStepCount() const;
			inline float GetStepSize() const;

			bool NearestBodyQuery(const Nz::Vector2f& from, float maxDistance, Nz::UInt32 collisionGroup, Nz::UInt32 categoryMask, Nz::UInt32 collisionMask, EntityHandle* nearestBody = nullptr);
			bool NearestBodyQuery(const Nz::Vector2f& from, float maxDistance, Nz::UInt32 collisionGroup, Nz::UInt32 categoryMask, Nz::UInt32 collisionMask, NearestQueryResult* result);

			bool RaycastQuery(const Nz::Vector2f& from, const Nz::Vector2f& to, float radius, Nz::UInt32 collisionGroup, Nz::UInt32 categoryMask, Nz::UInt32 collisionMask, std::vector<RaycastHit>* hitInfos);
			bool RaycastQueryFirst(const Nz::Vector2f& from, const Nz::Vector2f& to, float radius, Nz::UInt32 collisionGroup, Nz::UInt32 categoryMask, Nz::UInt32 collisionMask, RaycastHit* hitInfo = nullptr);

			void RegionQuery(const Nz::Rectf& boundingBox, Nz::UInt32 collisionGroup, Nz::UInt32 categoryMask, Nz::UInt32 collisionMask, std::vector<EntityHandle>* bodies);

			void RegisterCallbacks(unsigned int collisionId, Callback callbacks);
			void RegisterCallbacks(unsigned int collisionIdA, unsigned int collisionIdB, Callback callbacks);

			inline void SetDamping(float dampingValue);
			inline void SetGravity(const Nz::Vector2f& gravity);
			inline void SetIterationCount(std::size_t iterationCount);
			inline void SetMaxStepCount(std::size_t maxStepCount);
			inline void SetStepSize(float stepSize);

			inline void UseSpatialHash(float cellSize, std::size_t entityCount);

			struct Callback
			{
				ContactEndCallback endCallback = nullptr;
				ContactPreSolveCallback preSolveCallback = nullptr;
				ContactPostSolveCallback postSolveCallback = nullptr;
				ContactStartCallback startCallback = nullptr;
				void* userdata;
			};

			struct DebugDrawOptions
			{
				Nz::Color constraintColor;
				Nz::Color collisionPointColor;
				Nz::Color shapeOutlineColor;

				DebugDrawCircleCallback circleCallback;
				DebugDrawGetColorCallback colorCallback;
				DebugDrawDotCallback dotCallback;
				DebugDrawPolygonCallback polygonCallback;
				DebugDrawSegmentCallback segmentCallback;
				DebugDrawTickSegmentCallback thickSegmentCallback;

				void* userdata;
			};

			struct NearestQueryResult
			{
				EntityHandle nearestBody;
				Nz::Vector2f closestPoint;
				Nz::Vector2f fraction;
				float distance;
			};

			struct RaycastHit
			{
				EntityHandle body;
				Nz::Vector2f hitPos;
				Nz::Vector2f hitNormal;
				float fraction;
			};

			static SystemIndex systemIndex;

		private:
			void CreatePhysWorld() const;
			const EntityHandle& GetEntityFromBody(const Nz::RigidBody2D& body) const;
			inline Nz::PhysWorld2D& GetPhysWorld();
			inline const Nz::PhysWorld2D& GetPhysWorld() const;
			void OnEntityValidation(Entity* entity, bool justAdded) override;
			void OnUpdate(float elapsedTime) override;

			EntityList m_dynamicObjects;
			EntityList m_staticObjects;
			mutable std::unique_ptr<Nz::PhysWorld2D> m_physWorld; ///TODO: std::optional (Should I make a Nz::Optional class?)
	};
}

#include <NDK/Systems/PhysicsSystem2D.inl>

#endif // NDK_SYSTEMS_PHYSICSSYSTEM2D_HPP
