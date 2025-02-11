// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Physics3D/Collider3D.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/IndexBuffer.hpp>
#include <Nazara/Core/PrimitiveList.hpp>
#include <Nazara/Core/SoftwareBuffer.hpp>
#include <Nazara/Core/StaticMesh.hpp>
#include <Nazara/Core/VertexBuffer.hpp>
#include <Nazara/Physics3D/JoltHelper.hpp>
#include <NazaraUtils/StackArray.hpp>
#include <Jolt/Core/Core.h>
#include <Jolt/Physics/Collision/CollidePointResult.h>
#include <Jolt/Physics/Collision/CollisionCollectorImpl.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>
#include <Jolt/Physics/Collision/Shape/EmptyShape.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>
#include <Jolt/Physics/Collision/Shape/ScaledShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/StaticCompoundShape.h>
#include <optional>
#include <unordered_map>

namespace Nz
{
	Collider3D::Collider3D() = default;
	Collider3D::~Collider3D() = default;

	void Collider3D::BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const
	{
		const JPH::Shape* shape = GetShape();

		JPH::Shape::GetTrianglesContext context;
		shape->GetTrianglesStart(context, shape->GetLocalBounds(), shape->GetCenterOfMass(), JPH::Quat::sIdentity(), JPH::Vec3::sReplicate(1.f));

		constexpr int maxTrianglePerCall = 128;

		std::array<JPH::Float3, maxTrianglePerCall * 3> trianglePos;
		int triangleCount;
		while ((triangleCount = shape->GetTrianglesNext(context, maxTrianglePerCall, &trianglePos[0], nullptr)) > 0)
		{
			std::size_t vertexOffset = vertices.size();
			for (int i = 0; i < triangleCount; ++i)
			{
				vertices.push_back(offsetMatrix * FromJolt(trianglePos[i * 3 + 0]));
				vertices.push_back(offsetMatrix * FromJolt(trianglePos[i * 3 + 1]));
				vertices.push_back(offsetMatrix * FromJolt(trianglePos[i * 3 + 2]));

				std::size_t offset = vertexOffset + i * 3;
				indices.push_back(SafeCast<UInt16>(offset + 0));
				indices.push_back(SafeCast<UInt16>(offset + 1));

				indices.push_back(SafeCast<UInt16>(offset + 1));
				indices.push_back(SafeCast<UInt16>(offset + 2));

				indices.push_back(SafeCast<UInt16>(offset + 2));
				indices.push_back(SafeCast<UInt16>(offset + 0));
			}
		}
	}

	bool Collider3D::CollisionQuery(const Vector3f& point) const
	{
		auto result = m_shapeSettings->Create();
		if (result.HasError())
			throw std::runtime_error(std::string("shape creation failed: ") + result.GetError().c_str());

		JPH::AnyHitCollisionCollector<JPH::CollidePointCollector> collector;

		result.Get()->CollidePoint(ToJolt(point), JPH::SubShapeIDCreator{}, collector);

		return collector.HadHit();
	}

	std::shared_ptr<StaticMesh> Collider3D::GenerateDebugMesh() const
	{
		std::vector<Vector3f> colliderVertices;
		std::vector<UInt16> colliderIndices;
		BuildDebugMesh(colliderVertices, colliderIndices);
		if (colliderIndices.empty())
			return nullptr;

		std::shared_ptr<VertexBuffer> colliderVB = std::make_shared<VertexBuffer>(VertexDeclaration::Get(VertexLayout::XYZ), SafeCast<UInt32>(colliderVertices.size()), BufferUsage::Write, SoftwareBufferFactory, colliderVertices.data());
		std::shared_ptr<IndexBuffer> colliderIB = std::make_shared<IndexBuffer>(IndexType::U16, SafeCast<UInt32>(colliderIndices.size()), BufferUsage::Write, SoftwareBufferFactory, colliderIndices.data());

		std::shared_ptr<StaticMesh> colliderSubMesh = std::make_shared<StaticMesh>(std::move(colliderVB), std::move(colliderIB));
		colliderSubMesh->GenerateAABB();
		colliderSubMesh->SetPrimitiveMode(PrimitiveMode::LineList);

		return colliderSubMesh;
	}

	Boxf Collider3D::GetBoundingBox() const
	{
		return FromJolt(GetShape()->GetLocalBounds());
	}

	Vector3f Collider3D::GetCenterOfMass() const
	{
		return FromJolt(GetShape()->GetCenterOfMass());
	}

	const Collider3D* Collider3D::GetSubCollider(UInt32 subShapeID, UInt32& remainder) const
	{
		JPH::SubShapeID joltSubShapeID;
		joltSubShapeID.SetValue(subShapeID);

		JPH::SubShapeID joltRemainder;
		const JPH::Shape* leafShape = GetShape()->GetLeafShape(joltSubShapeID, joltRemainder);
		if (!leafShape)
			return nullptr;

		remainder = joltRemainder.GetValue();

		return IntegerToPointer<const Nz::Collider3D*>(leafShape->GetUserData());
	}

	void Collider3D::ResetShapeSettings()
	{
		m_shapeSettings.reset();
	}

	void Collider3D::SetupShapeSettings(std::unique_ptr<JPH::ShapeSettings>&& shapeSettings)
	{
		assert(!m_shapeSettings);
		m_shapeSettings = std::move(shapeSettings);
		m_shapeSettings->SetEmbedded();

		auto result = m_shapeSettings->Create();
		if (result.HasError())
			throw std::runtime_error(std::string("shape creation failed: ") + result.GetError().c_str());

		result.Get()->SetUserData(PointerToInteger<JPH::uint64>(this));
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
				childColliders[i].offset = primitive.matrix.GetTranslation();
				childColliders[i].rotation = primitive.matrix.GetRotation();
			}

			return std::make_shared<CompoundCollider3D>(std::move(childColliders));
		}
		else if (primitiveCount > 0)
			return CreateGeomFromPrimitive(list.GetPrimitive(0));
		else
			return nullptr;// std::make_shared<NullCollider3D>();  //< TODO
	}

	const JPH::Shape* Collider3D::GetShape() const
	{
		auto result = m_shapeSettings->Create();
		if (result.HasError())
			throw std::runtime_error(std::string("shape creation failed: ") + result.GetError().c_str());

		return result.Get();
	}

	std::shared_ptr<Collider3D> Collider3D::CreateGeomFromPrimitive(const Primitive& primitive)
	{
		switch (primitive.type)
		{
			case PrimitiveType::Box:
				return std::make_shared<BoxCollider3D>(primitive.box.lengths);

			case PrimitiveType::Cone:
				return nullptr; //< TODO
				//return std::make_shared<ConeCollider3D>(primitive.cone.length, primitive.cone.radius);

			case PrimitiveType::Plane:
				return std::make_shared<BoxCollider3D>(Vector3f(primitive.plane.size.x, 0.01f, primitive.plane.size.y));
				///TODO: PlaneGeom?

			case PrimitiveType::Sphere:
				return std::make_shared<SphereCollider3D>(primitive.sphere.size);
		}

		NazaraError("Primitive type not handled ({0:#x})", UnderlyingCast(primitive.type));
		return std::shared_ptr<Collider3D>();
	}

	/********************************** BoxCollider3D **********************************/

	BoxCollider3D::BoxCollider3D(const Vector3f& lengths, float convexRadius)
	{
		SetupShapeSettings(std::make_unique<JPH::BoxShapeSettings>(ToJolt(lengths * 0.5f), convexRadius));
	}

	void BoxCollider3D::BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const
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

	Vector3f BoxCollider3D::GetLengths() const
	{
		return FromJolt(GetShapeSettingsAs<JPH::BoxShapeSettings>()->mHalfExtent) * 2.f;
	}

	ColliderType3D BoxCollider3D::GetType() const
	{
		return ColliderType3D::Box;
	}

	/********************************** CapsuleCollider3D **********************************/

	CapsuleCollider3D::CapsuleCollider3D(float height, float radius)
	{
		SetupShapeSettings(std::make_unique<JPH::CapsuleShapeSettings>(height * 0.5f, radius));
	}

	void CapsuleCollider3D::BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const
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
			Quaternionf rot(RadianAnglef(Tau<float>() * slice / sliceCount), Nz::Vector3f::Down());

			Vector3f top(0.f, halfHeight, 0.f);
			for (unsigned int i = 0; i < cornerStepCount; ++i)
			{
				auto [sin, cos] = RadianAnglef(HalfPi<float>() * i / cornerStepCount).GetSinCos();

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
				auto [sin, cos] = RadianAnglef(HalfPi<float>() * i / cornerStepCount).GetSinCos();

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

	float CapsuleCollider3D::GetHeight() const
	{
		return GetShapeSettingsAs<JPH::CapsuleShapeSettings>()->mHalfHeightOfCylinder * 2.0f;
	}

	float CapsuleCollider3D::GetRadius() const
	{
		return GetShapeSettingsAs<JPH::CapsuleShapeSettings>()->mRadius;
	}

	ColliderType3D CapsuleCollider3D::GetType() const
	{
		return ColliderType3D::Capsule;
	}

	/******************************* CompoundCollider3D ********************************/

	CompoundCollider3D::CompoundCollider3D(std::vector<ChildCollider> childs) :
	m_childs(std::move(childs))
	{
		auto shapeSettings = std::make_unique<JPH::StaticCompoundShapeSettings>();
		for (const auto& child : m_childs)
			shapeSettings->AddShape(ToJolt(child.offset), ToJolt(child.rotation), child.collider->GetShapeSettings());

		SetupShapeSettings(std::move(shapeSettings));
	}

	CompoundCollider3D::~CompoundCollider3D()
	{
		// We have to destroy shape settings first as it carries references on the inner colliders
		ResetShapeSettings();
	}

	void CompoundCollider3D::BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const
	{
		for (const auto& child : m_childs)
			child.collider->BuildDebugMesh(vertices, indices, offsetMatrix * Matrix4f::Transform(child.offset, child.rotation));
	}

	auto CompoundCollider3D::GetGeoms() const -> const std::vector<ChildCollider>&
	{
		return m_childs;
	}

	ColliderType3D CompoundCollider3D::GetType() const
	{
		return ColliderType3D::Compound;
	}

	/******************************** ConvexHullCollider3D *********************************/

	ConvexHullCollider3D::ConvexHullCollider3D(SparsePtr<const Vector3f> vertices, std::size_t vertexCount, float hullTolerance, float convexRadius, float maxErrorConvexRadius)
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

	void ConvexHullCollider3D::BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const
	{
		const JPH::ConvexHullShape* shape = GetShapeAs<JPH::ConvexHullShape>();

		JPH::Vec3 centerOfMass = shape->GetCenterOfMass();

		std::unordered_map<unsigned int, UInt16> vertexCache;
		auto InsertVertex = [&](unsigned int vertexIndex) -> UInt16
		{
			auto it = vertexCache.find(vertexIndex);
			if (it != vertexCache.end())
				return it->second;

			UInt16 index = SafeCast<UInt16>(vertices.size());

			vertices.push_back(offsetMatrix * FromJolt(shape->GetPoint(vertexIndex) + centerOfMass));
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

	ColliderType3D ConvexHullCollider3D::GetType() const
	{
		return ColliderType3D::Convex;
	}

	/******************************** EmptyCollider3D *********************************/

	EmptyCollider3D::EmptyCollider3D()
	{
		SetupShapeSettings(std::make_unique<JPH::EmptyShapeSettings>());
	}

	void EmptyCollider3D::BuildDebugMesh(std::vector<Vector3f>& /*vertices*/, std::vector<UInt16>& /*indices*/, const Matrix4f& /*offsetMatrix*/) const
	{
	}

	ColliderType3D EmptyCollider3D::GetType() const
	{
		return ColliderType3D::Empty;
	}

	std::shared_ptr<EmptyCollider3D> EmptyCollider3D::Get()
	{
		static std::shared_ptr<EmptyCollider3D> emptyCollider = std::make_shared<EmptyCollider3D>();
		return emptyCollider;
	}

	/******************************** MeshCollider3D *********************************/

	MeshCollider3D::MeshCollider3D(const Settings& meshSettings)
	{
		NazaraAssertMsg(meshSettings.indexCount > 0, "index count should be positive (got %u)", meshSettings.vertexCount);
		NazaraAssertMsg(meshSettings.indexCount % 3 == 0, "index count should be a multiple of 3 (got %u)", meshSettings.indexCount);
		NazaraAssertMsg(meshSettings.vertexCount > 0, "vertex count should be positive (got %u)", meshSettings.vertexCount);
		NazaraAssertMsg(meshSettings.vertices != nullptr, "invalid vertices", meshSettings.vertexCount);

		std::unique_ptr<JPH::MeshShapeSettings> settings = std::make_unique<JPH::MeshShapeSettings>();
		settings->mTriangleVertices.resize(meshSettings.vertexCount);
		for (std::size_t i = 0; i < meshSettings.vertexCount; ++i)
		{
			settings->mTriangleVertices[i].x = meshSettings.vertices[i].x;
			settings->mTriangleVertices[i].y = meshSettings.vertices[i].y;
			settings->mTriangleVertices[i].z = meshSettings.vertices[i].z;
		}

		std::size_t triangleCount = meshSettings.indexCount / 3;
		settings->mIndexedTriangles.resize(triangleCount);

		auto FillIndices = [&]<typename T>(SparsePtr<T> indexPtr)
		{
			for (std::size_t i = 0; i < triangleCount; ++i)
			{
				settings->mIndexedTriangles[i].mIdx[0] = indexPtr[i * 3 + 0];
				settings->mIndexedTriangles[i].mIdx[1] = indexPtr[i * 3 + 1];
				settings->mIndexedTriangles[i].mIdx[2] = indexPtr[i * 3 + 2];
			}
		};

		std::visit(Overloaded {
			[](std::nullptr_t)
			{
				NazaraError("invalid indices");
			},
			[&](SparsePtr<const UInt16> indexPtr)
			{
				FillIndices(indexPtr);
			},
			[&](SparsePtr<const UInt32> indexPtr)
			{
				FillIndices(indexPtr);
			}
		}, meshSettings.indices);

		if (meshSettings.triangleMaterials)
		{
			for (std::size_t i = 0; i < triangleCount; ++i)
				settings->mIndexedTriangles[i].mMaterialIndex = meshSettings.triangleMaterials[i];
		}

		if (meshSettings.triangleUserdata)
		{
			for (std::size_t i = 0; i < triangleCount; ++i)
				settings->mIndexedTriangles[i].mUserData = meshSettings.triangleUserdata[i];

			settings->mPerTriangleUserData = true;
		}

		settings->Sanitize();

		SetupShapeSettings(std::move(settings));
	}

	UInt32 MeshCollider3D::GetTriangleUserData(UInt32 subShapeID) const
	{
		JPH::SubShapeID joltSubShapeID;
		joltSubShapeID.SetValue(subShapeID);

		return GetShapeAs<JPH::MeshShape>()->GetTriangleUserData(joltSubShapeID);
	}

	ColliderType3D MeshCollider3D::GetType() const
	{
		return ColliderType3D::Mesh;
	}

	/******************************** SphereCollider3D *********************************/

	SphereCollider3D::SphereCollider3D(float radius)
	{
		SetupShapeSettings(std::make_unique<JPH::SphereShapeSettings>(radius));
	}

	float SphereCollider3D::GetRadius() const
	{
		return GetShapeSettingsAs<JPH::SphereShapeSettings>()->mRadius;
	}

	ColliderType3D SphereCollider3D::GetType() const
	{
		return ColliderType3D::Sphere;
	}

	/******************************** ScaledCollider3D *********************************/

	ScaledCollider3D::ScaledCollider3D(std::shared_ptr<Collider3D> collider, const Vector3f& scale) :
	m_collider(std::move(collider))
	{
		SetupShapeSettings(std::make_unique<JPH::ScaledShapeSettings>(m_collider->GetShapeSettings(), ToJolt(scale)));
	}

	ScaledCollider3D::~ScaledCollider3D()
	{
		// We have to destroy shape settings first as it carries references on the inner collider
		ResetShapeSettings();
	}

	void ScaledCollider3D::BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const
	{
		const JPH::ScaledShapeSettings* settings = GetShapeSettingsAs<JPH::ScaledShapeSettings>();

		m_collider->BuildDebugMesh(vertices, indices, offsetMatrix * Matrix4f::Scale(FromJolt(settings->mScale)));
	}

	ColliderType3D ScaledCollider3D::GetType() const
	{
		return ColliderType3D::ScaledDecoration;
	}

	/******************************** TranslatedRotatedCollider3D *********************************/

	TranslatedRotatedCollider3D::TranslatedRotatedCollider3D(std::shared_ptr<Collider3D> collider, const Vector3f& translation, const Quaternionf& rotation) :
	m_collider(std::move(collider))
	{
		SetupShapeSettings(std::make_unique<JPH::RotatedTranslatedShapeSettings>(ToJolt(translation), ToJolt(rotation), m_collider->GetShapeSettings()));
	}

	TranslatedRotatedCollider3D::~TranslatedRotatedCollider3D()
	{
		// We have to destroy shape settings first as it carries references on the inner collider
		ResetShapeSettings();
	}

	void TranslatedRotatedCollider3D::BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const
	{
		const JPH::RotatedTranslatedShapeSettings* settings = GetShapeSettingsAs<JPH::RotatedTranslatedShapeSettings>();

		m_collider->BuildDebugMesh(vertices, indices, offsetMatrix * Matrix4f::Transform(FromJolt(settings->mPosition), FromJolt(settings->mRotation)));
	}

	ColliderType3D TranslatedRotatedCollider3D::GetType() const
	{
		return ColliderType3D::TranslatedRotatedDecoration;
	}
}
