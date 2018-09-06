// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics2D/Arbiter2D.hpp>
#include <chipmunk/chipmunk.h>
#include <Nazara/Physics3D/Debug.hpp>

namespace Nz
{
	float Nz::Arbiter2D::ComputeTotalKinematicEnergy() const
	{
		return float(cpArbiterTotalKE(m_arbiter));
	}

	Nz::Vector2f Arbiter2D::ComputeTotalImpulse() const
	{
		cpVect impulse = cpArbiterTotalImpulse(m_arbiter);
		return Nz::Vector2f(Nz::Vector2<cpFloat>(impulse.x, impulse.y));
	}

	std::size_t Arbiter2D::GetContactCount() const
	{
		return cpArbiterGetCount(m_arbiter);
	}

	float Arbiter2D::GetContactDepth(std::size_t i) const
	{
		return cpArbiterGetDepth(m_arbiter, int(i));
	}

	Nz::Vector2f Arbiter2D::GetContactPointA(std::size_t i) const
	{
		cpVect point = cpArbiterGetPointA(m_arbiter, int(i));
		return Nz::Vector2f(Nz::Vector2<cpFloat>(point.x, point.y));
	}

	Nz::Vector2f Arbiter2D::GetContactPointB(std::size_t i) const
	{
		cpVect point = cpArbiterGetPointB(m_arbiter, int(i));
		return Nz::Vector2f(Nz::Vector2<cpFloat>(point.x, point.y));
	}

	float Arbiter2D::GetElasticity() const
	{
		return float(cpArbiterGetRestitution(m_arbiter));
	}
	float Arbiter2D::GetFriction() const
	{
		return float(cpArbiterGetFriction(m_arbiter));
	}

	Nz::Vector2f Arbiter2D::GetNormal() const
	{
		cpVect normal = cpArbiterGetNormal(m_arbiter);
		return Nz::Vector2f(Nz::Vector2<cpFloat>(normal.x, normal.y));
	}

	Nz::Vector2f Arbiter2D::GetSurfaceVelocity() const
	{
		cpVect velocity = cpArbiterGetNormal(m_arbiter);
		return Nz::Vector2f(Nz::Vector2<cpFloat>(velocity.x, velocity.y));
	}

	bool Arbiter2D::IsFirstContact() const
	{
		return cpArbiterIsFirstContact(m_arbiter) == cpTrue;
	}

	bool Arbiter2D::IsRemoval() const
	{
		return cpArbiterIsRemoval(m_arbiter) == cpTrue;
	}

	void Arbiter2D::SetElasticity(float elasticity)
	{
		cpArbiterSetRestitution(m_arbiter, elasticity);
	}

	void Arbiter2D::SetFriction(float friction)
	{
		cpArbiterSetFriction(m_arbiter, friction);
	}

	void Arbiter2D::SetSurfaceVelocity(const Nz::Vector2f& surfaceVelocity)
	{
		cpArbiterSetSurfaceVelocity(m_arbiter, cpv(surfaceVelocity.x, surfaceVelocity.y));
	}
}
