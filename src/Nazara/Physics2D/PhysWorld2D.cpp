// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics2D/PhysWorld2D.hpp>
#include <Nazara/Core/MemoryHelper.hpp>
#include <chipmunk/chipmunk.h>
#include <Nazara/Physics2D/Debug.hpp>

namespace Nz
{
	namespace
	{
		Color CpDebugColorToColor(cpSpaceDebugColor c)
		{
			return Color{ static_cast<Nz::UInt8>(c.r * 255.f), static_cast<Nz::UInt8>(c.g * 255.f), static_cast<Nz::UInt8>(c.b * 255.f), static_cast<Nz::UInt8>(c.a * 255.f) };
		}

		cpSpaceDebugColor ColorToCpDebugColor(Color c)
		{
			return cpSpaceDebugColor{ c.r / 255.f, c.g / 255.f, c.b / 255.f, c.a / 255.f };
		}

		void DrawCircle(cpVect pos, cpFloat angle, cpFloat radius, cpSpaceDebugColor outlineColor, cpSpaceDebugColor fillColor, cpDataPointer userdata)
		{
			auto drawOptions = static_cast<PhysWorld2D::DebugDrawOptions*>(userdata);
			if (drawOptions->circleCallback)
				drawOptions->circleCallback(Vector2f(float(pos.x), float(pos.y)), float(angle), float(radius), CpDebugColorToColor(outlineColor), CpDebugColorToColor(fillColor), drawOptions->userdata);
		}

		void DrawDot(cpFloat size, cpVect pos, cpSpaceDebugColor color, cpDataPointer userdata)
		{
			auto drawOptions = static_cast<PhysWorld2D::DebugDrawOptions*>(userdata);
			if (drawOptions->dotCallback)
				drawOptions->dotCallback(Vector2f(float(pos.x), float(pos.y)), float(size), CpDebugColorToColor(color), drawOptions->userdata);
		}

		using DebugDrawPolygonCallback = std::function<void(const Vector2f* vertices, std::size_t vertexCount, float radius, Color outlineColor, Color fillColor, void* userdata)>;

		void DrawPolygon(int vertexCount, const cpVect* vertices, cpFloat radius, cpSpaceDebugColor outlineColor, cpSpaceDebugColor fillColor, cpDataPointer userdata)
		{
			auto drawOptions = static_cast<PhysWorld2D::DebugDrawOptions*>(userdata);
			if (drawOptions->polygonCallback)
			{
				//TODO: constexpr if to prevent copy/cast if sizeof(cpVect) == sizeof(Vector2f)

				StackArray<Vector2f> nVertices = NazaraStackAllocation(Vector2f, vertexCount);
				for (int i = 0; i < vertexCount; ++i)
					nVertices[i].Set(float(vertices[i].x), float(vertices[i].y));

				drawOptions->polygonCallback(nVertices.data(), vertexCount, float(radius), CpDebugColorToColor(outlineColor), CpDebugColorToColor(fillColor), drawOptions->userdata);
			}
		}

		void DrawSegment(cpVect a, cpVect b, cpSpaceDebugColor color, cpDataPointer userdata)
		{
			auto drawOptions = static_cast<PhysWorld2D::DebugDrawOptions*>(userdata);
			if (drawOptions->segmentCallback)
				drawOptions->segmentCallback(Vector2f(float(a.x), float(a.y)), Vector2f(float(b.x), float(b.y)), CpDebugColorToColor(color), drawOptions->userdata);
		}

		void DrawThickSegment(cpVect a, cpVect b, cpFloat radius, cpSpaceDebugColor outlineColor, cpSpaceDebugColor fillColor, cpDataPointer userdata)
		{
			auto drawOptions = static_cast<PhysWorld2D::DebugDrawOptions*>(userdata);
			if (drawOptions->thickSegmentCallback)
				drawOptions->thickSegmentCallback(Vector2f(float(a.x), float(a.y)), Vector2f(float(b.x), float(b.y)), float(radius), CpDebugColorToColor(outlineColor), CpDebugColorToColor(fillColor), drawOptions->userdata);
		}

		cpSpaceDebugColor GetColorForShape(cpShape* shape, cpDataPointer userdata)
		{
			auto drawOptions = static_cast<PhysWorld2D::DebugDrawOptions*>(userdata);
			if (drawOptions->colorCallback)
			{
				RigidBody2D& rigidBody = *static_cast<RigidBody2D*>(cpShapeGetUserData(shape));
				return ColorToCpDebugColor(drawOptions->colorCallback(rigidBody, rigidBody.GetShapeIndex(shape), drawOptions->userdata));
			}
			else
				return cpSpaceDebugColor{255.f, 0.f, 0.f, 255.f};
		}
	}

	PhysWorld2D::PhysWorld2D() :
	m_stepSize(0.005f),
	m_timestepAccumulator(0.f)
	{
		m_handle = cpSpaceNew();
		cpSpaceSetUserData(m_handle, this);
	}

	PhysWorld2D::~PhysWorld2D()
	{
		cpSpaceFree(m_handle);
	}

	void PhysWorld2D::DebugDraw(const DebugDrawOptions& options, bool drawShapes, bool drawConstraints, bool drawCollisions)
	{
		auto ColorToCpDebugColor = [](Color c) -> cpSpaceDebugColor
		{
			return cpSpaceDebugColor{ c.r / 255.f, c.g / 255.f, c.b / 255.f, c.a / 255.f };
		};

		cpSpaceDebugDrawOptions drawOptions;
		drawOptions.collisionPointColor = ColorToCpDebugColor(options.collisionPointColor);
		drawOptions.constraintColor = ColorToCpDebugColor(options.constraintColor);
		drawOptions.shapeOutlineColor = ColorToCpDebugColor(options.shapeOutlineColor);
		drawOptions.data = const_cast<DebugDrawOptions*>(&options); //< Yeah, I know, shame :bell: but it won't be used for writing anyway

		std::underlying_type_t<cpSpaceDebugDrawFlags> drawFlags = 0;
		if (drawCollisions)
			drawFlags |= CP_SPACE_DEBUG_DRAW_COLLISION_POINTS;

		if (drawConstraints)
			drawFlags |= CP_SPACE_DEBUG_DRAW_CONSTRAINTS;

		if (drawShapes)
			drawFlags |= CP_SPACE_DEBUG_DRAW_SHAPES;

		drawOptions.flags = static_cast<cpSpaceDebugDrawFlags>(drawFlags);

		// Callback trampoline
		drawOptions.colorForShape = GetColorForShape;
		drawOptions.drawCircle = DrawCircle;
		drawOptions.drawDot = DrawDot;
		drawOptions.drawFatSegment = DrawThickSegment;
		drawOptions.drawPolygon = DrawPolygon;
		drawOptions.drawSegment = DrawSegment;

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

	float PhysWorld2D::GetStepSize() const
	{
		return m_stepSize;
	}

	bool PhysWorld2D::NearestBodyQuery(const Vector2f & from, float maxDistance, Nz::UInt32 collisionGroup, Nz::UInt32 categoryMask, Nz::UInt32 collisionMask, RigidBody2D** nearestBody)
	{
		cpShapeFilter filter = cpShapeFilterNew(collisionGroup, categoryMask, collisionMask);

		if (cpShape* shape = cpSpacePointQueryNearest(m_handle, {from.x, from.y}, maxDistance, filter, nullptr))
		{
			if (nearestBody)
				*nearestBody = static_cast<Nz::RigidBody2D*>(cpShapeGetUserData(shape));

			return true;
		}
		else
			return false;
	}

	bool PhysWorld2D::NearestBodyQuery(const Vector2f& from, float maxDistance, Nz::UInt32 collisionGroup, Nz::UInt32 categoryMask, Nz::UInt32 collisionMask, NearestQueryResult* result)
	{
		cpShapeFilter filter = cpShapeFilterNew(collisionGroup, categoryMask, collisionMask);

		if (result)
		{
			cpPointQueryInfo queryInfo;

			if (cpSpacePointQueryNearest(m_handle, { from.x, from.y }, maxDistance, filter, &queryInfo))
			{
				result->closestPoint.Set(Nz::Vector2<cpFloat>(queryInfo.point.x, queryInfo.point.y));
				result->distance = float(queryInfo.distance);
				result->fraction.Set(Nz::Vector2<cpFloat>(queryInfo.gradient.x, queryInfo.gradient.y));
				result->nearestBody = static_cast<Nz::RigidBody2D*>(cpShapeGetUserData(queryInfo.shape));

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

	bool PhysWorld2D::RaycastQuery(const Nz::Vector2f& from, const Nz::Vector2f& to, float radius, Nz::UInt32 collisionGroup, Nz::UInt32 categoryMask, Nz::UInt32 collisionMask, std::vector<RaycastHit>* hitInfos)
	{
		using ResultType = decltype(hitInfos);

		auto callback = [](cpShape* shape, cpVect point, cpVect normal, cpFloat alpha, void* data)
		{
			ResultType results = static_cast<ResultType>(data);

			RaycastHit hitInfo;
			hitInfo.fraction = float(alpha);
			hitInfo.hitNormal.Set(Nz::Vector2<cpFloat>(normal.x, normal.y));
			hitInfo.hitPos.Set(Nz::Vector2<cpFloat>(point.x, point.y));
			hitInfo.nearestBody = static_cast<Nz::RigidBody2D*>(cpShapeGetUserData(shape));

			results->emplace_back(std::move(hitInfo));
		};

		cpShapeFilter filter = cpShapeFilterNew(collisionGroup, categoryMask, collisionMask);

		std::size_t previousSize = hitInfos->size();
		cpSpaceSegmentQuery(m_handle, { from.x, from.y }, { to.x, to.y }, radius, filter, callback, hitInfos);

		return hitInfos->size() != previousSize;
	}

	bool PhysWorld2D::RaycastQueryFirst(const Nz::Vector2f& from, const Nz::Vector2f& to, float radius, Nz::UInt32 collisionGroup, Nz::UInt32 categoryMask, Nz::UInt32 collisionMask, RaycastHit* hitInfo)
	{
		cpShapeFilter filter = cpShapeFilterNew(collisionGroup, categoryMask, collisionMask);

		if (hitInfo)
		{
			cpSegmentQueryInfo queryInfo;

			if (cpSpaceSegmentQueryFirst(m_handle, { from.x, from.y }, { to.x, to.y }, radius, filter, &queryInfo))
			{
				hitInfo->fraction = float(queryInfo.alpha);
				hitInfo->hitNormal.Set(Nz::Vector2<cpFloat>(queryInfo.normal.x, queryInfo.normal.y));
				hitInfo->hitPos.Set(Nz::Vector2<cpFloat>(queryInfo.point.x, queryInfo.point.y));
				hitInfo->nearestBody = static_cast<Nz::RigidBody2D*>(cpShapeGetUserData(queryInfo.shape));

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

	void PhysWorld2D::RegionQuery(const Nz::Rectf& boundingBox, Nz::UInt32 collisionGroup, Nz::UInt32 categoryMask, Nz::UInt32 collisionMask, std::vector<Nz::RigidBody2D*>* bodies)
	{
		using ResultType = decltype(bodies);

		auto callback = [] (cpShape* shape, void* data)
		{
			ResultType results = static_cast<ResultType>(data);
			results->push_back(static_cast<Nz::RigidBody2D*>(cpShapeGetUserData(shape)));
		};

		cpShapeFilter filter = cpShapeFilterNew(collisionGroup, categoryMask, collisionMask);
		cpSpaceBBQuery(m_handle, cpBBNew(boundingBox.x, boundingBox.y, boundingBox.x + boundingBox.width, boundingBox.y + boundingBox.height), filter, callback, bodies);
	}

	void PhysWorld2D::RegisterCallbacks(unsigned int collisionId, const Callback& callbacks)
	{
		InitCallbacks(cpSpaceAddWildcardHandler(m_handle, collisionId), callbacks);
	}

	void PhysWorld2D::RegisterCallbacks(unsigned int collisionIdA, unsigned int collisionIdB, const Callback& callbacks)
	{
		InitCallbacks(cpSpaceAddCollisionHandler(m_handle, collisionIdA, collisionIdB), callbacks);
	}

	void PhysWorld2D::SetDamping(float dampingValue)
	{
		cpSpaceSetDamping(m_handle, dampingValue);
	}

	void PhysWorld2D::SetGravity(const Vector2f& gravity)
	{
		cpSpaceSetGravity(m_handle, cpv(gravity.x, gravity.y));
	}

	void PhysWorld2D::SetStepSize(float stepSize)
	{
		m_stepSize = stepSize;
	}

	void PhysWorld2D::Step(float timestep)
	{
		m_timestepAccumulator += timestep;

		while (m_timestepAccumulator >= m_stepSize)
		{
			OnPhysWorld2DPreStep(this);

			cpSpaceStep(m_handle, m_stepSize);

			OnPhysWorld2DPostStep(this);
			if (!m_rigidPostSteps.empty())
			{
				for (const auto& pair : m_rigidPostSteps)
				{
					for (const auto& step : pair.second.funcs)
						step(pair.first);
				}

				m_rigidPostSteps.clear();
			}

			m_timestepAccumulator -= m_stepSize;
		}
	}

	void PhysWorld2D::InitCallbacks(cpCollisionHandler* handler, const Callback& callbacks)
	{
		auto it = m_callbacks.emplace(handler, std::make_unique<Callback>(callbacks)).first;

		handler->userData = it->second.get();

		if (callbacks.startCallback)
		{
			handler->beginFunc = [](cpArbiter* arb, cpSpace* space, void *data) -> cpBool
			{
				cpBody* firstBody;
				cpBody* secondBody;
				cpArbiterGetBodies(arb, &firstBody, &secondBody);

				PhysWorld2D* world = static_cast<PhysWorld2D*>(cpSpaceGetUserData(space));
				RigidBody2D* firstRigidBody = static_cast<RigidBody2D*>(cpBodyGetUserData(firstBody));
				RigidBody2D* secondRigidBody = static_cast<RigidBody2D*>(cpBodyGetUserData(secondBody));

				const Callback* customCallbacks = static_cast<const Callback*>(data);
				if (customCallbacks->startCallback(*world, *firstRigidBody, *secondRigidBody, customCallbacks->userdata))
				{
					cpBool retA = cpArbiterCallWildcardBeginA(arb, space);
					cpBool retB = cpArbiterCallWildcardBeginB(arb, space);
					return retA && retB;
				}
				else
					return cpFalse;
			};
		}

		if (callbacks.endCallback)
		{
			handler->separateFunc = [](cpArbiter* arb, cpSpace* space, void *data)
			{
				cpBody* firstBody;
				cpBody* secondBody;
				cpArbiterGetBodies(arb, &firstBody, &secondBody);

				PhysWorld2D* world = static_cast<PhysWorld2D*>(cpSpaceGetUserData(space));
				RigidBody2D* firstRigidBody = static_cast<RigidBody2D*>(cpBodyGetUserData(firstBody));
				RigidBody2D* secondRigidBody = static_cast<RigidBody2D*>(cpBodyGetUserData(secondBody));

				const Callback* customCallbacks = static_cast<const Callback*>(data);
				customCallbacks->endCallback(*world, *firstRigidBody, *secondRigidBody, customCallbacks->userdata);

				cpArbiterCallWildcardSeparateA(arb, space);
				cpArbiterCallWildcardSeparateB(arb, space);
			};
		}

		if (callbacks.preSolveCallback)
		{
			handler->preSolveFunc = [](cpArbiter* arb, cpSpace* space, void *data) -> cpBool
			{
				cpBody* firstBody;
				cpBody* secondBody;
				cpArbiterGetBodies(arb, &firstBody, &secondBody);

				PhysWorld2D* world = static_cast<PhysWorld2D*>(cpSpaceGetUserData(space));
				RigidBody2D* firstRigidBody = static_cast<RigidBody2D*>(cpBodyGetUserData(firstBody));
				RigidBody2D* secondRigidBody = static_cast<RigidBody2D*>(cpBodyGetUserData(secondBody));

				const Callback* customCallbacks = static_cast<const Callback*>(data);
				if (customCallbacks->preSolveCallback(*world, *firstRigidBody, *secondRigidBody, customCallbacks->userdata))
				{
					cpBool retA = cpArbiterCallWildcardPreSolveA(arb, space);
					cpBool retB = cpArbiterCallWildcardPreSolveB(arb, space);
					return retA && retB;
				}
				else
					return cpFalse;
			};
		}

		if (callbacks.postSolveCallback)
		{
			handler->postSolveFunc = [](cpArbiter* arb, cpSpace* space, void *data)
			{
				cpBody* firstBody;
				cpBody* secondBody;
				cpArbiterGetBodies(arb, &firstBody, &secondBody);

				PhysWorld2D* world = static_cast<PhysWorld2D*>(cpSpaceGetUserData(space));
				RigidBody2D* firstRigidBody = static_cast<RigidBody2D*>(cpBodyGetUserData(firstBody));
				RigidBody2D* secondRigidBody = static_cast<RigidBody2D*>(cpBodyGetUserData(secondBody));

				const Callback* customCallbacks = static_cast<const Callback*>(data);
				customCallbacks->postSolveCallback(*world, *firstRigidBody, *secondRigidBody, customCallbacks->userdata);

				cpArbiterCallWildcardPostSolveA(arb, space);
				cpArbiterCallWildcardPostSolveB(arb, space);
			};
		}
	}

	void PhysWorld2D::OnRigidBodyMoved(RigidBody2D* oldPointer, RigidBody2D* newPointer)
	{
		auto it = m_rigidPostSteps.find(oldPointer);
		if (it == m_rigidPostSteps.end())
			return; //< Shouldn't happen

		m_rigidPostSteps.emplace(std::make_pair(newPointer, std::move(it->second)));
		m_rigidPostSteps.erase(oldPointer);
	}

	void PhysWorld2D::OnRigidBodyRelease(RigidBody2D* rigidBody)
	{
		m_rigidPostSteps.erase(rigidBody);
	}

	void PhysWorld2D::RegisterPostStep(RigidBody2D* rigidBody, PostStep&& func)
	{
		// If space isn't locked, no need to wait
		if (!cpSpaceIsLocked(m_handle))
		{
			func(rigidBody);
			return;
		}

		auto it = m_rigidPostSteps.find(rigidBody);
		if (it == m_rigidPostSteps.end())
		{
			PostStepContainer postStep;
			postStep.onMovedSlot.Connect(rigidBody->OnRigidBody2DMove, this, &PhysWorld2D::OnRigidBodyMoved);
			postStep.onReleaseSlot.Connect(rigidBody->OnRigidBody2DRelease, this, &PhysWorld2D::OnRigidBodyRelease);

			it = m_rigidPostSteps.insert(std::make_pair(rigidBody, std::move(postStep))).first;
		}

		it->second.funcs.emplace_back(std::move(func));
	}
}
