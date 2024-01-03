// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
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
#include <NazaraUtils/StackArray.hpp>
#include <Jolt/Core/Core.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/StaticCompoundShape.h>
#include <optional>
#include <unordered_map>
#include <Nazara/JoltPhysics3D/Debug.hpp>

namespace Nz
{
	JoltCollider3D::JoltCollider3D() = default;
	JoltCollider3D::~JoltCollider3D() = default;

	std::shared_ptr<StaticMesh> JoltCollider3D::GenerateDebugMesh() const
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

	void JoltCollider3D::ResetShapeSettings()
	{
		m_shapeSettings.reset();
	}

	void JoltCollider3D::SetupShapeSettings(std::unique_ptr<JPH::ShapeSettings>&& shapeSettings)
	{
		assert(!m_shapeSettings);
		m_shapeSettings = std::move(shapeSettings);
		m_shapeSettings->SetEmbedded();

		if (auto result = m_shapeSettings->Create(); result.HasError())
			throw std::runtime_error(std::string("shape creation failed: ") + result.GetError().c_str());
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

		NazaraErrorFmt("Primitive type not handled ({0:#x})", UnderlyingCast(primitive.type));
		return std::shared_ptr<JoltCollider3D>();
	}

	/********************************** JoltBoxCollider3D **********************************/

	JoltBoxCollider3D::JoltBoxCollider3D(const Vector3f& lengths, float convexRadius)
	{
		SetupShapeSettings(std::make_unique<JPH::BoxShapeSettings>(ToJolt(lengths * 0.5f), convexRadius));
	}

	void JoltBoxCollider3D::BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const
	{
		auto InsertVertex = [&](float x, float y, float z) -> UInt16
		{
			UInt16 index = SafeCast<UInt16>(vertices.size());
			vertices.push_back(offsetMatrix * Vector3f(x, y, z));
			return index;
		};

		Vector3f halfLengths = FromJolt(GetShapeSettingsAs<JPH::BoxShapeSettings>()->mHalfExtent);

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

	Vector3f JoltBoxCollider3D::GetLengths() const
	{
		return FromJolt(GetShapeSettingsAs<JPH::BoxShapeSettings>()->mHalfExtent) * 2.f;
	}

	JoltColliderType3D JoltBoxCollider3D::GetType() const
	{
		return JoltColliderType3D::Box;
	}

	/********************************** JoltCapsuleCollider3D **********************************/

	JoltCapsuleCollider3D::JoltCapsuleCollider3D(float height, float radius)
	{
		SetupShapeSettings(std::make_unique<JPH::CapsuleShapeSettings>(height * 0.5f, radius));
	}

	void JoltCapsuleCollider3D::BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const
	{
		constexpr unsigned int cornerStepCount = 10;
		constexpr unsigned int sliceCount = 4;

		const JPH::CapsuleShapeSettings* capsuleSettings = GetShapeSettingsAs<JPH::CapsuleShapeSettings>();
		float radius = capsuleSettings->mRadius;
		float halfHeight = capsuleSettings->mHalfHeightOfCylinder;

		std::optional<UInt16> firstVertex;
		std::optional<UInt16> lastVertex;

		for (unsigned int slice = 0; slice < sliceCount; ++slice)
		{
			Quaternionf rot(RadianAnglef(2.f * Pi<float> * slice / sliceCount), Nz::Vector3f::Down());
			
			Vector3f top(0.f, halfHeight, 0.f);
			for (unsigned int i = 0; i < cornerStepCount; ++i)
			{
				auto [sin, cos] = RadianAnglef(0.5f * Pi<float> * i / cornerStepCount).GetSinCos();

				UInt16 index;
				if (firstVertex && i == 0)
					index = *firstVertex;
				else
				{
					index = SafeCast<UInt16>(vertices.size());
					vertices.push_back(offsetMatrix * (top + rot * (radius * Vector3f(sin, cos, 0.f))));
					if (i == 0)
						firstVertex = index;
				}

				if (i > 1)
				{
					indices.push_back(static_cast<UInt16>(index - 1));
					indices.push_back(index);
				}
				else if (i == 1)
				{
					indices.push_back(*firstVertex);
					indices.push_back(index);
				}
			}

			Vector3f bottom(0.f, -halfHeight, 0.f);
			for (unsigned int i = 0; i < cornerStepCount; ++i)
			{
				auto [sin, cos] = RadianAnglef(0.5f * Pi<float> * i / cornerStepCount).GetSinCos();

				UInt16 index;
				if (lastVertex && i == sliceCount - 1)
					index = *lastVertex;
				else
				{
					index = SafeCast<UInt16>(vertices.size());
					vertices.push_back(offsetMatrix * (bottom - rot * (radius * Vector3f(-cos, sin, 0.f))));
					if (i == sliceCount - 1)
						lastVertex = index;
				}

				indices.push_back(static_cast<UInt16>(index - 1));
				indices.push_back(index);
			}
		}
	}

	float JoltCapsuleCollider3D::GetHeight() const
	{
		return GetShapeSettingsAs<JPH::CapsuleShapeSettings>()->mHalfHeightOfCylinder * 2.0f;
	}

	float JoltCapsuleCollider3D::GetRadius() const
	{
		return GetShapeSettingsAs<JPH::CapsuleShapeSettings>()->mRadius;
	}

	JoltColliderType3D JoltCapsuleCollider3D::GetType() const
	{
		return JoltColliderType3D::Capsule;
	}

	/******************************* JoltCompoundCollider3D ********************************/

	JoltCompoundCollider3D::JoltCompoundCollider3D(std::vector<ChildCollider> childs) :
	m_childs(std::move(childs))
	{
		auto shapeSettings = std::make_unique<JPH::StaticCompoundShapeSettings>();
		for (const auto& child : m_childs)
			shapeSettings->AddShape(ToJolt(child.offset), ToJolt(child.rotation), child.collider->GetShapeSettings());

		SetupShapeSettings(std::move(shapeSettings));
	}

	JoltCompoundCollider3D::~JoltCompoundCollider3D()
	{
		// We have to destroy shape settings first as it carries references on the inner colliders
		ResetShapeSettings();
	}

	void JoltCompoundCollider3D::BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const
	{
		for (const auto& child : m_childs)
			child.collider->BuildDebugMesh(vertices, indices, offsetMatrix * Matrix4f::Transform(child.offset, child.rotation));
	}

	auto JoltCompoundCollider3D::GetGeoms() const -> const std::vector<ChildCollider>&
	{
		return m_childs;
	}

	JoltColliderType3D JoltCompoundCollider3D::GetType() const
	{
		return JoltColliderType3D::Compound;
	}

	/******************************** JoltConvexHullCollider3D *********************************/

	JoltConvexHullCollider3D::JoltConvexHullCollider3D(SparsePtr<const Vector3f> vertices, std::size_t vertexCount, float hullTolerance, float convexRadius, float maxErrorConvexRadius)
	{
		std::unique_ptr<JPH::ConvexHullShapeSettings> settings = std::make_unique<JPH::ConvexHullShapeSettings>();
		settings->mHullTolerance = hullTolerance;
		settings->mMaxConvexRadius = convexRadius;
		settings->mMaxErrorConvexRadius = maxErrorConvexRadius;

		settings->mPoints.resize(vertexCount);
		for (std::size_t i = 0; i < vertexCount; ++i)
			settings->mPoints[i] = ToJolt(vertices[i]);

		SetupShapeSettings(std::move(settings));
	}

	void JoltConvexHullCollider3D::BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const
	{
		const JPH::ConvexHullShapeSettings* settings = GetShapeSettingsAs<JPH::ConvexHullShapeSettings>();
		const JPH::ConvexHullShape* shape = SafeCast<const JPH::ConvexHullShape*>(settings->Create().Get().GetPtr());

		std::unordered_map<unsigned int, UInt16> vertexCache;
		auto InsertVertex = [&](unsigned int vertexIndex) -> UInt16
		{
			auto it = vertexCache.find(vertexIndex);
			if (it != vertexCache.end())
				return it->second;

			UInt16 index = SafeCast<UInt16>(vertices.size());

			vertices.push_back(offsetMatrix * FromJolt(shape->GetPoint(vertexIndex)));
			vertexCache.emplace(vertexIndex, index);

			return index;
		};

		unsigned int faceCount = shape->GetNumFaces();
		unsigned int maxVerticesInFace = 0;
		for (unsigned int i = 0; i < faceCount; ++i)
			maxVerticesInFace = std::max(maxVerticesInFace, shape->GetNumVerticesInFace(i));

		StackArray<unsigned int> faceVertices = NazaraStackArrayNoInit(unsigned int, maxVerticesInFace);
		for (unsigned int i = 0; i < faceCount; ++i)
		{
			unsigned int vertexCount = shape->GetFaceVertices(i, maxVerticesInFace, faceVertices.data());
			if NAZARA_LIKELY(vertexCount >= 2)
			{
				for (unsigned int j = 1; j < vertexCount; ++j)
				{
					indices.push_back(InsertVertex(faceVertices[j - 1]));
					indices.push_back(InsertVertex(faceVertices[j]));
				}

				if NAZARA_LIKELY(vertexCount > 2)
				{
					indices.push_back(InsertVertex(faceVertices[vertexCount - 1]));
					indices.push_back(InsertVertex(faceVertices[0]));
				}
			}
		}
	}

	JoltColliderType3D JoltConvexHullCollider3D::GetType() const
	{
		return JoltColliderType3D::Convex;
	}

	/******************************** JoltMeshCollider3D *********************************/
	
	JoltMeshCollider3D::JoltMeshCollider3D(SparsePtr<const Vector3f> vertices, std::size_t vertexCount, SparsePtr<const UInt16> indices, std::size_t indexCount)
	{
		std::unique_ptr<JPH::MeshShapeSettings> settings = std::make_unique<JPH::MeshShapeSettings>();
		settings->mTriangleVertices.resize(vertexCount);
		for (std::size_t i = 0; i < vertexCount; ++i)
		{
			settings->mTriangleVertices[i].x = vertices[i].x;
			settings->mTriangleVertices[i].y = vertices[i].y;
			settings->mTriangleVertices[i].z = vertices[i].z;
		}

		std::size_t triangleCount = indexCount / 3;
		settings->mIndexedTriangles.resize(triangleCount);
		for (std::size_t i = 0; i < triangleCount; ++i)
		{
			settings->mIndexedTriangles[i].mIdx[0] = indices[i * 3 + 0];
			settings->mIndexedTriangles[i].mIdx[1] = indices[i * 3 + 1];
			settings->mIndexedTriangles[i].mIdx[2] = indices[i * 3 + 2];
		}

		settings->Sanitize();

		SetupShapeSettings(std::move(settings));
	}

	JoltMeshCollider3D::JoltMeshCollider3D(SparsePtr<const Vector3f> vertices, std::size_t vertexCount, SparsePtr<const UInt32> indices, std::size_t indexCount)
	{
		std::unique_ptr<JPH::MeshShapeSettings> settings = std::make_unique<JPH::MeshShapeSettings>();
		settings->mTriangleVertices.resize(vertexCount);
		for (std::size_t i = 0; i < vertexCount; ++i)
		{
			settings->mTriangleVertices[i].x = vertices[i].x;
			settings->mTriangleVertices[i].y = vertices[i].y;
			settings->mTriangleVertices[i].z = vertices[i].z;
		}

		std::size_t triangleCount = indexCount / 3;
		settings->mIndexedTriangles.resize(triangleCount);
		for (std::size_t i = 0; i < triangleCount; ++i)
		{
			settings->mIndexedTriangles[i].mIdx[0] = indices[i * 3 + 0];
			settings->mIndexedTriangles[i].mIdx[1] = indices[i * 3 + 1];
			settings->mIndexedTriangles[i].mIdx[2] = indices[i * 3 + 2];
		}

		settings->Sanitize();

		SetupShapeSettings(std::move(settings));
	}

	void JoltMeshCollider3D::BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const
	{
	}

	JoltColliderType3D JoltMeshCollider3D::GetType() const
	{
		return JoltColliderType3D::Mesh;
	}

	/******************************** JoltSphereCollider3D *********************************/

	JoltSphereCollider3D::JoltSphereCollider3D(float radius)
	{
		SetupShapeSettings(std::make_unique<JPH::SphereShapeSettings>(radius));
	}

	void JoltSphereCollider3D::BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const
	{
	}

	float JoltSphereCollider3D::GetRadius() const
	{
		return GetShapeSettingsAs<JPH::SphereShapeSettings>()->mRadius;
	}

	JoltColliderType3D JoltSphereCollider3D::GetType() const
	{
		return JoltColliderType3D::Sphere;
	}

	/******************************** JoltTranslatedRotatedCollider3D *********************************/

	JoltTranslatedRotatedCollider3D::JoltTranslatedRotatedCollider3D(std::shared_ptr<JoltCollider3D> collider, const Vector3f& translation, const Quaternionf& rotation) :
	m_collider(std::move(collider))
	{
		SetupShapeSettings(std::make_unique<JPH::RotatedTranslatedShapeSettings>(ToJolt(translation), ToJolt(rotation), m_collider->GetShapeSettings()));
	}

	JoltTranslatedRotatedCollider3D::~JoltTranslatedRotatedCollider3D()
	{
		// We have to destroy shape settings first as it carries references on the inner collider
		ResetShapeSettings();
	}

	void JoltTranslatedRotatedCollider3D::BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const
	{
		const JPH::RotatedTranslatedShapeSettings* settings = GetShapeSettingsAs<JPH::RotatedTranslatedShapeSettings>();

		m_collider->BuildDebugMesh(vertices, indices, offsetMatrix * Matrix4f::Transform(FromJolt(settings->mPosition), FromJolt(settings->mRotation)));
	}

	JoltColliderType3D JoltTranslatedRotatedCollider3D::GetType() const
	{
		return JoltColliderType3D::TranslatedRotatedDecoration;
	}
}
