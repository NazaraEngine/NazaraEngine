// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - ChipmunkPhysics2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/ChipmunkPhysics2D/ChipmunkCollider2D.hpp>
#include <Nazara/ChipmunkPhysics2D/ChipmunkHelper.hpp>
#include <NazaraUtils/CallOnExit.hpp>
#include <NazaraUtils/StackArray.hpp>
#include <chipmunk/chipmunk.h>
#include <chipmunk/chipmunk_structs.h>
#include <array>
#include <Nazara/ChipmunkPhysics2D/Debug.hpp>

namespace Nz
{
	namespace
	{
		constexpr cpSpaceDebugColor s_chipmunkWhite = { 1.f, 1.f, 1.f, 1.f };
	}

	ChipmunkCollider2D::~ChipmunkCollider2D() = default;

	void ChipmunkCollider2D::ForEachPolygon(const FunctionRef<void(const Vector2f* vertices, std::size_t vertexCount)>& callback) const
	{
		// Currently, the only way to get only the polygons of a shape is to create a temporary cpSpace containing only this shape
		// A better way to do this would be to reimplement this function in every subclass type in the very same way chipmunk does

		cpSpace* space = cpSpaceNew();
		if (!space)
			throw std::runtime_error("failed to create chipmunk space");

		CallOnExit spaceRRID([&] { cpSpaceFree(space); });

		cpBody* body = cpSpaceGetStaticBody(space);

		std::vector<cpShape*> shapes;
		CreateShapes(body, &shapes);
		CallOnExit shapeRRID([&]
		{
			for (cpShape* shape : shapes)
				cpShapeDestroy(shape);
		});

		for (cpShape* shape : shapes)
			cpSpaceAddShape(space, shape);

		using CallbackType = std::decay_t<decltype(callback)>;

		cpSpaceDebugDrawOptions drawOptions;
		drawOptions.collisionPointColor = s_chipmunkWhite;
		drawOptions.constraintColor = s_chipmunkWhite;
		drawOptions.shapeOutlineColor = s_chipmunkWhite;
		drawOptions.data = const_cast<void*>(static_cast<const void*>(&callback));
		drawOptions.flags = CP_SPACE_DEBUG_DRAW_SHAPES;

		// Callback trampoline
		drawOptions.colorForShape = [](cpShape* /*shape*/, cpDataPointer /*userdata*/) { return s_chipmunkWhite; };
		drawOptions.drawCircle = [](cpVect pos, cpFloat /*angle*/, cpFloat radius, cpSpaceDebugColor /*outlineColor*/, cpSpaceDebugColor /*fillColor*/, cpDataPointer userdata)
		{
			const auto& callback = *static_cast<const CallbackType*>(userdata);

			constexpr std::size_t circleVerticesCount = 20;

			std::array<Vector2f, circleVerticesCount> vertices;

			Vector2f origin = FromChipmunk(pos);
			float r = SafeCast<float>(radius);

			RadianAnglef angleBetweenVertices = 2.f * Pi<float> / vertices.size();
			for (std::size_t i = 0; i < vertices.size(); ++i)
			{
				RadianAnglef angle = float(i) * angleBetweenVertices;
				std::pair<float, float> sincos = angle.GetSinCos();

				vertices[i] = origin + Vector2f(r * sincos.first, r * sincos.second);
			}

			callback(vertices.data(), vertices.size());
		};

		drawOptions.drawDot = [](cpFloat /*size*/, cpVect /*pos*/, cpSpaceDebugColor /*color*/, cpDataPointer /*userdata*/) {}; //< Dummy

		drawOptions.drawFatSegment = [](cpVect a, cpVect b, cpFloat radius, cpSpaceDebugColor /*outlineColor*/, cpSpaceDebugColor /*fillColor*/, cpDataPointer userdata)
		{
			const auto& callback = *static_cast<const CallbackType*>(userdata);

			static std::pair<float, float> sincos = DegreeAnglef(90.f).GetSinCos();

			Vector2f from = FromChipmunk(a);
			Vector2f to = FromChipmunk(b);

			Vector2f normal = Vector2f::Normalize(to - from);
			Vector2f thicknessNormal(sincos.second * normal.x - sincos.first * normal.y,
			                         sincos.first * normal.x + sincos.second * normal.y);

			float thickness = SafeCast<float>(radius);

			std::array<Vector2f, 4> vertices;
			vertices[0] = from + thickness * thicknessNormal;
			vertices[1] = from - thickness * thicknessNormal;
			vertices[2] = to - thickness * thicknessNormal;
			vertices[3] = to + thickness * thicknessNormal;

			callback(vertices.data(), vertices.size());
		};

		drawOptions.drawPolygon = [](int vertexCount, const cpVect* vertices, cpFloat /*radius*/, cpSpaceDebugColor /*outlineColor*/, cpSpaceDebugColor /*fillColor*/, cpDataPointer userdata)
		{
			const auto& callback = *static_cast<const CallbackType*>(userdata);

			StackArray<Vector2f> nVertices = NazaraStackArray(Vector2f, vertexCount);
			for (int i = 0; i < vertexCount; ++i)
				nVertices[i] = Vector2f(float(vertices[i].x), float(vertices[i].y));

			callback(nVertices.data(), nVertices.size());
		};

		drawOptions.drawSegment = [](cpVect a, cpVect b, cpSpaceDebugColor /*fillColor*/, cpDataPointer userdata)
		{
			const auto& callback = *static_cast<const CallbackType*>(userdata);

			std::array<Vector2f, 2> vertices = { FromChipmunk(a), FromChipmunk(b) };
			callback(vertices.data(), vertices.size());
		};

		cpSpaceDebugDraw(space, &drawOptions);
	}

	std::size_t ChipmunkCollider2D::GenerateShapes(cpBody* body, std::vector<cpShape*>* shapes) const
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

	ChipmunkBoxCollider2D::ChipmunkBoxCollider2D(const Vector2f& size, float radius) :
	ChipmunkBoxCollider2D(Rectf(-size.x / 2.f, -size.y / 2.f, size.x, size.y), radius)
	{
	}

	ChipmunkBoxCollider2D::ChipmunkBoxCollider2D(const Rectf& rect, float radius) :
	m_rect(rect),
	m_radius(radius)
	{
	}

	Vector2f ChipmunkBoxCollider2D::ComputeCenterOfMass() const
	{
		return m_rect.GetCenter();
	}

	float ChipmunkBoxCollider2D::ComputeMomentOfInertia(float mass) const
	{
		return SafeCast<float>(cpMomentForBox2(mass, cpBBNew(m_rect.x, m_rect.y, m_rect.x + m_rect.width, m_rect.y + m_rect.height)));
	}

	ChipmunkColliderType2D ChipmunkBoxCollider2D::GetType() const
	{
		return ChipmunkColliderType2D::Box;
	}

	std::size_t ChipmunkBoxCollider2D::CreateShapes(cpBody* body, std::vector<cpShape*>* shapes) const
	{
		shapes->push_back(cpBoxShapeNew2(body, cpBBNew(m_rect.x, m_rect.y, m_rect.x + m_rect.width, m_rect.y + m_rect.height), m_radius));
		return 1;
	}

	/******************************** CircleCollider2D *********************************/

	ChipmunkCircleCollider2D::ChipmunkCircleCollider2D(float radius, const Vector2f& offset) :
	m_offset(offset),
	m_radius(radius)
	{
	}

	Vector2f ChipmunkCircleCollider2D::ComputeCenterOfMass() const
	{
		return m_offset;
	}

	float ChipmunkCircleCollider2D::ComputeMomentOfInertia(float mass) const
	{
		return SafeCast<float>(cpMomentForCircle(mass, 0.f, m_radius, cpv(m_offset.x, m_offset.y)));
	}

	ChipmunkColliderType2D ChipmunkCircleCollider2D::GetType() const
	{
		return ChipmunkColliderType2D::Circle;
	}

	std::size_t ChipmunkCircleCollider2D::CreateShapes(cpBody* body, std::vector<cpShape*>* shapes) const
	{
		shapes->push_back(cpCircleShapeNew(body, m_radius, cpv(m_offset.x, m_offset.y)));
		return 1;
	}

	/******************************** CompoundCollider2D *********************************/

	ChipmunkCompoundCollider2D::ChipmunkCompoundCollider2D(std::vector<std::shared_ptr<ChipmunkCollider2D>> geoms) :
	m_geoms(std::move(geoms)),
	m_doesOverrideCollisionProperties(true)
	{
	}

	Vector2f ChipmunkCompoundCollider2D::ComputeCenterOfMass() const
	{
		Vector2f centerOfMass = Vector2f::Zero();
		for (const auto& geom : m_geoms)
			centerOfMass += geom->ComputeCenterOfMass();

		return centerOfMass / float(m_geoms.size());
	}

	float ChipmunkCompoundCollider2D::ComputeMomentOfInertia(float mass) const
	{
		///TODO: Correctly compute moment using parallel axis theorem:
		/// https://chipmunk-physics.net/forum/viewtopic.php?t=1056
		float momentOfInertia = 0.f;
		for (const auto& geom : m_geoms)
			momentOfInertia += geom->ComputeMomentOfInertia(mass); //< Eeeer

		return momentOfInertia;
	}

	ChipmunkColliderType2D ChipmunkCompoundCollider2D::GetType() const
	{
		return ChipmunkColliderType2D::Compound;
	}

	std::size_t ChipmunkCompoundCollider2D::CreateShapes(cpBody* body, std::vector<cpShape*>* shapes) const
	{
		// Since C++ does not allow protected call from other objects, we have to be a friend of Collider2D, yay

		std::size_t shapeCount = 0;
		for (const auto& geom : m_geoms)
			shapeCount += geom->CreateShapes(body, shapes);

		return shapeCount;
	}

	std::size_t ChipmunkCompoundCollider2D::GenerateShapes(cpBody* body, std::vector<cpShape*>* shapes) const
	{
		// This is our parent's default behavior
		if (m_doesOverrideCollisionProperties)
			return ChipmunkCollider2D::GenerateShapes(body, shapes);
		else
		{
			std::size_t shapeCount = 0;
			for (const auto& geom : m_geoms)
				shapeCount += geom->GenerateShapes(body, shapes);

			return shapeCount;
		}
	}

	/******************************** ConvexCollider2D *********************************/

	ChipmunkConvexCollider2D::ChipmunkConvexCollider2D(SparsePtr<const Vector2f> vertices, std::size_t vertexCount, float radius) :
	m_radius(radius)
	{
		m_vertices.resize(vertexCount);
		for (std::size_t i = 0; i < vertexCount; ++i)
			m_vertices[i] = Vector2<cpFloat>(*vertices++);
	}

	Vector2f ChipmunkConvexCollider2D::ComputeCenterOfMass() const
	{
		static_assert(sizeof(cpVect) == sizeof(Vector2d), "Chipmunk vector is not equivalent to Vector2d");

		cpVect center = cpCentroidForPoly(int(m_vertices.size()), reinterpret_cast<const cpVect*>(m_vertices.data()));

		return Vector2f(float(center.x), float(center.y));
	}

	float ChipmunkConvexCollider2D::ComputeMomentOfInertia(float mass) const
	{
		static_assert(sizeof(cpVect) == sizeof(Vector2d), "Chipmunk vector is not equivalent to Vector2d");

		return SafeCast<float>(cpMomentForPoly(mass, int(m_vertices.size()), reinterpret_cast<const cpVect*>(m_vertices.data()), cpv(0.0, 0.0), m_radius));
	}

	ChipmunkColliderType2D ChipmunkConvexCollider2D::GetType() const
	{
		return ChipmunkColliderType2D::Convex;
	}

	std::size_t ChipmunkConvexCollider2D::CreateShapes(cpBody* body, std::vector<cpShape*>* shapes) const
	{
		shapes->push_back(cpPolyShapeNew(body, int(m_vertices.size()), reinterpret_cast<const cpVect*>(m_vertices.data()), cpTransformIdentity, m_radius));
		return 1;
	}

	/********************************* NullCollider2D **********************************/

	ChipmunkColliderType2D ChipmunkNullCollider2D::GetType() const
	{
		return ChipmunkColliderType2D::Null;
	}

	Vector2f ChipmunkNullCollider2D::ComputeCenterOfMass() const
	{
		return Vector2f::Zero();
	}

	float ChipmunkNullCollider2D::ComputeMomentOfInertia(float mass) const
	{
		return (mass > 0.f) ? 1.f : 0.f; //< Null inertia is only possible for static/kinematic objects
	}

	std::size_t ChipmunkNullCollider2D::CreateShapes(cpBody* /*body*/, std::vector<cpShape*>* /*shapes*/) const
	{
		return 0;
	}

	/******************************** SegmentCollider2D *********************************/

	Vector2f ChipmunkSegmentCollider2D::ComputeCenterOfMass() const
	{
		return (m_first + m_second) / 2.f;
	}

	float ChipmunkSegmentCollider2D::ComputeMomentOfInertia(float mass) const
	{
		return SafeCast<float>(cpMomentForSegment(mass, cpv(m_first.x, m_first.y), cpv(m_second.x, m_second.y), m_thickness));
	}

	ChipmunkColliderType2D ChipmunkSegmentCollider2D::GetType() const
	{
		return ChipmunkColliderType2D::Segment;
	}

	std::size_t ChipmunkSegmentCollider2D::CreateShapes(cpBody* body, std::vector<cpShape*>* shapes) const
	{
		cpShape* segment = cpSegmentShapeNew(body, cpv(m_first.x, m_first.y), cpv(m_second.x, m_second.y), m_thickness);
		cpSegmentShapeSetNeighbors(segment, cpv(m_firstNeighbor.x, m_firstNeighbor.y), cpv(m_secondNeighbor.x, m_secondNeighbor.y));

		shapes->push_back(segment);
		return 1;
	}
}
