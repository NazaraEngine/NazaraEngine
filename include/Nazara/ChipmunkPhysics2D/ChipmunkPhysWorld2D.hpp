// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - ChipmunkPhysics2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CHIPMUNKPHYSICS2D_CHIPMUNKPHYSWORLD2D_HPP
#define NAZARA_CHIPMUNKPHYSICS2D_CHIPMUNKPHYSWORLD2D_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/ChipmunkPhysics2D/ChipmunkRigidBody2D.hpp>
#include <Nazara/ChipmunkPhysics2D/Config.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/Time.hpp>
#include <Nazara/Math/Angle.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <NazaraUtils/Bitset.hpp>
#include <NazaraUtils/FunctionRef.hpp>
#include <NazaraUtils/Signal.hpp>
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

struct cpCollisionHandler;
struct cpSpace;

namespace Nz
{
	class ChipmunkArbiter2D;

	class NAZARA_CHIPMUNKPHYSICS2D_API ChipmunkPhysWorld2D
	{
		friend ChipmunkRigidBody2D;

		using ContactEndCallback = std::function<void(ChipmunkPhysWorld2D& world, ChipmunkArbiter2D& arbiter, ChipmunkRigidBody2D& bodyA, ChipmunkRigidBody2D& bodyB, void* userdata)>;
		using ContactPreSolveCallback = std::function<bool(ChipmunkPhysWorld2D& world, ChipmunkArbiter2D& arbiter, ChipmunkRigidBody2D& bodyA, ChipmunkRigidBody2D& bodyB, void* userdata)>;
		using ContactPostSolveCallback = std::function<void(ChipmunkPhysWorld2D& world, ChipmunkArbiter2D& arbiter, ChipmunkRigidBody2D& bodyA, ChipmunkRigidBody2D& bodyB, void* userdata)>;
		using ContactStartCallback = std::function<bool(ChipmunkPhysWorld2D& world, ChipmunkArbiter2D& arbiter, ChipmunkRigidBody2D& bodyA, ChipmunkRigidBody2D& bodyB, void* userdata)>;

		using DebugDrawCircleCallback = std::function<void(const Vector2f& origin, const RadianAnglef& rotation, float radius, const Color& outlineColor, const Color& fillColor, void* userdata)>;
		using DebugDrawDotCallback = std::function<void(const Vector2f& origin, float radius, const Color& color, void* userdata)>;
		using DebugDrawPolygonCallback = std::function<void(const Vector2f* vertices, std::size_t vertexCount, float radius, const Color& outlineColor, const Color& fillColor, void* userdata)>;
		using DebugDrawSegmentCallback = std::function<void(const Vector2f& first, const Vector2f& second, const Color& color, void* userdata)>;
		using DebugDrawTickSegmentCallback = std::function<void(const Vector2f& first, const Vector2f& second, float thickness, const Color& outlineColor, const Color& fillColor, void* userdata)>;
		using DebugDrawGetColorCallback = std::function<Color(ChipmunkRigidBody2D& body, std::size_t shapeIndex, void* userdata)>;

		public:
			struct ContactCallbacks;
			struct DebugDrawOptions;
			struct NearestQueryResult;
			struct RaycastHit;

			ChipmunkPhysWorld2D();
			ChipmunkPhysWorld2D(const ChipmunkPhysWorld2D&) = delete;
			ChipmunkPhysWorld2D(ChipmunkPhysWorld2D&&) = delete;
			~ChipmunkPhysWorld2D();

			void DebugDraw(const DebugDrawOptions& options, bool drawShapes = true, bool drawConstraints = true, bool drawCollisions = true) const;

			float GetDamping() const;
			Vector2f GetGravity() const;
			cpSpace* GetHandle() const;
			std::size_t GetIterationCount() const;
			std::size_t GetMaxStepCount() const;
			Time GetStepSize() const;

			bool NearestBodyQuery(const Vector2f& from, float maxDistance, UInt32 collisionGroup, UInt32 categoryMask, UInt32 collisionMask, ChipmunkRigidBody2D** nearestBody = nullptr);
			bool NearestBodyQuery(const Vector2f& from, float maxDistance, UInt32 collisionGroup, UInt32 categoryMask, UInt32 collisionMask, NearestQueryResult* result);

			void RaycastQuery(const Vector2f& from, const Vector2f& to, float radius, UInt32 collisionGroup, UInt32 categoryMask, UInt32 collisionMask, const FunctionRef<void(const RaycastHit&)>& callback);
			bool RaycastQuery(const Vector2f& from, const Vector2f& to, float radius, UInt32 collisionGroup, UInt32 categoryMask, UInt32 collisionMask, std::vector<RaycastHit>* hitInfos);
			bool RaycastQueryFirst(const Vector2f& from, const Vector2f& to, float radius, UInt32 collisionGroup, UInt32 categoryMask, UInt32 collisionMask, RaycastHit* hitInfo = nullptr);

			void RegionQuery(const Rectf& boundingBox, UInt32 collisionGroup, UInt32 categoryMask, UInt32 collisionMask, const FunctionRef<void(ChipmunkRigidBody2D*)>& callback);
			void RegionQuery(const Rectf& boundingBox, UInt32 collisionGroup, UInt32 categoryMask, UInt32 collisionMask, std::vector<ChipmunkRigidBody2D*>* bodies);

			void RegisterCallbacks(unsigned int collisionId, ContactCallbacks callbacks);
			void RegisterCallbacks(unsigned int collisionIdA, unsigned int collisionIdB, ContactCallbacks callbacks);

			void SetDamping(float dampingValue);
			void SetGravity(const Vector2f& gravity);
			void SetIterationCount(std::size_t iterationCount);
			void SetMaxStepCount(std::size_t maxStepCount);
			void SetSleepTime(Time sleepTime);
			void SetStepSize(Time stepSize);

			void Step(Time timestep);

			void UseSpatialHash(float cellSize, std::size_t entityCount);

			ChipmunkPhysWorld2D& operator=(const ChipmunkPhysWorld2D&) = delete;
			ChipmunkPhysWorld2D& operator=(ChipmunkPhysWorld2D&&) = delete;

			struct ContactCallbacks
			{
				ContactEndCallback endCallback = nullptr;
				ContactPostSolveCallback postSolveCallback = nullptr;
				ContactPreSolveCallback preSolveCallback = nullptr;
				ContactStartCallback startCallback = nullptr;
				void* userdata = nullptr;
			};

			struct DebugDrawOptions
			{
				Color constraintColor = Color::Blue();
				Color collisionPointColor = Color::Green();
				Color shapeOutlineColor = Color::Red();

				DebugDrawCircleCallback circleCallback;
				DebugDrawGetColorCallback colorCallback;
				DebugDrawDotCallback dotCallback;
				DebugDrawPolygonCallback polygonCallback;
				DebugDrawSegmentCallback segmentCallback;
				DebugDrawTickSegmentCallback thickSegmentCallback;

				void* userdata = nullptr;
			};

			struct NearestQueryResult
			{
				ChipmunkRigidBody2D* nearestBody;
				Vector2f closestPoint;
				Vector2f fraction;
				float distance;
			};

			struct RaycastHit
			{
				ChipmunkRigidBody2D* nearestBody;
				Vector2f hitPos;
				Vector2f hitNormal;
				float fraction;
			};

			NazaraSignal(OnPhysWorld2DPreStep, const ChipmunkPhysWorld2D* /*physWorld*/, float /*invStepCount*/);
			NazaraSignal(OnPhysWorld2DPostStep, const ChipmunkPhysWorld2D* /*physWorld*/, float /*invStepCount*/);

		private:
			using PostStep = std::function<void(ChipmunkRigidBody2D* body)>;

			static constexpr std::size_t FreeBodyIdGrowRate = 256;

			void DeferBodyAction(ChipmunkRigidBody2D& rigidBody, PostStep&& func);
			void InitCallbacks(cpCollisionHandler* handler, ContactCallbacks callbacks);
			inline UInt32 RegisterBody(ChipmunkRigidBody2D& rigidBody);
			inline void UnregisterBody(UInt32 bodyIndex);
			inline void UpdateBodyPointer(ChipmunkRigidBody2D& rigidBody);

			std::size_t m_maxStepCount;
			std::unordered_map<cpCollisionHandler*, std::unique_ptr<ContactCallbacks>> m_callbacks;
			std::unordered_map<UInt32, std::vector<PostStep>> m_rigidBodyPostSteps;
			std::vector<ChipmunkRigidBody2D*> m_bodies;
			cpSpace* m_handle;
			Bitset<UInt64> m_freeBodyIndices;
			Time m_stepSize;
			Time m_timestepAccumulator;
	};
}

#include <Nazara/ChipmunkPhysics2D/ChipmunkPhysWorld2D.inl>

#endif // NAZARA_CHIPMUNKPHYSICS2D_CHIPMUNKPHYSWORLD2D_HPP
