// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - JoltPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/JoltPhysics3D/JoltCollider3D.hpp>
#include <Nazara/Core/PrimitiveList.hpp>
#include <Nazara/JoltPhysics3D/JoltHelper.hpp>
#include <Nazara/Utility/Algorithm.hpp>
#include <Nazara/Utility/IndexBuffer.hpp>
#include <Nazara/Utility/SoftwareBuffer.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>
#include <Nazara/Utils/MemoryHelper.hpp>
#include <Jolt/Core/Core.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/StaticCompoundShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Nazara/JoltPhysics3D/Debug.hpp>

namespace Nz
{
	JoltCollider3D::~JoltCollider3D() = default;

	std::shared_ptr<StaticMesh> JoltCollider3D::GenerateDebugMesh() const
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

	std::shared_ptr<JoltCollider3D> JoltCollider3D::Build(const PrimitiveList& list)
	{
		std::size_t primitiveCount = list.GetSize();
		if (primitiveCount > 1)
		{
			std::vector<JoltCompoundCollider3D::ChildCollider> childColliders(primitiveCount);

			for (unsigned int i = 0; i < primitiveCount; ++i)
			{
				const Primitive& primitive = list.GetPrimitive(i);
				childColliders[i].collider = CreateGeomFromPrimitive(primitive);
				childColliders[i].offset = primitive.matrix.GetTranslation();
				childColliders[i].rotation = primitive.matrix.GetRotation();
			}

			return std::make_shared<JoltCompoundCollider3D>(std::move(childColliders));
		}
		else if (primitiveCount > 0)
			return CreateGeomFromPrimitive(list.GetPrimitive(0));
		else
			return nullptr;// std::make_shared<NullCollider3D>();  //< TODO
	}
	
	std::shared_ptr<JoltCollider3D> JoltCollider3D::CreateGeomFromPrimitive(const Primitive& primitive)
	{
		switch (primitive.type)
		{
			case PrimitiveType::Box:
				return std::make_shared<JoltBoxCollider3D>(primitive.box.lengths);

			case PrimitiveType::Cone:
				return nullptr; //< TODO
				//return std::make_shared<ConeCollider3D>(primitive.cone.length, primitive.cone.radius);

			case PrimitiveType::Plane:
				return std::make_shared<JoltBoxCollider3D>(Vector3f(primitive.plane.size.x, 0.01f, primitive.plane.size.y));
				///TODO: PlaneGeom?

			case PrimitiveType::Sphere:
				return std::make_shared<JoltSphereCollider3D>(primitive.sphere.size);
		}

		NazaraError("Primitive type not handled (0x" + NumberToString(UnderlyingCast(primitive.type), 16) + ')');
		return std::shared_ptr<JoltCollider3D>();
	}

	/********************************** BoxCollider3D **********************************/

	JoltBoxCollider3D::JoltBoxCollider3D(const Vector3f& lengths, float convexRadius) :
	m_lengths(lengths)
	{
		m_shapeSettings = std::make_unique<JPH::BoxShapeSettings>(ToJolt(m_lengths * 0.5f), convexRadius);
	}

	JoltBoxCollider3D::~JoltBoxCollider3D() = default;

	void JoltBoxCollider3D::BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const
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

	JPH::ShapeSettings* JoltBoxCollider3D::GetShapeSettings() const
	{
		return m_shapeSettings.get();
	}

	Vector3f JoltBoxCollider3D::GetLengths() const
	{
		return m_lengths;
	}

	JoltColliderType3D JoltBoxCollider3D::GetType() const
	{
		return JoltColliderType3D::Box;
	}

	/******************************* JoltCompoundCollider3D ********************************/

	JoltCompoundCollider3D::JoltCompoundCollider3D(std::vector<ChildCollider> childs) :
	m_childs(std::move(childs))
	{
		m_shapeSettings = std::make_unique<JPH::StaticCompoundShapeSettings>();
		for (const auto& child : m_childs)
			m_shapeSettings->AddShape(ToJolt(child.offset), ToJolt(child.rotation), child.collider->GetShapeSettings());
	}

	JoltCompoundCollider3D::~JoltCompoundCollider3D() = default;

	void JoltCompoundCollider3D::BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const
	{
		for (const auto& child : m_childs)
			child.collider->BuildDebugMesh(vertices, indices, offsetMatrix * Matrix4f::Transform(child.offset, child.rotation));
	}

	auto JoltCompoundCollider3D::GetGeoms() const -> const std::vector<ChildCollider>&
	{
		return m_childs;
	}

	JPH::ShapeSettings* JoltCompoundCollider3D::GetShapeSettings() const
	{
		return m_shapeSettings.get();
	}

	JoltColliderType3D JoltCompoundCollider3D::GetType() const
	{
		return JoltColliderType3D::Compound;
	}

	/******************************** JoltSphereCollider3D *********************************/

	JoltSphereCollider3D::JoltSphereCollider3D(float radius) :
	m_radius(radius)
	{
		m_shapeSettings = std::make_unique<JPH::SphereShapeSettings>(radius);
	}

	JoltSphereCollider3D::~JoltSphereCollider3D() = default;

	void JoltSphereCollider3D::BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const
	{
	}

	float JoltSphereCollider3D::GetRadius() const
	{
		return m_radius;
	}

	JPH::ShapeSettings* JoltSphereCollider3D::GetShapeSettings() const
	{
		return m_shapeSettings.get();
	}

	JoltColliderType3D JoltSphereCollider3D::GetType() const
	{
		return JoltColliderType3D::Sphere;
	}
}
