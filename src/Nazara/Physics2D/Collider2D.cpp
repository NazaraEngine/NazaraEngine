// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics2D/Collider2D.hpp>
#include <Nazara/Physics2D/RigidBody2D.hpp>
#include <chipmunk/chipmunk.h>
#include <Nazara/Physics3D/Debug.hpp>

namespace Nz
{
	Collider2D::~Collider2D() = default;

	/******************************** CircleCollider2D *********************************/

	CircleCollider2D::CircleCollider2D(float radius, const Vector2f& offset) :
	m_offset(offset),
	m_radius(radius)
	{
	}

	float CircleCollider2D::ComputeInertialMatrix(float mass) const
	{
		return cpMomentForCircle(mass, 0.f, m_radius, cpv(m_offset.x, m_offset.y));
	}

	ColliderType2D CircleCollider2D::GetType() const
	{
		return ColliderType2D_Circle;
	}

	std::vector<cpShape*> CircleCollider2D::CreateShapes(RigidBody2D* body) const
	{
		std::vector<cpShape*> shapes;
		shapes.push_back(cpCircleShapeNew(body->GetHandle(), m_radius, cpv(m_offset.x, m_offset.y)));

		return shapes;
	}

	/********************************* NullCollider2D **********************************/

	ColliderType2D NullCollider2D::GetType() const
	{
		return ColliderType2D_Null;
	}

	float NullCollider2D::ComputeInertialMatrix(float mass) const
	{
		return 0.f;
	}

	std::vector<cpShape*> NullCollider2D::CreateShapes(RigidBody2D* body) const
	{
		return std::vector<cpShape*>();
	}
}
