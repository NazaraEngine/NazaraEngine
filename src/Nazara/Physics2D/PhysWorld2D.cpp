// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics2D module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Physics2D/PhysWorld2D.hpp>
#include <Nazara/Physics2D/PhysArbiter2D.hpp>
#include <NazaraUtils/StackArray.hpp>
#include <chipmunk/chipmunk.h>

namespace Nz
{
	namespace
	{
		Color CpDebugColorToColor(cpSpaceDebugColor c)
		{
			return Color{ c.r, c.g, c.b, c.a };
		}

		cpSpaceDebugColor ColorToCpDebugColor(Color c)
		{
			return cpSpaceDebugColor{ c.r, c.g, c.b, c.a };
		}

		void CpCircleCallback(cpVect pos, cpFloat angle, cpFloat radius, cpSpaceDebugColor outlineColor, cpSpaceDebugColor fillColor, cpDataPointer userdata)
		{
			auto drawOptions = static_cast<PhysWorld2D::DebugDrawOptions*>(userdata);
			if (drawOptions->circleCallback)
				drawOptions->circleCallback(Vector2f(float(pos.x), float(pos.y)), RadianAnglef(float(angle)), float(radius), CpDebugColorToColor(outlineColor), CpDebugColorToColor(fillColor), drawOptions->userdata);
		}

		void CpDotCallback(cpFloat size, cpVect pos, cpSpaceDebugColor color, cpDataPointer userdata)
		{
			auto drawOptions = static_cast<PhysWorld2D::DebugDrawOptions*>(userdata);
			if (drawOptions->dotCallback)
				drawOptions->dotCallback(Vector2f(float(pos.x), float(pos.y)), float(size), CpDebugColorToColor(color), drawOptions->userdata);
		}

		void CpPolygonCallback(int vertexCount, const cpVect* vertices, cpFloat radius, cpSpaceDebugColor outlineColor, cpSpaceDebugColor fillColor, cpDataPointer userdata)
		{
			auto drawOptions = static_cast<PhysWorld2D::DebugDrawOptions*>(userdata);
			if (drawOptions->polygonCallback)
			{
				if constexpr (sizeof(cpVect) == sizeof(Vector2f))
					drawOptions->polygonCallback(reinterpret_cast<const Vector2f*>(vertices), vertexCount, float(radius), CpDebugColorToColor(outlineColor), CpDebugColorToColor(fillColor), drawOptions->userdata);
				else
				{
					StackArray<Vector2f> nVertices = NazaraStackArray(Vector2f, vertexCount);
					for (int i = 0; i < vertexCount; ++i)
						nVertices[i] = Vector2f(float(vertices[i].x), float(vertices[i].y));

					drawOptions->polygonCallback(nVertices.data(), vertexCount, float(radius), CpDebugColorToColor(outlineColor), CpDebugColorToColor(fillColor), drawOptions->userdata);
				}
			}
		}

		void CpSegmentCallback(cpVect a, cpVect b, cpSpaceDebugColor color, cpDataPointer userdata)
		{
			auto drawOptions = static_cast<PhysWorld2D::DebugDrawOptions*>(userdata);
			if (drawOptions->segmentCallback)
				drawOptions->segmentCallback(Vector2f(float(a.x), float(a.y)), Vector2f(float(b.x), float(b.y)), CpDebugColorToColor(color), drawOptions->userdata);
		}

		void CpThickSegmentCallback(cpVect a, cpVect b, cpFloat radius, cpSpaceDebugColor outlineColor, cpSpaceDebugColor fillColor, cpDataPointer userdata)
		{
			auto drawOptions = static_cast<PhysWorld2D::DebugDrawOptions*>(userdata);
			if (drawOptions->thickSegmentCallback)
				drawOptions->thickSegmentCallback(Vector2f(float(a.x), float(a.y)), Vector2f(float(b.x), float(b.y)), float(radius), CpDebugColorToColor(outlineColor), CpDebugColorToColor(fillColor), drawOptions->userdata);
		}

		cpSpaceDebugColor CpShapeColorCallback(cpShape* shape, cpDataPointer userdata)
		{
			auto drawOptions = static_cast<PhysWorld2D::DebugDrawOptions*>(userdata);
			if (drawOptions->colorCallback)
			{
				RigidBody2D& rigidBody = *static_cast<RigidBody2D*>(cpShapeGetUserData(shape));
				return ColorToCpDebugColor(drawOptions->colorCallback(rigidBody, rigidBody.GetShapeIndex(shape), drawOptions->userdata));
			}
			else
				return cpSpaceDebugColor{1.f, 0.f, 0.f, 1.f};
		}
	}

	PhysWorld2D::PhysWorld2D() :
	m_maxStepCount(50),
	m_stepSize(Time::TickDuration(120)),
	m_timestepAccumulator(Time::Zero())
	{
		m_handle = cpSpaceNew();
		cpSpaceSetUserData(m_handle, this);
	}

	PhysWorld2D::~PhysWorld2D()
	{
		cpSpaceFree(m_handle);
	}

	void PhysWorld2D::DebugDraw(const DebugDrawOptions& options, bool drawShapes, bool drawConstraints, bool drawCollisions) const
	{
		auto ColorToCpDebugColor = [](Color c) -> cpSpaceDebugColor
		{
			return cpSpaceDebugColor{ c.r, c.g, c.b, c.a };
		};

		cpSpaceDebugDrawOptions drawOptions;
		drawOptions.collisionPointColor = ColorToCpDebugColor(options.collisionPointColor);
		drawOptions.constraintColor = ColorToCpDebugColor(options.constraintColor);
		drawOptions.shapeOutlineColor = ColorToCpDebugColor(options.shapeOutlineColor);
		drawOptions.data = const_cast<DebugDrawOptions*>(&options); //< won't be used to write

		std::underlying_type_t<cpSpaceDebugDrawFlags> drawFlags = 0;
		if (drawCollisions)
			drawFlags |= CP_SPACE_DEBUG_DRAW_COLLISION_POINTS;

		if (drawConstraints)
			drawFlags |= CP_SPACE_DEBUG_DRAW_CONSTRAINTS;

		if (drawShapes)
			drawFlags |= CP_SPACE_DEBUG_DRAW_SHAPES;

		drawOptions.flags = static_cast<cpSpaceDebugDrawFlags>(drawFlags);

		// Callback trampoline
		drawOptions.colorForShape = CpShapeColorCallback;
		drawOptions.drawCircle = CpCircleCallback;
		drawOptions.drawDot = CpDotCallback;
		drawOptions.drawFatSegment = CpThickSegmentCallback;
		drawOptions.drawPolygon = CpPolygonCallback;
		drawOptions.drawSegment = CpSegmentCallback;

		cpSpaceDebugDraw(m_handle, &drawOptions);
	}

	float PhysWorld2D::GetDamping() const
	{
		return float(cpSpaceGetDamping(m_handle));
	}

	Vector2f PhysWorld2D::GetGravity() const
	{
		cpVect gravity = cpSpaceGetGravity(m_handle);
		return Vector2f(Vector2<cpFloat>(gravity.x, gravity.y));
	}

	cpSpace* PhysWorld2D::GetHandle() const
	{
		return m_handle;
	}

	std::size_t PhysWorld2D::GetIterationCount() const
	{
		return cpSpaceGetIterations(m_handle);
	}

	std::size_t PhysWorld2D::GetMaxStepCount() const
	{
		return m_maxStepCount;
	}

	Time PhysWorld2D::GetStepSize() const
	{
		return m_stepSize;
	}

	bool PhysWorld2D::NearestBodyQuery(const Vector2f & from, float maxDistance, UInt32 collisionGroup, UInt32 categoryMask, UInt32 collisionMask, RigidBody2D** nearestBody)
	{
		cpShapeFilter filter = cpShapeFilterNew(collisionGroup, categoryMask, collisionMask);

		if (cpShape* shape = cpSpacePointQueryNearest(m_handle, {from.x, from.y}, maxDistance, filter, nullptr))
		{
			if (nearestBody)
				*nearestBody = static_cast<RigidBody2D*>(cpShapeGetUserData(shape));

			return true;
		}
		else
			return false;
	}

	bool PhysWorld2D::NearestBodyQuery(const Vector2f& from, float maxDistance, UInt32 collisionGroup, UInt32 categoryMask, UInt32 collisionMask, NearestQueryResult* result)
	{
		cpShapeFilter filter = cpShapeFilterNew(collisionGroup, categoryMask, collisionMask);

		if (result)
		{
			cpPointQueryInfo queryInfo;

			if (cpSpacePointQueryNearest(m_handle, { from.x, from.y }, maxDistance, filter, &queryInfo))
			{
				result->closestPoint = Vector2f(Vector2<cpFloat>(queryInfo.point.x, queryInfo.point.y));
				result->distance = float(queryInfo.distance);
				result->fraction = Vector2f(Vector2<cpFloat>(queryInfo.gradient.x, queryInfo.gradient.y));
				result->nearestBody = static_cast<RigidBody2D*>(cpShapeGetUserData(queryInfo.shape));

				return true;
			}
			else
				return false;
		}
		else
		{
			if (cpSpacePointQueryNearest(m_handle, { from.x, from.y }, maxDistance, filter, nullptr))
				return true;
			else
				return false;
		}
	}

	void PhysWorld2D::RaycastQuery(const Vector2f& from, const Vector2f& to, float radius, UInt32 collisionGroup, UInt32 categoryMask, UInt32 collisionMask, const FunctionRef<void(const RaycastHit&)>& callback)
	{
		using CallbackType = std::remove_reference_t<decltype(callback)>;

		auto cpCallback = [](cpShape* shape, cpVect point, cpVect normal, cpFloat alpha, void* data)
		{
			CallbackType& callback = *static_cast<CallbackType*>(data);

			RaycastHit hitInfo;
			hitInfo.fraction = float(alpha);
			hitInfo.hitNormal = Vector2f(Vector2<cpFloat>(normal.x, normal.y));
			hitInfo.hitPos = Vector2f(Vector2<cpFloat>(point.x, point.y));
			hitInfo.nearestBody = static_cast<RigidBody2D*>(cpShapeGetUserData(shape));

			callback(hitInfo);
		};

		cpShapeFilter filter = cpShapeFilterNew(collisionGroup, categoryMask, collisionMask);
		cpSpaceSegmentQuery(m_handle, { from.x, from.y }, { to.x, to.y }, radius, filter, cpCallback, const_cast<void*>(static_cast<const void*>(&callback)));
	}

	bool PhysWorld2D::RaycastQuery(const Vector2f& from, const Vector2f& to, float radius, UInt32 collisionGroup, UInt32 categoryMask, UInt32 collisionMask, std::vector<RaycastHit>* hitInfos)
	{
		using ResultType = decltype(hitInfos);

		auto callback = [](cpShape* shape, cpVect point, cpVect normal, cpFloat alpha, void* data)
		{
			ResultType results = static_cast<ResultType>(data);

			RaycastHit hitInfo;
			hitInfo.fraction = float(alpha);
			hitInfo.hitNormal = Vector2f(Vector2<cpFloat>(normal.x, normal.y));
			hitInfo.hitPos = Vector2f(Vector2<cpFloat>(point.x, point.y));
			hitInfo.nearestBody = static_cast<RigidBody2D*>(cpShapeGetUserData(shape));

			results->emplace_back(std::move(hitInfo));
		};

		cpShapeFilter filter = cpShapeFilterNew(collisionGroup, categoryMask, collisionMask);

		std::size_t previousSize = hitInfos->size();
		cpSpaceSegmentQuery(m_handle, { from.x, from.y }, { to.x, to.y }, radius, filter, callback, hitInfos);

		return hitInfos->size() != previousSize;
	}

	bool PhysWorld2D::RaycastQueryFirst(const Vector2f& from, const Vector2f& to, float radius, UInt32 collisionGroup, UInt32 categoryMask, UInt32 collisionMask, RaycastHit* hitInfo)
	{
		cpShapeFilter filter = cpShapeFilterNew(collisionGroup, categoryMask, collisionMask);

		if (hitInfo)
		{
			cpSegmentQueryInfo queryInfo;

			if (cpSpaceSegmentQueryFirst(m_handle, { from.x, from.y }, { to.x, to.y }, radius, filter, &queryInfo))
			{
				hitInfo->fraction = float(queryInfo.alpha);
				hitInfo->hitNormal = Vector2f(Vector2<cpFloat>(queryInfo.normal.x, queryInfo.normal.y));
				hitInfo->hitPos = Vector2f(Vector2<cpFloat>(queryInfo.point.x, queryInfo.point.y));
				hitInfo->nearestBody = static_cast<RigidBody2D*>(cpShapeGetUserData(queryInfo.shape));

				return true;
			}
			else
				return false;
		}
		else
		{
			if (cpSpaceSegmentQueryFirst(m_handle, { from.x, from.y }, { to.x, to.y }, radius, filter, nullptr))
				return true;
			else
				return false;
		}
	}

	void PhysWorld2D::RegionQuery(const Rectf& boundingBox, UInt32 collisionGroup, UInt32 categoryMask, UInt32 collisionMask, const FunctionRef<void(RigidBody2D*)>& callback)
	{
		using CallbackType = std::remove_reference_t<decltype(callback)>;

		auto cpCallback = [](cpShape* shape, void* data)
		{
			CallbackType& callback = *static_cast<CallbackType*>(data);
			callback(static_cast<RigidBody2D*>(cpShapeGetUserData(shape)));
		};

		cpShapeFilter filter = cpShapeFilterNew(collisionGroup, categoryMask, collisionMask);
		cpSpaceBBQuery(m_handle, cpBBNew(boundingBox.x, boundingBox.y, boundingBox.x + boundingBox.width, boundingBox.y + boundingBox.height), filter, cpCallback, const_cast<void*>(static_cast<const void*>(&callback)));
	}

	void PhysWorld2D::RegionQuery(const Rectf& boundingBox, UInt32 collisionGroup, UInt32 categoryMask, UInt32 collisionMask, std::vector<RigidBody2D*>* bodies)
	{
		using ResultType = decltype(bodies);

		auto callback = [] (cpShape* shape, void* data)
		{
			ResultType results = static_cast<ResultType>(data);
			results->push_back(static_cast<RigidBody2D*>(cpShapeGetUserData(shape)));
		};

		cpShapeFilter filter = cpShapeFilterNew(collisionGroup, categoryMask, collisionMask);
		cpSpaceBBQuery(m_handle, cpBBNew(boundingBox.x, boundingBox.y, boundingBox.x + boundingBox.width, boundingBox.y + boundingBox.height), filter, callback, bodies);
	}

	void PhysWorld2D::RegisterCallbacks(unsigned int collisionId, ContactCallbacks callbacks)
	{
		InitCallbacks(cpSpaceAddWildcardHandler(m_handle, collisionId), std::move(callbacks));
	}

	void PhysWorld2D::RegisterCallbacks(unsigned int collisionIdA, unsigned int collisionIdB, ContactCallbacks callbacks)
	{
		InitCallbacks(cpSpaceAddCollisionHandler(m_handle, collisionIdA, collisionIdB), std::move(callbacks));
	}

	void PhysWorld2D::SetDamping(float dampingValue)
	{
		cpSpaceSetDamping(m_handle, dampingValue);
	}

	void PhysWorld2D::SetGravity(const Vector2f& gravity)
	{
		cpSpaceSetGravity(m_handle, cpv(gravity.x, gravity.y));
	}

	void PhysWorld2D::SetIterationCount(std::size_t iterationCount)
	{
		cpSpaceSetIterations(m_handle, SafeCast<int>(iterationCount));
	}

	void PhysWorld2D::SetMaxStepCount(std::size_t maxStepCount)
	{
		m_maxStepCount = maxStepCount;
	}

	void PhysWorld2D::SetSleepTime(Time sleepTime)
	{
		if (sleepTime > Time::Zero())
			cpSpaceSetSleepTimeThreshold(m_handle, sleepTime.AsSeconds<cpFloat>());
		else
			cpSpaceSetSleepTimeThreshold(m_handle, std::numeric_limits<cpFloat>::infinity());
	}

	void PhysWorld2D::SetStepSize(Time stepSize)
	{
		m_stepSize = stepSize;
	}

	void PhysWorld2D::Step(Time timestep)
	{
		m_timestepAccumulator += timestep;

		std::size_t stepCount = std::min(static_cast<std::size_t>(static_cast<Int64>(m_timestepAccumulator / m_stepSize)), m_maxStepCount);
		float invStepCount = 1.f / stepCount;

		cpFloat dt = m_stepSize.AsSeconds<float>(); //< FIXME: AsSeconds<cpFloat> is more precise but it fails unit tests on Linux
		for (std::size_t i = 0; i < stepCount; ++i)
		{
			OnPhysWorld2DPreStep(this, invStepCount);

			cpSpaceStep(m_handle, dt);

			OnPhysWorld2DPostStep(this, invStepCount);
			if (!m_rigidBodyPostSteps.empty())
			{
				for (auto&& [bodyIndex, callbackVec] : m_rigidBodyPostSteps)
				{
					RigidBody2D* rigidBody = m_bodies[bodyIndex];
					assert(rigidBody);

					for (const auto& step : callbackVec)
						step(rigidBody);
				}

				m_rigidBodyPostSteps.clear();
			}

			m_timestepAccumulator -= m_stepSize;
		}
	}

	void PhysWorld2D::UseSpatialHash(float cellSize, std::size_t entityCount)
	{
		cpSpaceUseSpatialHash(m_handle, cpFloat(cellSize), int(entityCount));
	}

	void PhysWorld2D::InitCallbacks(cpCollisionHandler* handler, ContactCallbacks callbacks)
	{
		auto it = m_callbacks.find(handler);
		if (it == m_callbacks.end())
			it = m_callbacks.emplace(handler, std::make_unique<ContactCallbacks>(std::move(callbacks))).first;
		else
			it->second = std::make_unique<ContactCallbacks>(std::move(callbacks));

		ContactCallbacks* callbackFunctions = it->second.get();
		handler->userData = callbackFunctions;

		if (callbackFunctions->startCallback)
		{
			handler->beginFunc = [](cpArbiter* arb, cpSpace* space, void* data) -> cpBool
			{
				cpBody* firstBody;
				cpBody* secondBody;
				cpArbiterGetBodies(arb, &firstBody, &secondBody);

				PhysWorld2D* world = static_cast<PhysWorld2D*>(cpSpaceGetUserData(space));
				RigidBody2D* firstRigidBody = static_cast<RigidBody2D*>(cpBodyGetUserData(firstBody));
				RigidBody2D* secondRigidBody = static_cast<RigidBody2D*>(cpBodyGetUserData(secondBody));

				PhysArbiter2D arbiter(arb);

				const ContactCallbacks* customCallbacks = static_cast<const ContactCallbacks*>(data);
				if (customCallbacks->startCallback(*world, arbiter, *firstRigidBody, *secondRigidBody, customCallbacks->userdata))
					return cpTrue;
				else
					return cpFalse;
			};
		}
		else
		{
			handler->beginFunc = [](cpArbiter*, cpSpace*, void*) -> cpBool
			{
				return cpTrue;
			};
		}

		if (callbackFunctions->endCallback)
		{
			handler->separateFunc = [](cpArbiter* arb, cpSpace* space, void* data)
			{
				cpBody* firstBody;
				cpBody* secondBody;
				cpArbiterGetBodies(arb, &firstBody, &secondBody);

				PhysWorld2D* world = static_cast<PhysWorld2D*>(cpSpaceGetUserData(space));
				RigidBody2D* firstRigidBody = static_cast<RigidBody2D*>(cpBodyGetUserData(firstBody));
				RigidBody2D* secondRigidBody = static_cast<RigidBody2D*>(cpBodyGetUserData(secondBody));

				PhysArbiter2D arbiter(arb);

				const ContactCallbacks* customCallbacks = static_cast<const ContactCallbacks*>(data);
				customCallbacks->endCallback(*world, arbiter, *firstRigidBody, *secondRigidBody, customCallbacks->userdata);
			};
		}
		else
		{
			handler->separateFunc = [](cpArbiter*, cpSpace*, void*)
			{
			};
		}

		if (callbackFunctions->preSolveCallback)
		{
			handler->preSolveFunc = [](cpArbiter* arb, cpSpace* space, void* data) -> cpBool
			{
				cpBody* firstBody;
				cpBody* secondBody;
				cpArbiterGetBodies(arb, &firstBody, &secondBody);

				PhysWorld2D* world = static_cast<PhysWorld2D*>(cpSpaceGetUserData(space));
				RigidBody2D* firstRigidBody = static_cast<RigidBody2D*>(cpBodyGetUserData(firstBody));
				RigidBody2D* secondRigidBody = static_cast<RigidBody2D*>(cpBodyGetUserData(secondBody));

				PhysArbiter2D arbiter(arb);

				const ContactCallbacks* customCallbacks = static_cast<const ContactCallbacks*>(data);
				if (customCallbacks->preSolveCallback(*world, arbiter, *firstRigidBody, *secondRigidBody, customCallbacks->userdata))
					return cpTrue;
				else
					return cpFalse;
			};
		}
		else
		{
			handler->preSolveFunc = [](cpArbiter*, cpSpace*, void*) -> cpBool
			{
				return cpTrue;
			};
		}

		if (callbackFunctions->postSolveCallback)
		{
			handler->postSolveFunc = [](cpArbiter* arb, cpSpace* space, void *data)
			{
				cpBody* firstBody;
				cpBody* secondBody;
				cpArbiterGetBodies(arb, &firstBody, &secondBody);

				PhysWorld2D* world = static_cast<PhysWorld2D*>(cpSpaceGetUserData(space));
				RigidBody2D* firstRigidBody = static_cast<RigidBody2D*>(cpBodyGetUserData(firstBody));
				RigidBody2D* secondRigidBody = static_cast<RigidBody2D*>(cpBodyGetUserData(secondBody));

				PhysArbiter2D arbiter(arb);

				const ContactCallbacks* customCallbacks = static_cast<const ContactCallbacks*>(data);
				customCallbacks->postSolveCallback(*world, arbiter, *firstRigidBody, *secondRigidBody, customCallbacks->userdata);
			};
		}
		else
		{
			handler->postSolveFunc = [](cpArbiter*, cpSpace*, void*)
			{
			};
		}
	}

	void PhysWorld2D::DeferBodyAction(RigidBody2D& rigidBody, PostStep&& func)
	{
		// If space isn't locked, no need to wait
		if (!cpSpaceIsLocked(m_handle))
		{
			func(&rigidBody);
			return;
		}

		UInt32 bodyIndex = rigidBody.GetBodyIndex();
		m_rigidBodyPostSteps[bodyIndex].emplace_back(std::move(func));
	}
}
