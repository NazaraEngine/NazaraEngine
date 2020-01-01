// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics2D/Collider2D.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Physics2D/RigidBody2D.hpp>
#include <Nazara/Physics2D/PhysWorld2D.hpp>
#include <chipmunk/chipmunk.h>
#include <chipmunk/chipmunk_structs.h>
#include <array>
#include <Nazara/Physics2D/Debug.hpp>

namespace Nz
{
	Collider2D::~Collider2D() = default;

	void Collider2D::ForEachPolygon(const std::function<void(const Vector2f* vertices, std::size_t vertexCount)>& callback) const
	{
		// Currently, the only way to get only the polygons of a shape is to create a temporary cpSpace containing only this shape
		// A better way to do this would be to reimplement this function in every subclass type in the very same way chipmunk does

		PhysWorld2D physWorld;
		RigidBody2D rigidBody(&physWorld, 0.f);

		std::vector<cpShape*> shapeVector;
		rigidBody.SetGeom(const_cast<Collider2D*>(this), false, false); //< Won't be used for writing, but still ugly

		PhysWorld2D::DebugDrawOptions drawCallbacks;
		drawCallbacks.circleCallback = [&](const Vector2f& origin, const RadianAnglef& /*rotation*/, float radius, Nz::Color /*outlineColor*/, Nz::Color /*fillColor*/, void* /*userData*/)
		{
			constexpr std::size_t circleVerticesCount = 20;

			std::array<Vector2f, circleVerticesCount> vertices;

			RadianAnglef angleBetweenVertices = 2.f * float(M_PI) / vertices.size();
			for (std::size_t i = 0; i < vertices.size(); ++i)
			{
				RadianAnglef angle = float(i) * angleBetweenVertices;
				std::pair<float, float> sincos = angle.GetSinCos();

				vertices[i] = origin + Vector2f(radius * sincos.first, radius * sincos.second);
			}

			callback(vertices.data(), vertices.size());
		};

		drawCallbacks.polygonCallback = [&](const Vector2f* vertices, std::size_t vertexCount, float radius, Nz::Color /*outlineColor*/, Nz::Color /*fillColor*/, void* /*userData*/)
		{
			//TODO: Handle radius
			callback(vertices, vertexCount);
		};

		drawCallbacks.segmentCallback = [&](const Vector2f& first, const Vector2f& second, Nz::Color /*color*/, void* /*userData*/)
		{
			std::array<Vector2f, 2> vertices = { first, second };

			callback(vertices.data(), vertices.size());
		};

		drawCallbacks.thickSegmentCallback = [&](const Vector2f& first, const Vector2f& second, float thickness, Nz::Color /*outlineColor*/, Nz::Color /*fillColor*/, void* /*userData*/)
		{
			static std::pair<float, float> sincos = Nz::DegreeAnglef(90.f).GetSinCos();

			Vector2f normal = Vector2f::Normalize(second - first);
			Vector2f thicknessNormal(sincos.second * normal.x - sincos.first * normal.y,
			                         sincos.first * normal.x + sincos.second * normal.y);

			std::array<Vector2f, 4> vertices;
			vertices[0] = first + thickness * thicknessNormal;
			vertices[1] = first - thickness * thicknessNormal;
			vertices[2] = second - thickness * thicknessNormal;
			vertices[3] = second + thickness * thicknessNormal;

			callback(vertices.data(), vertices.size());
		};

		physWorld.DebugDraw(drawCallbacks, true, false, false);
	}

	std::size_t Collider2D::GenerateShapes(RigidBody2D* body, std::vector<cpShape*>* shapes) const
	{
		std::size_t shapeCount = CreateShapes(body, shapes);

		cpShapeFilter filter = cpShapeFilterNew(m_collisionGroup, m_categoryMask, m_collisionMask);
		for (std::size_t i = shapes->size() - shapeCount; i < shapes->size(); ++i)
		{
			cpShape* shape = (*shapes)[i];

			cpShapeSetCollisionType(shape, m_collisionId);
			cpShapeSetElasticity(shape, cpFloat(m_elasticity));
			cpShapeSetFilter(shape, filter);
			cpShapeSetFriction(shape, cpFloat(m_friction));
			cpShapeSetSensor(shape, (m_trigger) ? cpTrue : cpFalse);
			cpShapeSetSurfaceVelocity(shape, cpv(cpFloat(m_surfaceVelocity.x), cpFloat(m_surfaceVelocity.y)));
		}

		return shapeCount;
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

	Nz::Vector2f BoxCollider2D::ComputeCenterOfMass() const
	{
		return m_rect.GetCenter();
	}

	float BoxCollider2D::ComputeMomentOfInertia(float mass) const
	{
		return static_cast<float>(cpMomentForBox2(mass, cpBBNew(m_rect.x, m_rect.y, m_rect.x + m_rect.width, m_rect.y + m_rect.height)));
	}

	ColliderType2D BoxCollider2D::GetType() const
	{
		return ColliderType2D_Box;
	}

	std::size_t BoxCollider2D::CreateShapes(RigidBody2D* body, std::vector<cpShape*>* shapes) const
	{
		shapes->push_back(cpBoxShapeNew2(body->GetHandle(), cpBBNew(m_rect.x, m_rect.y, m_rect.x + m_rect.width, m_rect.y + m_rect.height), m_radius));
		return 1;
	}

	/******************************** CircleCollider2D *********************************/

	CircleCollider2D::CircleCollider2D(float radius, const Vector2f& offset) :
	m_offset(offset),
	m_radius(radius)
	{
	}

	Nz::Vector2f CircleCollider2D::ComputeCenterOfMass() const
	{
		return m_offset;
	}

	float CircleCollider2D::ComputeMomentOfInertia(float mass) const
	{
		return static_cast<float>(cpMomentForCircle(mass, 0.f, m_radius, cpv(m_offset.x, m_offset.y)));
	}

	ColliderType2D CircleCollider2D::GetType() const
	{
		return ColliderType2D_Circle;
	}

	std::size_t CircleCollider2D::CreateShapes(RigidBody2D* body, std::vector<cpShape*>* shapes) const
	{
		shapes->push_back(cpCircleShapeNew(body->GetHandle(), m_radius, cpv(m_offset.x, m_offset.y)));
		return 1;
	}

	/******************************** CompoundCollider2D *********************************/

	CompoundCollider2D::CompoundCollider2D(std::vector<Collider2DRef> geoms) :
	m_geoms(std::move(geoms)),
	m_doesOverrideCollisionProperties(true)
	{
	}

	Nz::Vector2f CompoundCollider2D::ComputeCenterOfMass() const
	{
		Nz::Vector2f centerOfMass = Nz::Vector2f::Zero();
		for (const auto& geom : m_geoms)
			centerOfMass += geom->ComputeCenterOfMass();

		return centerOfMass / float(m_geoms.size());
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

	std::size_t CompoundCollider2D::CreateShapes(RigidBody2D* body, std::vector<cpShape*>* shapes) const
	{
		// Since C++ does not allow protected call from other objects, we have to be a friend of Collider2D, yay

		std::size_t shapeCount = 0;
		for (const auto& geom : m_geoms)
			shapeCount += geom->CreateShapes(body, shapes);

		return shapeCount;
	}

	std::size_t CompoundCollider2D::GenerateShapes(RigidBody2D* body, std::vector<cpShape*>* shapes) const
	{
		// This is our parent's default behavior
		if (m_doesOverrideCollisionProperties)
			return Collider2D::GenerateShapes(body, shapes);
		else
		{
			std::size_t shapeCount = 0;
			for (const auto& geom : m_geoms)
				shapeCount += geom->GenerateShapes(body, shapes);

			return shapeCount;
		}
	}

	/******************************** ConvexCollider2D *********************************/

	ConvexCollider2D::ConvexCollider2D(SparsePtr<const Vector2f> vertices, std::size_t vertexCount, float radius) :
	m_radius(radius)
	{
		m_vertices.resize(vertexCount);
		for (std::size_t i = 0; i < vertexCount; ++i)
			m_vertices[i].Set(*vertices++);
	}

	Nz::Vector2f ConvexCollider2D::ComputeCenterOfMass() const
	{
		static_assert(sizeof(cpVect) == sizeof(Vector2d), "Chipmunk vector is not equivalent to Vector2d");

		cpVect center = cpCentroidForPoly(int(m_vertices.size()), reinterpret_cast<const cpVect*>(m_vertices.data()));

		return Nz::Vector2f(float(center.x), float(center.y));
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

	std::size_t ConvexCollider2D::CreateShapes(RigidBody2D* body, std::vector<cpShape*>* shapes) const
	{
		shapes->push_back(cpPolyShapeNew(body->GetHandle(), int(m_vertices.size()), reinterpret_cast<const cpVect*>(m_vertices.data()), cpTransformIdentity, m_radius));
		return 1;
	}

	/********************************* NullCollider2D **********************************/

	ColliderType2D NullCollider2D::GetType() const
	{
		return ColliderType2D_Null;
	}

	Nz::Vector2f NullCollider2D::ComputeCenterOfMass() const
	{
		return Nz::Vector2f::Zero();
	}

	float NullCollider2D::ComputeMomentOfInertia(float mass) const
	{
		return (mass > 0.f) ? 1.f : 0.f; //< Null inertia is only possible for static/kinematic objects
	}

	std::size_t NullCollider2D::CreateShapes(RigidBody2D* /*body*/, std::vector<cpShape*>* /*shapes*/) const
	{
		return 0;
	}

	/******************************** SegmentCollider2D *********************************/

	Nz::Vector2f SegmentCollider2D::ComputeCenterOfMass() const
	{
		return (m_first + m_second) / 2.f;
	}

	float SegmentCollider2D::ComputeMomentOfInertia(float mass) const
	{
		return static_cast<float>(cpMomentForSegment(mass, cpv(m_first.x, m_first.y), cpv(m_second.x, m_second.y), m_thickness));
	}

	ColliderType2D SegmentCollider2D::GetType() const
	{
		return ColliderType2D_Segment;
	}

	std::size_t SegmentCollider2D::CreateShapes(RigidBody2D* body, std::vector<cpShape*>* shapes) const
	{
		cpShape* segment = cpSegmentShapeNew(body->GetHandle(), cpv(m_first.x, m_first.y), cpv(m_second.x, m_second.y), m_thickness);
		cpSegmentShapeSetNeighbors(segment, cpv(m_firstNeighbor.x, m_firstNeighbor.y), cpv(m_secondNeighbor.x, m_secondNeighbor.y));

		shapes->push_back(segment);
		return 1;
	}
}
