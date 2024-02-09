// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics2D/PhysArbiter2D.hpp>
#include <chipmunk/chipmunk.h>
#include <Nazara/Physics2D/Debug.hpp>

namespace Nz
{
	float PhysArbiter2D::ComputeTotalKinematicEnergy() const
	{
		return float(cpArbiterTotalKE(m_arbiter));
	}

	Vector2f PhysArbiter2D::ComputeTotalImpulse() const
	{
		cpVect impulse = cpArbiterTotalImpulse(m_arbiter);
		return Vector2f(Vector2<cpFloat>(impulse.x, impulse.y));
	}

	std::pair<RigidBody2D*, RigidBody2D*> PhysArbiter2D::GetBodies() const
	{
		std::pair<RigidBody2D*, RigidBody2D*> bodies;
		cpBody* firstBody;
		cpBody* secondBody;
		cpArbiterGetBodies(m_arbiter, &firstBody, &secondBody);

		bodies.first = static_cast<RigidBody2D*>(cpBodyGetUserData(firstBody));
		bodies.second = static_cast<RigidBody2D*>(cpBodyGetUserData(secondBody));

		return bodies;
	}

	std::size_t PhysArbiter2D::GetContactCount() const
	{
		return cpArbiterGetCount(m_arbiter);
	}

	float PhysArbiter2D::GetContactDepth(std::size_t i) const
	{
		return float(cpArbiterGetDepth(m_arbiter, int(i)));
	}

	Vector2f PhysArbiter2D::GetContactPointA(std::size_t i) const
	{
		cpVect point = cpArbiterGetPointA(m_arbiter, int(i));
		return Vector2f(Vector2<cpFloat>(point.x, point.y));
	}

	Vector2f PhysArbiter2D::GetContactPointB(std::size_t i) const
	{
		cpVect point = cpArbiterGetPointB(m_arbiter, int(i));
		return Vector2f(Vector2<cpFloat>(point.x, point.y));
	}

	float PhysArbiter2D::GetElasticity() const
	{
		return float(cpArbiterGetRestitution(m_arbiter));
	}
	float PhysArbiter2D::GetFriction() const
	{
		return float(cpArbiterGetFriction(m_arbiter));
	}

	Vector2f PhysArbiter2D::GetNormal() const
	{
		cpVect normal = cpArbiterGetNormal(m_arbiter);
		return Vector2f(Vector2<cpFloat>(normal.x, normal.y));
	}

	Vector2f PhysArbiter2D::GetSurfaceVelocity() const
	{
		cpVect velocity = cpArbiterGetNormal(m_arbiter);
		return Vector2f(Vector2<cpFloat>(velocity.x, velocity.y));
	}

	bool PhysArbiter2D::IsFirstContact() const
	{
		return cpArbiterIsFirstContact(m_arbiter) == cpTrue;
	}

	bool PhysArbiter2D::IsRemoval() const
	{
		return cpArbiterIsRemoval(m_arbiter) == cpTrue;
	}

	void PhysArbiter2D::SetElasticity(float elasticity)
	{
		cpArbiterSetRestitution(m_arbiter, elasticity);
	}

	void PhysArbiter2D::SetFriction(float friction)
	{
		cpArbiterSetFriction(m_arbiter, friction);
	}

	void PhysArbiter2D::SetSurfaceVelocity(const Vector2f& surfaceVelocity)
	{
		cpArbiterSetSurfaceVelocity(m_arbiter, cpv(surfaceVelocity.x, surfaceVelocity.y));
	}
}
