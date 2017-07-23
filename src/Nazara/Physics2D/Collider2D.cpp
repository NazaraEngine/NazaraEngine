// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics2D/Collider2D.hpp>
#include <Nazara/Physics2D/RigidBody2D.hpp>
#include <chipmunk/chipmunk.h>
#include <Nazara/Physics3D/Debug.hpp>

namespace Nz
{
	Collider2D::~Collider2D() = default;

	std::vector<cpShape*> Collider2D::GenerateShapes(RigidBody2D* body) const
	{
		cpShapeFilter filter = cpShapeFilterNew(m_collisionGroup, m_categoryMask, m_collisionMask);

		std::vector<cpShape*> shapes = CreateShapes(body);
		for (cpShape* shape : shapes)
		{
			cpShapeSetFilter(shape, filter);
			cpShapeSetCollisionType(shape, m_collisionId);
			cpShapeSetSensor(shape, (m_trigger) ? cpTrue : cpFalse);
		}

		return shapes;
	}

	/******************************** BoxCollider2D *********************************/

	BoxCollider2D::BoxCollider2D(const Vector2f& size, float radius) :
	BoxCollider2D(Rectf(-size.x / 2.f, -size.y / 2.f, size.x, size.y), radius)
	{
	}

	BoxCollider2D::BoxCollider2D(const Rectf& rect, float radius) :
	m_rect(rect),
	m_radius(radius)
	{
	}

	float BoxCollider2D::ComputeInertialMatrix(float mass) const
	{
		return static_cast<float>(cpMomentForBox2(mass, cpBBNew(m_rect.x, m_rect.y, m_rect.x + m_rect.width, m_rect.y + m_rect.height)));
	}

	ColliderType2D BoxCollider2D::GetType() const
	{
		return ColliderType2D_Box;
	}

	std::vector<cpShape*> BoxCollider2D::CreateShapes(RigidBody2D* body) const
	{
		std::vector<cpShape*> shapes;
		shapes.push_back(cpBoxShapeNew2(body->GetHandle(), cpBBNew(m_rect.x, m_rect.y, m_rect.x + m_rect.width, m_rect.y + m_rect.height), m_radius));

		return shapes;
	}

	/******************************** CircleCollider2D *********************************/

	CircleCollider2D::CircleCollider2D(float radius, const Vector2f& offset) :
	m_offset(offset),
	m_radius(radius)
	{
	}

	float CircleCollider2D::ComputeInertialMatrix(float mass) const
	{
		return static_cast<float>(cpMomentForCircle(mass, 0.f, m_radius, cpv(m_offset.x, m_offset.y)));
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

	float NullCollider2D::ComputeInertialMatrix(float /*mass*/) const
	{
		return 0.f;
	}

	std::vector<cpShape*> NullCollider2D::CreateShapes(RigidBody2D* /*body*/) const
	{
		return std::vector<cpShape*>();
	}

	/******************************** SegmentCollider2D *********************************/

	float SegmentCollider2D::ComputeInertialMatrix(float mass) const
	{
		return static_cast<float>(cpMomentForSegment(mass, cpv(m_first.x, m_first.y), cpv(m_second.x, m_second.y), m_thickness));
	}

	ColliderType2D SegmentCollider2D::GetType() const
	{
		return ColliderType2D_Segment;
	}

	std::vector<cpShape*> SegmentCollider2D::CreateShapes(RigidBody2D* body) const
	{
		std::vector<cpShape*> shapes;
		shapes.push_back(cpSegmentShapeNew(body->GetHandle(), cpv(m_first.x, m_first.y), cpv(m_second.x, m_second.y), m_thickness));

		return shapes;
	}

}
