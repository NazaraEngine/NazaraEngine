// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics3D/Collider3D.hpp>
#include <Nazara/Core/PrimitiveList.hpp>
#include <Nazara/Physics3D/BulletHelper.hpp>
#include <Nazara/Utility/Algorithm.hpp>
#include <Nazara/Utility/IndexBuffer.hpp>
#include <Nazara/Utility/SoftwareBuffer.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>
#include <NazaraUtils/MemoryHelper.hpp>
#include <tsl/ordered_map.h>
#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <BulletCollision/CollisionShapes/btCapsuleShape.h>
#include <BulletCollision/CollisionShapes/btCompoundShape.h>
#include <BulletCollision/CollisionShapes/btConeShape.h>
#include <BulletCollision/CollisionShapes/btConvexHullShape.h>
#include <BulletCollision/CollisionShapes/btCylinderShape.h>
#include <BulletCollision/CollisionShapes/btEmptyShape.h>
#include <BulletCollision/CollisionShapes/btSphereShape.h>
#include <BulletCollision/CollisionShapes/btStaticPlaneShape.h>
#include <Nazara/Physics3D/Debug.hpp>

namespace Nz
{
	Collider3D::~Collider3D() = default;

	Boxf Collider3D::ComputeAABB(const Vector3f& translation, const Quaternionf& rotation, const Vector3f& scale) const
	{
		return ComputeAABB(Matrix4f::Transform(translation, rotation), scale);
	}

	Boxf Collider3D::ComputeAABB(const Matrix4f& offsetMatrix, const Vector3f& scale) const
	{
		btTransform transform = ToBullet(offsetMatrix);

		btVector3 min, max;
		GetShape()->getAabb(transform, min, max);

		return Boxf(scale * FromBullet(min), scale * FromBullet(max));
	}

	void Collider3D::ComputeInertia(float mass, Vector3f* inertia) const
	{
		NazaraAssert(inertia, "invalid inertia pointer");

		btVector3 inertiaVec;
		GetShape()->calculateLocalInertia(mass, inertiaVec);

		*inertia = FromBullet(inertiaVec);
	}

	std::shared_ptr<StaticMesh> Collider3D::GenerateDebugMesh() const
	{
		std::vector<Vector3f> colliderVertices;
		std::vector<UInt16> colliderIndices;
		BuildDebugMesh(colliderVertices, colliderIndices);

		std::shared_ptr<VertexBuffer> colliderVB = std::make_shared<VertexBuffer>(VertexDeclaration::Get(VertexLayout::XYZ), SafeCast<UInt32>(colliderVertices.size()), BufferUsage::Write, SoftwareBufferFactory, colliderVertices.data());
		std::shared_ptr<IndexBuffer> colliderIB = std::make_shared<IndexBuffer>(IndexType::U16, colliderIndices.size(), BufferUsage::Write, SoftwareBufferFactory, colliderIndices.data());

		std::shared_ptr<StaticMesh> colliderSubMesh = std::make_shared<StaticMesh>(std::move(colliderVB), std::move(colliderIB));
		colliderSubMesh->GenerateAABB();
		colliderSubMesh->SetPrimitiveMode(PrimitiveMode::LineList);

		return colliderSubMesh;
	}

	std::shared_ptr<Collider3D> Collider3D::Build(const PrimitiveList& list)
	{
		std::size_t primitiveCount = list.GetSize();
		if (primitiveCount > 1)
		{
			std::vector<CompoundCollider3D::ChildCollider> childColliders(primitiveCount);

			for (unsigned int i = 0; i < primitiveCount; ++i)
			{
				const Primitive& primitive = list.GetPrimitive(i);
				childColliders[i].collider = CreateGeomFromPrimitive(primitive);
				childColliders[i].offsetMatrix = primitive.matrix;
			}

			return std::make_shared<CompoundCollider3D>(std::move(childColliders));
		}
		else if (primitiveCount > 0)
			return CreateGeomFromPrimitive(list.GetPrimitive(0));
		else
			return std::make_shared<NullCollider3D>();
	}
	
	std::shared_ptr<Collider3D> Collider3D::CreateGeomFromPrimitive(const Primitive& primitive)
	{
		switch (primitive.type)
		{
			case PrimitiveType::Box:
				return std::make_shared<BoxCollider3D>(primitive.box.lengths);

			case PrimitiveType::Cone:
				return std::make_shared<ConeCollider3D>(primitive.cone.length, primitive.cone.radius);

			case PrimitiveType::Plane:
				return std::make_shared<BoxCollider3D>(Vector3f(primitive.plane.size.x, 0.01f, primitive.plane.size.y));
				///TODO: PlaneGeom?

			case PrimitiveType::Sphere:
				return std::make_shared<SphereCollider3D>(primitive.sphere.size);
		}

		NazaraError("Primitive type not handled (0x" + NumberToString(UnderlyingCast(primitive.type), 16) + ')');
		return std::shared_ptr<Collider3D>();
	}

	/********************************** BoxCollider3D **********************************/

	BoxCollider3D::BoxCollider3D(const Vector3f& lengths) :
	m_lengths(lengths)
	{
		m_shape = std::make_unique<btBoxShape>(ToBullet(m_lengths * 0.5f));
	}

	BoxCollider3D::~BoxCollider3D() = default;

	void BoxCollider3D::BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const
	{
		auto InsertVertex = [&](float x, float y, float z) -> UInt16
		{
			UInt16 index = SafeCast<UInt16>(vertices.size());
			vertices.push_back(offsetMatrix * Vector3f(x, y, z));
			return index;
		};

		Vector3f halfLengths = m_lengths * 0.5f;

		UInt16 v0 = InsertVertex(-halfLengths.x, -halfLengths.y, -halfLengths.z);
		UInt16 v1 = InsertVertex(halfLengths.x, -halfLengths.y, -halfLengths.z);
		UInt16 v2 = InsertVertex(halfLengths.x, -halfLengths.y, halfLengths.z);
		UInt16 v3 = InsertVertex(-halfLengths.x, -halfLengths.y, halfLengths.z);

		UInt16 v4 = InsertVertex(-halfLengths.x, halfLengths.y, -halfLengths.z);
		UInt16 v5 = InsertVertex(halfLengths.x, halfLengths.y, -halfLengths.z);
		UInt16 v6 = InsertVertex(halfLengths.x, halfLengths.y, halfLengths.z);
		UInt16 v7 = InsertVertex(-halfLengths.x, halfLengths.y, halfLengths.z);

		auto InsertEdge = [&](UInt16 from, UInt16 to)
		{
			indices.push_back(from);
			indices.push_back(to);
		};
		InsertEdge(v0, v1);
		InsertEdge(v1, v2);
		InsertEdge(v2, v3);
		InsertEdge(v3, v0);

		InsertEdge(v4, v5);
		InsertEdge(v5, v6);
		InsertEdge(v6, v7);
		InsertEdge(v7, v4);

		InsertEdge(v0, v4);
		InsertEdge(v1, v5);
		InsertEdge(v2, v6);
		InsertEdge(v3, v7);
	}

	btCollisionShape* BoxCollider3D::GetShape() const
	{
		return m_shape.get();
	}

	Vector3f BoxCollider3D::GetLengths() const
	{
		return m_lengths;
	}

	ColliderType3D BoxCollider3D::GetType() const
	{
		return ColliderType3D::Box;
	}

	/******************************** CapsuleCollider3D ********************************/

	CapsuleCollider3D::CapsuleCollider3D(float length, float radius) :
	m_length(length),
	m_radius(radius)
	{
		m_shape = std::make_unique<btCapsuleShape>(radius, length);
	}

	CapsuleCollider3D::~CapsuleCollider3D() = default;

	void CapsuleCollider3D::BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const
	{
	}

	float CapsuleCollider3D::GetLength() const
	{
		return m_length;
	}

	float CapsuleCollider3D::GetRadius() const
	{
		return m_radius;
	}

	btCollisionShape* CapsuleCollider3D::GetShape() const
	{
		return m_shape.get();
	}

	ColliderType3D CapsuleCollider3D::GetType() const
	{
		return ColliderType3D::Capsule;
	}

	/******************************* CompoundCollider3D ********************************/

	CompoundCollider3D::CompoundCollider3D(std::vector<ChildCollider> childs) :
	m_childs(std::move(childs))
	{
		m_shape = std::make_unique<btCompoundShape>();
		for (const auto& child : m_childs)
		{
			btTransform transform = ToBullet(child.offsetMatrix);
			m_shape->addChildShape(transform, child.collider->GetShape());
		}
	}

	CompoundCollider3D::~CompoundCollider3D() = default;

	void CompoundCollider3D::BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const
	{
		for (const auto& child : m_childs)
			child.collider->BuildDebugMesh(vertices, indices, offsetMatrix * child.offsetMatrix);
	}

	auto CompoundCollider3D::GetGeoms() const -> const std::vector<ChildCollider>&
	{
		return m_childs;
	}

	btCollisionShape* CompoundCollider3D::GetShape() const
	{
		return m_shape.get();
	}

	ColliderType3D CompoundCollider3D::GetType() const
	{
		return ColliderType3D::Compound;
	}

	/********************************* ConeCollider3D **********************************/

	ConeCollider3D::ConeCollider3D(float length, float radius) :
	m_length(length),
	m_radius(radius)
	{
		m_shape = std::make_unique<btConeShape>(radius, length);
	}

	ConeCollider3D::~ConeCollider3D() = default;

	void ConeCollider3D::BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const
	{
	}

	float ConeCollider3D::GetLength() const
	{
		return m_length;
	}

	float ConeCollider3D::GetRadius() const
	{
		return m_radius;
	}

	btCollisionShape* ConeCollider3D::GetShape() const
	{
		return m_shape.get();
	}

	ColliderType3D ConeCollider3D::GetType() const
	{
		return ColliderType3D::Cone;
	}

	/****************************** ConvexCollider3D *******************************/

	ConvexCollider3D::ConvexCollider3D(SparsePtr<const Vector3f> vertices, unsigned int vertexCount)
	{
		static_assert(std::is_same_v<btScalar, float>);

		m_shape = std::make_unique<btConvexHullShape>(reinterpret_cast<const float*>(vertices.GetPtr()), vertexCount, vertices.GetStride());
		m_shape->optimizeConvexHull();
	}

	ConvexCollider3D::~ConvexCollider3D() = default;

	void ConvexCollider3D::BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const
	{
		tsl::ordered_map<Vector3f, UInt16> vertexCache;
		auto InsertVertex = [&](const Vector3f& position) -> UInt16
		{
			auto it = vertexCache.find(position);
			if (it != vertexCache.end())
				return it->second;

			UInt16 index = SafeCast<UInt16>(vertices.size());

			vertices.push_back(position);
			vertexCache.emplace(position, index);

			return index;
		};

		int edgeCount = m_shape->getNumEdges();
		for (int i = 0; i < edgeCount; ++i)
		{
			btVector3 from, to;
			m_shape->getEdge(i, from, to);

			indices.push_back(InsertVertex(offsetMatrix * FromBullet(from)));
			indices.push_back(InsertVertex(offsetMatrix * FromBullet(to)));
		}
	}

	btCollisionShape* ConvexCollider3D::GetShape() const
	{
		return m_shape.get();
	}

	ColliderType3D ConvexCollider3D::GetType() const
	{
		return ColliderType3D::ConvexHull;
	}

	/******************************* CylinderCollider3D ********************************/

	CylinderCollider3D::CylinderCollider3D(float length, float radius) :
	m_length(length),
	m_radius(radius)
	{
		// TODO: Allow to use two different radius
		m_shape = std::make_unique<btCylinderShape>(btVector3{ radius, length, radius });
	}

	CylinderCollider3D::~CylinderCollider3D() = default;

	void CylinderCollider3D::BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const
	{
	}

	float CylinderCollider3D::GetLength() const
	{
		return m_length;
	}

	float CylinderCollider3D::GetRadius() const
	{
		return m_radius;
	}

	btCollisionShape* CylinderCollider3D::GetShape() const
	{
		return m_shape.get();
	}

	ColliderType3D CylinderCollider3D::GetType() const
	{
		return ColliderType3D::Cylinder;
	}

	/********************************* NullCollider3D **********************************/

	NullCollider3D::NullCollider3D()
	{
		m_shape = std::make_unique<btEmptyShape>();
	}

	NullCollider3D::~NullCollider3D() = default;

	void NullCollider3D::BuildDebugMesh(std::vector<Vector3f>& /*vertices*/, std::vector<UInt16>& /*indices*/, const Matrix4f& /*offsetMatrix*/) const
	{
	}

	void NullCollider3D::ComputeInertia(float /*mass*/, Vector3f* inertia) const
	{
		inertia->Set(1.f, 1.f, 1.f);
	}

	btCollisionShape* NullCollider3D::GetShape() const
	{
		return m_shape.get();
	}

	ColliderType3D NullCollider3D::GetType() const
	{
		return ColliderType3D::Null;
	}

	/******************************** SphereCollider3D *********************************/

	SphereCollider3D::SphereCollider3D(float radius) :
	m_radius(radius)
	{
		m_shape = std::make_unique<btSphereShape>(radius);
	}

	SphereCollider3D::~SphereCollider3D() = default;

	void SphereCollider3D::BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const
	{
	}

	float SphereCollider3D::GetRadius() const
	{
		return m_radius;
	}

	btCollisionShape* SphereCollider3D::GetShape() const
	{
		return m_shape.get();
	}

	ColliderType3D SphereCollider3D::GetType() const
	{
		return ColliderType3D::Sphere;
	}

	/******************************** StaticPlaneCollider3D *********************************/

	StaticPlaneCollider3D::StaticPlaneCollider3D(const Planef& plane) :
	StaticPlaneCollider3D(plane.normal, plane.distance)
	{
	}

	StaticPlaneCollider3D::StaticPlaneCollider3D(const Vector3f& normal, float distance) :
	m_normal(normal),
	m_distance(distance)
	{
		m_shape = std::make_unique<btStaticPlaneShape>(ToBullet(m_normal), m_distance);
	}

	StaticPlaneCollider3D::~StaticPlaneCollider3D() = default;

	void StaticPlaneCollider3D::BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const
	{
	}

	float StaticPlaneCollider3D::GetDistance() const
	{
		return m_distance;
	}

	const Vector3f& StaticPlaneCollider3D::GetNormal() const
	{
		return m_normal;
	}

	btCollisionShape* StaticPlaneCollider3D::GetShape() const
	{
		return m_shape.get();
	}

	ColliderType3D StaticPlaneCollider3D::GetType() const
	{
		return ColliderType3D::StaticPlane;
	}
}
