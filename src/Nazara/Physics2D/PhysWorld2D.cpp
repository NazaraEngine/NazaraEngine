// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics2D/PhysWorld2D.hpp>
#include <chipmunk/chipmunk.h>
#include <Nazara/Physics2D/Debug.hpp>

namespace Nz
{
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

	void PhysWorld2D::RegisterCallbacks(unsigned int collisionId, const Callback& callbacks)
	{
		InitCallbacks(cpSpaceAddWildcardHandler(m_handle, collisionId), callbacks);
	}

	void PhysWorld2D::RegisterCallbacks(unsigned int collisionIdA, unsigned int collisionIdB, const Callback& callbacks)
	{
		InitCallbacks(cpSpaceAddCollisionHandler(m_handle, collisionIdA, collisionIdB), callbacks);
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
			cpSpaceStep(m_handle, m_stepSize);
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
}
