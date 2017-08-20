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

		std::vector<cpShape*> shapes;
		CreateShapes(body, shapes);

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

	float BoxCollider2D::ComputeMomentOfInertia(float mass) const
	{
		return static_cast<float>(cpMomentForBox2(mass, cpBBNew(m_rect.x, m_rect.y, m_rect.x + m_rect.width, m_rect.y + m_rect.height)));
	}

	ColliderType2D BoxCollider2D::GetType() const
	{
		return ColliderType2D_Box;
	}

	void BoxCollider2D::CreateShapes(RigidBody2D* body, std::vector<cpShape*>& shapes) const
	{
		shapes.push_back(cpBoxShapeNew2(body->GetHandle(), cpBBNew(m_rect.x, m_rect.y, m_rect.x + m_rect.width, m_rect.y + m_rect.height), m_radius));
	}

	/******************************** CircleCollider2D *********************************/

	CircleCollider2D::CircleCollider2D(float radius, const Vector2f& offset) :
	m_offset(offset),
	m_radius(radius)
	{
	}

	float CircleCollider2D::ComputeMomentOfInertia(float mass) const
	{
		return static_cast<float>(cpMomentForCircle(mass, 0.f, m_radius, cpv(m_offset.x, m_offset.y)));
	}

	ColliderType2D CircleCollider2D::GetType() const
	{
		return ColliderType2D_Circle;
	}

	void CircleCollider2D::CreateShapes(RigidBody2D* body, std::vector<cpShape*>& shapes) const
	{
		shapes.push_back(cpCircleShapeNew(body->GetHandle(), m_radius, cpv(m_offset.x, m_offset.y)));
	}

	/******************************** CompoundCollider2D *********************************/

	CompoundCollider2D::CompoundCollider2D(std::vector<Collider2DRef> geoms) :
	m_geoms(std::move(geoms))
	{
	}

	float CompoundCollider2D::ComputeMomentOfInertia(float mass) const
	{
		///TODO: Correctly compute moment using parallel axis theorem:
		/// https://chipmunk-physics.net/forum/viewtopic.php?t=1056
		float momentOfInertia = 0.f;
		for (const auto& geom : m_geoms)
			momentOfInertia += geom->ComputeMomentOfInertia(mass); //< Eeeer

		return momentOfInertia;
	}

	ColliderType2D CompoundCollider2D::GetType() const
	{
		return ColliderType2D_Compound;
	}

	void CompoundCollider2D::CreateShapes(RigidBody2D* body, std::vector<cpShape*>& shapes) const
	{
		// Since C++ does not allow protected call from other objects, we have to be a friend of Collider2D, yay
		for (const auto& geom : m_geoms)
			geom->CreateShapes(body, shapes);
	}

	/******************************** ConvexCollider2D *********************************/

	ConvexCollider2D::ConvexCollider2D(SparsePtr<const Vector2f> vertices, std::size_t vertexCount, float radius) :
	m_radius(radius)
	{
		m_vertices.resize(vertexCount);
		for (std::size_t i = 0; i < vertexCount; ++i)
			m_vertices[i].Set(*vertices++);
	}

	float ConvexCollider2D::ComputeMomentOfInertia(float mass) const
	{
		static_assert(sizeof(cpVect) == sizeof(Vector2d), "Chipmunk vector is not equivalent to Vector2d");

		return static_cast<float>(cpMomentForPoly(mass, int(m_vertices.size()), reinterpret_cast<const cpVect*>(m_vertices.data()), cpv(0.0, 0.0), m_radius));
	}

	ColliderType2D ConvexCollider2D::GetType() const
	{
		return ColliderType2D_Convex;
	}

	void ConvexCollider2D::CreateShapes(RigidBody2D* body, std::vector<cpShape*>& shapes) const
	{
		shapes.push_back(cpPolyShapeNew(body->GetHandle(), int(m_vertices.size()), reinterpret_cast<const cpVect*>(m_vertices.data()), cpTransformIdentity, m_radius));
	}

	/********************************* NullCollider2D **********************************/

	ColliderType2D NullCollider2D::GetType() const
	{
		return ColliderType2D_Null;
	}

	float NullCollider2D::ComputeMomentOfInertia(float /*mass*/) const
	{
		return 0.f;
	}

	void NullCollider2D::CreateShapes(RigidBody2D* /*body*/, std::vector<cpShape*>& /*shapes*/) const
	{
	}

	/******************************** SegmentCollider2D *********************************/

	float SegmentCollider2D::ComputeMomentOfInertia(float mass) const
	{
		return static_cast<float>(cpMomentForSegment(mass, cpv(m_first.x, m_first.y), cpv(m_second.x, m_second.y), m_thickness));
	}

	ColliderType2D SegmentCollider2D::GetType() const
	{
		return ColliderType2D_Segment;
	}

	void SegmentCollider2D::CreateShapes(RigidBody2D* body, std::vector<cpShape*>& shapes) const
	{
		shapes.push_back(cpSegmentShapeNew(body->GetHandle(), cpv(m_first.x, m_first.y), cpv(m_second.x, m_second.y), m_thickness));
	}
}
