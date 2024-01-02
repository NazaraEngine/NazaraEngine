// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - ChipmunkPhysics2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/ChipmunkPhysics2D/ChipmunkArbiter2D.hpp>
#include <chipmunk/chipmunk.h>
#include <Nazara/ChipmunkPhysics2D/Debug.hpp>

namespace Nz
{
	float ChipmunkArbiter2D::ComputeTotalKinematicEnergy() const
	{
		return float(cpArbiterTotalKE(m_arbiter));
	}

	Vector2f ChipmunkArbiter2D::ComputeTotalImpulse() const
	{
		cpVect impulse = cpArbiterTotalImpulse(m_arbiter);
		return Vector2f(Vector2<cpFloat>(impulse.x, impulse.y));
	}

	std::pair<ChipmunkRigidBody2D*, ChipmunkRigidBody2D*> ChipmunkArbiter2D::GetBodies() const
	{
		std::pair<ChipmunkRigidBody2D*, ChipmunkRigidBody2D*> bodies;
		cpBody* firstBody;
		cpBody* secondBody;
		cpArbiterGetBodies(m_arbiter, &firstBody, &secondBody);

		bodies.first = static_cast<ChipmunkRigidBody2D*>(cpBodyGetUserData(firstBody));
		bodies.second = static_cast<ChipmunkRigidBody2D*>(cpBodyGetUserData(secondBody));

		return bodies;
	}

	std::size_t ChipmunkArbiter2D::GetContactCount() const
	{
		return cpArbiterGetCount(m_arbiter);
	}

	float ChipmunkArbiter2D::GetContactDepth(std::size_t i) const
	{
		return float(cpArbiterGetDepth(m_arbiter, int(i)));
	}

	Vector2f ChipmunkArbiter2D::GetContactPointA(std::size_t i) const
	{
		cpVect point = cpArbiterGetPointA(m_arbiter, int(i));
		return Vector2f(Vector2<cpFloat>(point.x, point.y));
	}

	Vector2f ChipmunkArbiter2D::GetContactPointB(std::size_t i) const
	{
		cpVect point = cpArbiterGetPointB(m_arbiter, int(i));
		return Vector2f(Vector2<cpFloat>(point.x, point.y));
	}

	float ChipmunkArbiter2D::GetElasticity() const
	{
		return float(cpArbiterGetRestitution(m_arbiter));
	}
	float ChipmunkArbiter2D::GetFriction() const
	{
		return float(cpArbiterGetFriction(m_arbiter));
	}

	Vector2f ChipmunkArbiter2D::GetNormal() const
	{
		cpVect normal = cpArbiterGetNormal(m_arbiter);
		return Vector2f(Vector2<cpFloat>(normal.x, normal.y));
	}

	Vector2f ChipmunkArbiter2D::GetSurfaceVelocity() const
	{
		cpVect velocity = cpArbiterGetNormal(m_arbiter);
		return Vector2f(Vector2<cpFloat>(velocity.x, velocity.y));
	}

	bool ChipmunkArbiter2D::IsFirstContact() const
	{
		return cpArbiterIsFirstContact(m_arbiter) == cpTrue;
	}

	bool ChipmunkArbiter2D::IsRemoval() const
	{
		return cpArbiterIsRemoval(m_arbiter) == cpTrue;
	}

	void ChipmunkArbiter2D::SetElasticity(float elasticity)
	{
		cpArbiterSetRestitution(m_arbiter, elasticity);
	}

	void ChipmunkArbiter2D::SetFriction(float friction)
	{
		cpArbiterSetFriction(m_arbiter, friction);
	}

	void ChipmunkArbiter2D::SetSurfaceVelocity(const Vector2f& surfaceVelocity)
	{
		cpArbiterSetSurfaceVelocity(m_arbiter, cpv(surfaceVelocity.x, surfaceVelocity.y));
	}
}
