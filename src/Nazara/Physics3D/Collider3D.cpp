// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics3D/Collider3D.hpp>
#include <Nazara/Core/PrimitiveList.hpp>
#include <Nazara/Physics3D/PhysWorld3D.hpp>
#include <Nazara/Utility/IndexBuffer.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>
#include <newton/Newton.h>
#include <Nazara/Physics3D/Debug.hpp>

namespace Nz
{
	namespace
	{
		std::shared_ptr<Collider3D> CreateGeomFromPrimitive(const Primitive& primitive)
		{
			switch (primitive.type)
			{
				case PrimitiveType::Box:
					return std::make_shared<BoxCollider3D>(primitive.box.lengths, primitive.matrix);

				case PrimitiveType::Cone:
					return std::make_shared<ConeCollider3D>(primitive.cone.length, primitive.cone.radius, primitive.matrix);

				case PrimitiveType::Plane:
					return std::make_shared<BoxCollider3D>(Vector3f(primitive.plane.size.x, 0.01f, primitive.plane.size.y), primitive.matrix);
					///TODO: PlaneGeom?

				case PrimitiveType::Sphere:
					return std::make_shared<SphereCollider3D>(primitive.sphere.size, primitive.matrix.GetTranslation());
			}

			NazaraError("Primitive type not handled (0x" + NumberToString(UnderlyingCast(primitive.type), 16) + ')');
			return std::shared_ptr<Collider3D>();
		}
	}

	Collider3D::~Collider3D()
	{
		for (auto& pair : m_handles)
			NewtonDestroyCollision(pair.second);
	}

	Boxf Collider3D::ComputeAABB(const Vector3f& translation, const Quaternionf& rotation, const Vector3f& scale) const
	{
		return ComputeAABB(Matrix4f::Transform(translation, rotation), scale);
	}

	Boxf Collider3D::ComputeAABB(const Matrix4f& offsetMatrix, const Vector3f& scale) const
	{
		Vector3f min, max;

		// Check for existing collision handles, and create a temporary one if none is available
		if (m_handles.empty())
		{
			PhysWorld3D world;

			NewtonCollision* collision = CreateHandle(&world);
			{
				NewtonCollisionCalculateAABB(collision, &offsetMatrix.m11, &min.x, &max.x);
			}
			NewtonDestroyCollision(collision);
		}
		else
			NewtonCollisionCalculateAABB(m_handles.begin()->second, &offsetMatrix.m11, &min.x, &max.x);

		return Boxf(scale * min, scale * max);
	}

	void Collider3D::ComputeInertialMatrix(Vector3f* inertia, Vector3f* center) const
	{
		float inertiaMatrix[3];
		float origin[3];

		// Check for existing collision handles, and create a temporary one if none is available
		if (m_handles.empty())
		{
			PhysWorld3D world;

			NewtonCollision* collision = CreateHandle(&world);
			{
				NewtonConvexCollisionCalculateInertialMatrix(collision, inertiaMatrix, origin);
			}
			NewtonDestroyCollision(collision);
		}
		else
			NewtonConvexCollisionCalculateInertialMatrix(m_handles.begin()->second, inertiaMatrix, origin);

		if (inertia)
			inertia->Set(inertiaMatrix);

		if (center)
			center->Set(origin);
	}

	float Collider3D::ComputeVolume() const
	{
		float volume;

		// Check for existing collision handles, and create a temporary one if none is available
		if (m_handles.empty())
		{
			PhysWorld3D world;

			NewtonCollision* collision = CreateHandle(&world);
			{
				volume = NewtonConvexCollisionCalculateVolume(collision);
			}
			NewtonDestroyCollision(collision);
		}
		else
			volume = NewtonConvexCollisionCalculateVolume(m_handles.begin()->second);

		return volume;
	}

	void Collider3D::ForEachPolygon(const std::function<void(const Vector3f* vertices, std::size_t vertexCount)>& callback) const
	{
		auto newtCallback = [](void* const userData, int vertexCount, const dFloat* const faceArray, int /*faceId*/)
		{
			static_assert(sizeof(Vector3f) == 3 * sizeof(float), "Vector3 is expected to contain 3 floats without padding");

			const auto& cb = *static_cast<std::add_pointer_t<decltype(callback)>>(userData);
			cb(reinterpret_cast<const Vector3f*>(faceArray), vertexCount);
		};

		// Check for existing collision handles, and create a temporary one if none is available
		Matrix4f identity = Matrix4f::Identity();
		if (m_handles.empty())
		{
			PhysWorld3D world;

			NewtonCollision* collision = CreateHandle(&world);
			{
				NewtonCollisionForEachPolygonDo(collision, &identity.m11, newtCallback, const_cast<void*>(static_cast<const void*>(&callback))); //< This isn't that bad; pointer will not be used for writing
			}
			NewtonDestroyCollision(collision);
		}
		else
			NewtonCollisionForEachPolygonDo(m_handles.begin()->second, &identity.m11, newtCallback, const_cast<void*>(static_cast<const void*>(&callback))); //< This isn't that bad; pointer will not be used for writing
	}

	std::shared_ptr<StaticMesh> Collider3D::GenerateMesh() const
	{
		std::vector<Nz::Vector3f> colliderVertices;
		std::vector<Nz::UInt16> colliderIndices;

		// Generate a line list
		ForEachPolygon([&](const Nz::Vector3f* vertices, std::size_t vertexCount)
		{
			Nz::UInt16 firstIndex = colliderVertices.size();
			for (std::size_t i = 0; i < vertexCount; ++i)
				colliderVertices.push_back(vertices[i]);

			for (std::size_t i = 1; i < vertexCount; ++i)
			{
				colliderIndices.push_back(firstIndex + i - 1);
				colliderIndices.push_back(firstIndex + i);
			}

			if (vertexCount > 2)
			{
				colliderIndices.push_back(firstIndex + vertexCount - 1);
				colliderIndices.push_back(firstIndex);
			}
		});

		std::shared_ptr<Nz::VertexBuffer> colliderVB = std::make_shared<Nz::VertexBuffer>(Nz::VertexDeclaration::Get(Nz::VertexLayout::XYZ), colliderVertices.size(), Nz::DataStorage::Software, 0);
		colliderVB->Fill(colliderVertices.data(), 0, colliderVertices.size());

		std::shared_ptr<Nz::IndexBuffer> colliderIB = std::make_shared<Nz::IndexBuffer>(false, colliderIndices.size(), Nz::DataStorage::Software, 0);
		colliderIB->Fill(colliderIndices.data(), 0, colliderIndices.size());

		std::shared_ptr<Nz::StaticMesh> colliderSubMesh = std::make_shared<Nz::StaticMesh>(std::move(colliderVB), std::move(colliderIB));
		colliderSubMesh->GenerateAABB();
		colliderSubMesh->SetPrimitiveMode(Nz::PrimitiveMode::LineList);

		return colliderSubMesh;
	}

	NewtonCollision* Collider3D::GetHandle(PhysWorld3D* world) const
	{
		auto it = m_handles.find(world);
		if (it == m_handles.end())
			it = m_handles.insert(std::make_pair(world, CreateHandle(world))).first;

		return it->second;
	}

	std::shared_ptr<Collider3D> Collider3D::Build(const PrimitiveList& list)
	{
		std::size_t primitiveCount = list.GetSize();
		if (primitiveCount > 1)
		{
			std::vector<std::shared_ptr<Collider3D>> geoms(primitiveCount);

			for (unsigned int i = 0; i < primitiveCount; ++i)
				geoms[i] = CreateGeomFromPrimitive(list.GetPrimitive(i));

			return std::make_shared<CompoundCollider3D>(std::move(geoms));
		}
		else if (primitiveCount > 0)
			return CreateGeomFromPrimitive(list.GetPrimitive(0));
		else
			return std::make_shared<NullCollider3D>();
	}

	/********************************** BoxCollider3D **********************************/

	BoxCollider3D::BoxCollider3D(const Vector3f& lengths, const Matrix4f& transformMatrix) :
	m_matrix(transformMatrix),
	m_lengths(lengths)
	{
	}

	BoxCollider3D::BoxCollider3D(const Vector3f& lengths, const Vector3f& translation, const Quaternionf& rotation) :
	BoxCollider3D(lengths, Matrix4f::Transform(translation, rotation))
	{
	}

	Boxf BoxCollider3D::ComputeAABB(const Matrix4f& offsetMatrix, const Vector3f& scale) const
	{
		Vector3f halfLengths(m_lengths * 0.5f);

		Boxf aabb(-halfLengths.x, -halfLengths.y, -halfLengths.z, m_lengths.x, m_lengths.y, m_lengths.z);
		aabb.Transform(offsetMatrix, true);
		aabb *= scale;

		return aabb;
	}

	float BoxCollider3D::ComputeVolume() const
	{
		return m_lengths.x * m_lengths.y * m_lengths.z;
	}

	Vector3f BoxCollider3D::GetLengths() const
	{
		return m_lengths;
	}

	ColliderType3D BoxCollider3D::GetType() const
	{
		return ColliderType3D::Box;
	}

	NewtonCollision* BoxCollider3D::CreateHandle(PhysWorld3D* world) const
	{
		return NewtonCreateBox(world->GetHandle(), m_lengths.x, m_lengths.y, m_lengths.z, 0, &m_matrix.m11);
	}

	/******************************** CapsuleCollider3D ********************************/

	CapsuleCollider3D::CapsuleCollider3D(float length, float radius, const Matrix4f& transformMatrix) :
	m_matrix(transformMatrix),
	m_length(length),
	m_radius(radius)
	{
	}

	CapsuleCollider3D::CapsuleCollider3D(float length, float radius, const Vector3f& translation, const Quaternionf& rotation) :
	CapsuleCollider3D(length, radius, Matrix4f::Transform(translation, rotation))
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

	ColliderType3D CapsuleCollider3D::GetType() const
	{
		return ColliderType3D::Capsule;
	}

	NewtonCollision* CapsuleCollider3D::CreateHandle(PhysWorld3D* world) const
	{
		return NewtonCreateCapsule(world->GetHandle(), m_radius, m_radius, m_length, 0, &m_matrix.m11);
	}

	/******************************* CompoundCollider3D ********************************/

	CompoundCollider3D::CompoundCollider3D(std::vector<std::shared_ptr<Collider3D>> geoms) :
	m_geoms(std::move(geoms))
	{
	}

	const std::vector<std::shared_ptr<Collider3D>>& CompoundCollider3D::GetGeoms() const
	{
		return m_geoms;
	}

	ColliderType3D CompoundCollider3D::GetType() const
	{
		return ColliderType3D::Compound;
	}

	NewtonCollision* CompoundCollider3D::CreateHandle(PhysWorld3D* world) const
	{
		NewtonCollision* compoundCollision = NewtonCreateCompoundCollision(world->GetHandle(), 0);

		NewtonCompoundCollisionBeginAddRemove(compoundCollision);
		for (const std::shared_ptr<Collider3D>& geom : m_geoms)
		{
			if (geom->GetType() == ColliderType3D::Compound)
			{
				CompoundCollider3D& compoundGeom = static_cast<CompoundCollider3D&>(*geom);
				for (const std::shared_ptr<Collider3D>& piece : compoundGeom.GetGeoms())
					NewtonCompoundCollisionAddSubCollision(compoundCollision, piece->GetHandle(world));
			}
			else
				NewtonCompoundCollisionAddSubCollision(compoundCollision, geom->GetHandle(world));
		}
		NewtonCompoundCollisionEndAddRemove(compoundCollision);

		return compoundCollision;
	}

	/********************************* ConeCollider3D **********************************/

	ConeCollider3D::ConeCollider3D(float length, float radius, const Matrix4f& transformMatrix) :
	m_matrix(transformMatrix),
	m_length(length),
	m_radius(radius)
	{
	}

	ConeCollider3D::ConeCollider3D(float length, float radius, const Vector3f& translation, const Quaternionf& rotation) :
	ConeCollider3D(length, radius, Matrix4f::Transform(translation, rotation))
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

	ColliderType3D ConeCollider3D::GetType() const
	{
		return ColliderType3D::Cone;
	}

	NewtonCollision* ConeCollider3D::CreateHandle(PhysWorld3D* world) const
	{
		return NewtonCreateCone(world->GetHandle(), m_radius, m_length, 0, &m_matrix.m11);
	}

	/****************************** ConvexCollider3D *******************************/

	ConvexCollider3D::ConvexCollider3D(SparsePtr<const Vector3f> vertices, unsigned int vertexCount, float tolerance, const Matrix4f& transformMatrix) :
	m_matrix(transformMatrix),
	m_tolerance(tolerance)
	{
		m_vertices.resize(vertexCount);
		if (vertices.GetStride() != sizeof(Vector3f))
		{
			for (unsigned int i = 0; i < vertexCount; ++i)
				m_vertices[i] = *vertices++;
		}
		else // Fast path
			std::memcpy(m_vertices.data(), vertices.GetPtr(), vertexCount*sizeof(Vector3f));
	}

	ConvexCollider3D::ConvexCollider3D(SparsePtr<const Vector3f> vertices, unsigned int vertexCount, float tolerance, const Vector3f& translation, const Quaternionf& rotation) :
	ConvexCollider3D(vertices, vertexCount, tolerance, Matrix4f::Transform(translation, rotation))
	{
	}

	ColliderType3D ConvexCollider3D::GetType() const
	{
		return ColliderType3D::ConvexHull;
	}

	NewtonCollision* ConvexCollider3D::CreateHandle(PhysWorld3D* world) const
	{
		return NewtonCreateConvexHull(world->GetHandle(), static_cast<int>(m_vertices.size()), reinterpret_cast<const float*>(m_vertices.data()), sizeof(Vector3f), m_tolerance, 0, &m_matrix.m11);
	}

	/******************************* CylinderCollider3D ********************************/

	CylinderCollider3D::CylinderCollider3D(float length, float radius, const Matrix4f& transformMatrix) :
	m_matrix(transformMatrix),
	m_length(length),
	m_radius(radius)
	{
	}

	CylinderCollider3D::CylinderCollider3D(float length, float radius, const Vector3f& translation, const Quaternionf& rotation) :
	CylinderCollider3D(length, radius, Matrix4f::Transform(translation, rotation))
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

	ColliderType3D CylinderCollider3D::GetType() const
	{
		return ColliderType3D::Cylinder;
	}

	NewtonCollision* CylinderCollider3D::CreateHandle(PhysWorld3D* world) const
	{
		return NewtonCreateCylinder(world->GetHandle(), m_radius, m_radius, m_length, 0, &m_matrix.m11);
	}

	/********************************* NullCollider3D **********************************/

	NullCollider3D::NullCollider3D()
	{
	}

	ColliderType3D NullCollider3D::GetType() const
	{
		return ColliderType3D::Null;
	}

	void NullCollider3D::ComputeInertialMatrix(Vector3f* inertia, Vector3f* center) const
	{
		if (inertia)
			inertia->MakeUnit();

		if (center)
			center->MakeZero();
	}

	NewtonCollision* NullCollider3D::CreateHandle(PhysWorld3D* world) const
	{
		return NewtonCreateNull(world->GetHandle());
	}

	/******************************** SphereCollider3D *********************************/

	SphereCollider3D::SphereCollider3D(float radius, const Matrix4f& transformMatrix) :
	SphereCollider3D(radius, transformMatrix.GetTranslation())
	{
	}

	SphereCollider3D::SphereCollider3D(float radius, const Vector3f& translation, const Quaternionf& /*rotation*/) :
	m_position(translation),
	m_radius(radius)
	{
	}

	Boxf SphereCollider3D::ComputeAABB(const Matrix4f& offsetMatrix, const Vector3f& scale) const
	{
		Vector3f size(m_radius * Sqrt5<float> * scale);
		Vector3f position(offsetMatrix.GetTranslation());

		return Boxf(position - size, position + size);
	}

	float SphereCollider3D::ComputeVolume() const
	{
		return Pi<float> * m_radius * m_radius * m_radius / 3.f;
	}

	float SphereCollider3D::GetRadius() const
	{
		return m_radius;
	}

	ColliderType3D SphereCollider3D::GetType() const
	{
		return ColliderType3D::Sphere;
	}

	NewtonCollision* SphereCollider3D::CreateHandle(PhysWorld3D* world) const
	{
		Matrix4f transformMatrix = Matrix4f::Translate(m_position);
		return NewtonCreateSphere(world->GetHandle(), m_radius, 0, &transformMatrix.m11);
	}
}
