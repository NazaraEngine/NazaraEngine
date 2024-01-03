// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - BulletPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/BulletPhysics3D/BulletCollider3D.hpp>
#include <Nazara/BulletPhysics3D/BulletHelper.hpp>
#include <Nazara/Core/PrimitiveList.hpp>
#include <Nazara/Utility/Algorithm.hpp>
#include <Nazara/Utility/IndexBuffer.hpp>
#include <Nazara/Utility/SoftwareBuffer.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>
#include <NazaraUtils/MemoryHelper.hpp>
#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <BulletCollision/CollisionShapes/btCapsuleShape.h>
#include <BulletCollision/CollisionShapes/btCompoundShape.h>
#include <BulletCollision/CollisionShapes/btConeShape.h>
#include <BulletCollision/CollisionShapes/btConvexHullShape.h>
#include <BulletCollision/CollisionShapes/btCylinderShape.h>
#include <BulletCollision/CollisionShapes/btEmptyShape.h>
#include <BulletCollision/CollisionShapes/btSphereShape.h>
#include <BulletCollision/CollisionShapes/btStaticPlaneShape.h>
#include <unordered_map>
#include <Nazara/BulletPhysics3D/Debug.hpp>

namespace Nz
{
	BulletCollider3D::~BulletCollider3D() = default;

	Boxf BulletCollider3D::ComputeAABB(const Vector3f& translation, const Quaternionf& rotation, const Vector3f& scale) const
	{
		return ComputeAABB(Matrix4f::Transform(translation, rotation), scale);
	}

	Boxf BulletCollider3D::ComputeAABB(const Matrix4f& offsetMatrix, const Vector3f& scale) const
	{
		btTransform transform = ToBullet(offsetMatrix);

		btVector3 min, max;
		GetShape()->getAabb(transform, min, max);

		return Boxf(scale * FromBullet(min), scale * FromBullet(max));
	}

	void BulletCollider3D::ComputeInertia(float mass, Vector3f* inertia) const
	{
		NazaraAssert(inertia, "invalid inertia pointer");

		btVector3 inertiaVec;
		GetShape()->calculateLocalInertia(mass, inertiaVec);

		*inertia = FromBullet(inertiaVec);
	}

	std::shared_ptr<StaticMesh> BulletCollider3D::GenerateDebugMesh() const
	{
		std::vector<Vector3f> colliderVertices;
		std::vector<UInt16> colliderIndices;
		BuildDebugMesh(colliderVertices, colliderIndices);

		std::shared_ptr<VertexBuffer> colliderVB = std::make_shared<VertexBuffer>(VertexDeclaration::Get(VertexLayout::XYZ), SafeCast<UInt32>(colliderVertices.size()), BufferUsage::Write, SoftwareBufferFactory, colliderVertices.data());
		std::shared_ptr<IndexBuffer> colliderIB = std::make_shared<IndexBuffer>(IndexType::U16, SafeCast<UInt32>(colliderIndices.size()), BufferUsage::Write, SoftwareBufferFactory, colliderIndices.data());

		std::shared_ptr<StaticMesh> colliderSubMesh = std::make_shared<StaticMesh>(std::move(colliderVB), std::move(colliderIB));
		colliderSubMesh->GenerateAABB();
		colliderSubMesh->SetPrimitiveMode(PrimitiveMode::LineList);

		return colliderSubMesh;
	}

	std::shared_ptr<BulletCollider3D> BulletCollider3D::Build(const PrimitiveList& list)
	{
		std::size_t primitiveCount = list.GetSize();
		if (primitiveCount > 1)
		{
			std::vector<BulletCompoundCollider3D::ChildCollider> childColliders(primitiveCount);

			for (unsigned int i = 0; i < primitiveCount; ++i)
			{
				const Primitive& primitive = list.GetPrimitive(i);
				childColliders[i].collider = CreateGeomFromPrimitive(primitive);
				childColliders[i].offsetMatrix = primitive.matrix;
			}

			return std::make_shared<BulletCompoundCollider3D>(std::move(childColliders));
		}
		else if (primitiveCount > 0)
			return CreateGeomFromPrimitive(list.GetPrimitive(0));
		else
			return std::make_shared<BulletNullCollider3D>();
	}
	
	std::shared_ptr<BulletCollider3D> BulletCollider3D::CreateGeomFromPrimitive(const Primitive& primitive)
	{
		switch (primitive.type)
		{
			case PrimitiveType::Box:
				return std::make_shared<BulletBoxCollider3D>(primitive.box.lengths);

			case PrimitiveType::Cone:
				return std::make_shared<BulletConeCollider3D>(primitive.cone.length, primitive.cone.radius);

			case PrimitiveType::Plane:
				return std::make_shared<BulletBoxCollider3D>(Vector3f(primitive.plane.size.x, 0.01f, primitive.plane.size.y));
				///TODO: PlaneGeom?

			case PrimitiveType::Sphere:
				return std::make_shared<BulletSphereCollider3D>(primitive.sphere.size);
		}

		NazaraErrorFmt("Primitive type not handled ({0:#x})", UnderlyingCast(primitive.type));
		return std::shared_ptr<BulletCollider3D>();
	}

	/********************************** BoxCollider3D **********************************/

	BulletBoxCollider3D::BulletBoxCollider3D(const Vector3f& lengths) :
	m_lengths(lengths)
	{
		m_shape = std::make_unique<btBoxShape>(ToBullet(m_lengths * 0.5f));
	}

	BulletBoxCollider3D::~BulletBoxCollider3D() = default;

	void BulletBoxCollider3D::BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const
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

	btCollisionShape* BulletBoxCollider3D::GetShape() const
	{
		return m_shape.get();
	}

	Vector3f BulletBoxCollider3D::GetLengths() const
	{
		return m_lengths;
	}

	BulletColliderType3D BulletBoxCollider3D::GetType() const
	{
		return BulletColliderType3D::Box;
	}

	/******************************** CapsuleCollider3D ********************************/

	BulletCapsuleCollider3D::BulletCapsuleCollider3D(float length, float radius) :
	m_length(length),
	m_radius(radius)
	{
		m_shape = std::make_unique<btCapsuleShape>(radius, length);
	}

	BulletCapsuleCollider3D::~BulletCapsuleCollider3D() = default;

	void BulletCapsuleCollider3D::BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const
	{
	}

	float BulletCapsuleCollider3D::GetLength() const
	{
		return m_length;
	}

	float BulletCapsuleCollider3D::GetRadius() const
	{
		return m_radius;
	}

	btCollisionShape* BulletCapsuleCollider3D::GetShape() const
	{
		return m_shape.get();
	}

	BulletColliderType3D BulletCapsuleCollider3D::GetType() const
	{
		return BulletColliderType3D::Capsule;
	}

	/******************************* CompoundCollider3D ********************************/

	BulletCompoundCollider3D::BulletCompoundCollider3D(std::vector<ChildCollider> childs) :
	m_childs(std::move(childs))
	{
		m_shape = std::make_unique<btCompoundShape>();
		for (const auto& child : m_childs)
		{
			btTransform transform = ToBullet(child.offsetMatrix);
			m_shape->addChildShape(transform, child.collider->GetShape());
		}
	}

	BulletCompoundCollider3D::~BulletCompoundCollider3D() = default;

	void BulletCompoundCollider3D::BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const
	{
		for (const auto& child : m_childs)
			child.collider->BuildDebugMesh(vertices, indices, offsetMatrix * child.offsetMatrix);
	}

	auto BulletCompoundCollider3D::GetGeoms() const -> const std::vector<ChildCollider>&
	{
		return m_childs;
	}

	btCollisionShape* BulletCompoundCollider3D::GetShape() const
	{
		return m_shape.get();
	}

	BulletColliderType3D BulletCompoundCollider3D::GetType() const
	{
		return BulletColliderType3D::Compound;
	}

	/********************************* ConeCollider3D **********************************/

	BulletConeCollider3D::BulletConeCollider3D(float length, float radius) :
	m_length(length),
	m_radius(radius)
	{
		m_shape = std::make_unique<btConeShape>(radius, length);
	}

	BulletConeCollider3D::~BulletConeCollider3D() = default;

	void BulletConeCollider3D::BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const
	{
	}

	float BulletConeCollider3D::GetLength() const
	{
		return m_length;
	}

	float BulletConeCollider3D::GetRadius() const
	{
		return m_radius;
	}

	btCollisionShape* BulletConeCollider3D::GetShape() const
	{
		return m_shape.get();
	}

	BulletColliderType3D BulletConeCollider3D::GetType() const
	{
		return BulletColliderType3D::Cone;
	}

	/****************************** ConvexCollider3D *******************************/

	BulletConvexCollider3D::BulletConvexCollider3D(SparsePtr<const Vector3f> vertices, unsigned int vertexCount)
	{
		static_assert(std::is_same_v<btScalar, float>);

		m_shape = std::make_unique<btConvexHullShape>(reinterpret_cast<const float*>(vertices.GetPtr()), vertexCount, vertices.GetStride());
		m_shape->optimizeConvexHull();
	}

	BulletConvexCollider3D::~BulletConvexCollider3D() = default;

	void BulletConvexCollider3D::BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const
	{
		std::unordered_map<Vector3f, UInt16> vertexCache;
		auto InsertVertex = [&](const Vector3f& position) -> UInt16
		{
			auto it = vertexCache.find(position);
			if (it != vertexCache.end())
				return it->second;

			UInt16 index = SafeCast<UInt16>(vertices.size());

			vertices.push_back(offsetMatrix * position);
			vertexCache.emplace(position, index);

			return index;
		};

		int edgeCount = m_shape->getNumEdges();
		for (int i = 0; i < edgeCount; ++i)
		{
			btVector3 from, to;
			m_shape->getEdge(i, from, to);

			indices.push_back(InsertVertex(FromBullet(from)));
			indices.push_back(InsertVertex(FromBullet(to)));
		}
	}

	btCollisionShape* BulletConvexCollider3D::GetShape() const
	{
		return m_shape.get();
	}

	BulletColliderType3D BulletConvexCollider3D::GetType() const
	{
		return BulletColliderType3D::ConvexHull;
	}

	/******************************* CylinderCollider3D ********************************/

	BulletCylinderCollider3D::BulletCylinderCollider3D(float length, float radius) :
	m_length(length),
	m_radius(radius)
	{
		// TODO: Allow to use two different radius
		m_shape = std::make_unique<btCylinderShape>(btVector3{ radius, length, radius });
	}

	BulletCylinderCollider3D::~BulletCylinderCollider3D() = default;

	void BulletCylinderCollider3D::BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const
	{
	}

	float BulletCylinderCollider3D::GetLength() const
	{
		return m_length;
	}

	float BulletCylinderCollider3D::GetRadius() const
	{
		return m_radius;
	}

	btCollisionShape* BulletCylinderCollider3D::GetShape() const
	{
		return m_shape.get();
	}

	BulletColliderType3D BulletCylinderCollider3D::GetType() const
	{
		return BulletColliderType3D::Cylinder;
	}

	/********************************* NullCollider3D **********************************/

	BulletNullCollider3D::BulletNullCollider3D()
	{
		m_shape = std::make_unique<btEmptyShape>();
	}

	BulletNullCollider3D::~BulletNullCollider3D() = default;

	void BulletNullCollider3D::BuildDebugMesh(std::vector<Vector3f>& /*vertices*/, std::vector<UInt16>& /*indices*/, const Matrix4f& /*offsetMatrix*/) const
	{
	}

	void BulletNullCollider3D::ComputeInertia(float /*mass*/, Vector3f* inertia) const
	{
		*inertia = Vector3f::Unit();
	}

	btCollisionShape* BulletNullCollider3D::GetShape() const
	{
		return m_shape.get();
	}

	BulletColliderType3D BulletNullCollider3D::GetType() const
	{
		return BulletColliderType3D::Null;
	}

	/******************************** SphereCollider3D *********************************/

	BulletSphereCollider3D::BulletSphereCollider3D(float radius) :
	m_radius(radius)
	{
		m_shape = std::make_unique<btSphereShape>(radius);
	}

	BulletSphereCollider3D::~BulletSphereCollider3D() = default;

	void BulletSphereCollider3D::BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const
	{
	}

	float BulletSphereCollider3D::GetRadius() const
	{
		return m_radius;
	}

	btCollisionShape* BulletSphereCollider3D::GetShape() const
	{
		return m_shape.get();
	}

	BulletColliderType3D BulletSphereCollider3D::GetType() const
	{
		return BulletColliderType3D::Sphere;
	}

	/******************************** StaticPlaneCollider3D *********************************/

	BulletStaticPlaneCollider3D::BulletStaticPlaneCollider3D(const Planef& plane) :
	BulletStaticPlaneCollider3D(plane.normal, plane.distance)
	{
	}

	BulletStaticPlaneCollider3D::BulletStaticPlaneCollider3D(const Vector3f& normal, float distance) :
	m_normal(normal),
	m_distance(distance)
	{
		m_shape = std::make_unique<btStaticPlaneShape>(ToBullet(m_normal), m_distance);
	}

	BulletStaticPlaneCollider3D::~BulletStaticPlaneCollider3D() = default;

	void BulletStaticPlaneCollider3D::BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const
	{
	}

	float BulletStaticPlaneCollider3D::GetDistance() const
	{
		return m_distance;
	}

	const Vector3f& BulletStaticPlaneCollider3D::GetNormal() const
	{
		return m_normal;
	}

	btCollisionShape* BulletStaticPlaneCollider3D::GetShape() const
	{
		return m_shape.get();
	}

	BulletColliderType3D BulletStaticPlaneCollider3D::GetType() const
	{
		return BulletColliderType3D::StaticPlane;
	}
}
