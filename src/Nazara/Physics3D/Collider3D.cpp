// Copyright (C) 2015 J√©r√¥me Leclercq
// This file is part of the "Nazara Engine - Physics 3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics3D/Collider3D.hpp>
#include <Nazara/Physics3D/PhysWorld.hpp>
#include <Newton/Newton.h>
#include <memory>
#include <Nazara/Physics3D/Debug.hpp>

namespace Nz
{
	namespace
	{
		Collider3DRef CreateGeomFromPrimitive(const Primitive& primitive)
		{
			switch (primitive.type)
			{
				case PrimitiveType_Box:
					return BoxCollider3D::New(primitive.box.lengths, primitive.matrix);

				case PrimitiveType_Cone:
					return ConeCollider3D::New(primitive.cone.length, primitive.cone.radius, primitive.matrix);

				case PrimitiveType_Plane:
					return BoxCollider3D::New(Vector3f(primitive.plane.size.x, 0.01f, primitive.plane.size.y), primitive.matrix);
					///TODO: PlaneGeom?

				case PrimitiveType_Sphere:
					return SphereCollider3D::New(primitive.sphere.size, primitive.matrix.GetTranslation());
			}

			NazaraError("Primitive type not handled (0x" + String::Number(primitive.type, 16) + ')');
			return Collider3DRef();
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

		// Si nous n'avons aucune instance, nous en crÈons une temporaire
		if (m_handles.empty())
		{
			PhysWorld world;

			NewtonCollision* collision = CreateHandle(&world);
			{
				NewtonCollisionCalculateAABB(collision, offsetMatrix, min, max);
			}
			NewtonDestroyCollision(collision);
		}
		else // Sinon on utilise une instance au hasard (elles sont toutes identiques de toute faÁon)
			NewtonCollisionCalculateAABB(m_handles.begin()->second, offsetMatrix, min, max);

		return Boxf(scale * min, scale * max);
	}

	void Collider3D::ComputeInertialMatrix(Vector3f* inertia, Vector3f* center) const
	{
		float inertiaMatrix[3];
		float origin[3];

		// Si nous n'avons aucune instance, nous en crÈons une temporaire
		if (m_handles.empty())
		{
			PhysWorld world;

			NewtonCollision* collision = CreateHandle(&world);
			{
				NewtonConvexCollisionCalculateInertialMatrix(collision, inertiaMatrix, origin);
			}
			NewtonDestroyCollision(collision);
		}
		else // Sinon on utilise une instance au hasard (elles sont toutes identiques de toute faÁon)
			NewtonConvexCollisionCalculateInertialMatrix(m_handles.begin()->second, inertiaMatrix, origin);

		if (inertia)
			inertia->Set(inertiaMatrix);

		if (center)
			center->Set(origin);
	}

	float Collider3D::ComputeVolume() const
	{
		float volume;

		// Si nous n'avons aucune instance, nous en crÈons une temporaire
		if (m_handles.empty())
		{
			PhysWorld world;

			NewtonCollision* collision = CreateHandle(&world);
			{
				volume = NewtonConvexCollisionCalculateVolume(collision);
			}
			NewtonDestroyCollision(collision);
		}
		else // Sinon on utilise une instance au hasard (elles sont toutes identiques de toute faÁon)
			volume = NewtonConvexCollisionCalculateVolume(m_handles.begin()->second);

		return volume;
	}

	NewtonCollision* Collider3D::GetHandle(PhysWorld* world) const
	{
		auto it = m_handles.find(world);
		if (it == m_handles.end())
			it = m_handles.insert(std::make_pair(world, CreateHandle(world))).first;

		return it->second;
	}

	Collider3DRef Collider3D::Build(const PrimitiveList& list)
	{
		std::size_t primitiveCount = list.GetSize();
		if (primitiveCount > 1)
		{
			std::vector<Collider3D*> geoms(primitiveCount);

			for (unsigned int i = 0; i < primitiveCount; ++i)
				geoms[i] = CreateGeomFromPrimitive(list.GetPrimitive(i));

			return CompoundCollider3D::New(&geoms[0], primitiveCount);
		}
		else if (primitiveCount > 0)
			return CreateGeomFromPrimitive(list.GetPrimitive(0));
		else
			return NullCollider3D::New();
	}

	bool Collider3D::Initialize()
	{
		if (!Collider3DLibrary::Initialize())
		{
			NazaraError("Failed to initialise library");
			return false;
		}

		return true;
	}

	void Collider3D::Uninitialize()
	{
		Collider3DLibrary::Uninitialize();
	}

	Collider3DLibrary::LibraryMap Collider3D::s_library;

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

	GeomType BoxCollider3D::GetType() const
	{
		return GeomType_Box;
	}

	NewtonCollision* BoxCollider3D::CreateHandle(PhysWorld* world) const
	{
		return NewtonCreateBox(world->GetHandle(), m_lengths.x, m_lengths.y, m_lengths.z, 0, m_matrix);
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

	GeomType CapsuleCollider3D::GetType() const
	{
		return GeomType_Capsule;
	}

	NewtonCollision* CapsuleCollider3D::CreateHandle(PhysWorld* world) const
	{
		return NewtonCreateCapsule(world->GetHandle(), m_radius, m_length, 0, m_matrix);
	}

	/******************************* CompoundCollider3D ********************************/

	CompoundCollider3D::CompoundCollider3D(Collider3D** geoms, std::size_t geomCount)
	{
		m_geoms.reserve(geomCount);
		for (std::size_t i = 0; i < geomCount; ++i)
			m_geoms.emplace_back(geoms[i]);
	}

	const std::vector<Collider3DRef>& CompoundCollider3D::GetGeoms() const
	{
		return m_geoms;
	}

	GeomType CompoundCollider3D::GetType() const
	{
		return GeomType_Compound;
	}

	NewtonCollision* CompoundCollider3D::CreateHandle(PhysWorld* world) const
	{
		NewtonCollision* compoundCollision = NewtonCreateCompoundCollision(world->GetHandle(), 0);

		NewtonCompoundCollisionBeginAddRemove(compoundCollision);
		for (const Collider3DRef& geom : m_geoms)
		{
			if (geom->GetType() == GeomType_Compound)
			{
				CompoundCollider3D* compoundGeom = static_cast<CompoundCollider3D*>(geom.Get());
				for (const Collider3DRef& piece : compoundGeom->GetGeoms())
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

	GeomType ConeCollider3D::GetType() const
	{
		return GeomType_Cone;
	}

	NewtonCollision* ConeCollider3D::CreateHandle(PhysWorld* world) const
	{
		return NewtonCreateCone(world->GetHandle(), m_radius, m_length, 0, m_matrix);
	}

	/****************************** ConvexCollider3D *******************************/

	ConvexCollider3D::ConvexCollider3D(const void* vertices, unsigned int vertexCount, unsigned int stride, float tolerance, const Matrix4f& transformMatrix) :
	m_matrix(transformMatrix),
	m_tolerance(tolerance),
	m_vertexStride(stride)
	{
		const UInt8* ptr = static_cast<const UInt8*>(vertices);

		m_vertices.resize(vertexCount);
		if (stride != sizeof(Vector3f))
		{
			for (unsigned int i = 0; i < vertexCount; ++i)
				m_vertices[i] = *reinterpret_cast<const Vector3f*>(ptr + stride*i);
		}
		else // Fast path
			std::memcpy(m_vertices.data(), vertices, vertexCount*sizeof(Vector3f));
	}

	ConvexCollider3D::ConvexCollider3D(const void* vertices, unsigned int vertexCount, unsigned int stride, float tolerance, const Vector3f& translation, const Quaternionf& rotation) :
	ConvexCollider3D(vertices, vertexCount, stride, tolerance, Matrix4f::Transform(translation, rotation))
	{
	}

	GeomType ConvexCollider3D::GetType() const
	{
		return GeomType_Compound;
	}

	NewtonCollision* ConvexCollider3D::CreateHandle(PhysWorld* world) const
	{
		return NewtonCreateConvexHull(world->GetHandle(), static_cast<int>(m_vertices.size()), reinterpret_cast<const float*>(m_vertices.data()), sizeof(Vector3f), m_tolerance, 0, m_matrix);
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

	GeomType CylinderCollider3D::GetType() const
	{
		return GeomType_Cylinder;
	}

	NewtonCollision* CylinderCollider3D::CreateHandle(PhysWorld* world) const
	{
		return NewtonCreateCylinder(world->GetHandle(), m_radius, m_length, 0, m_matrix);
	}

	/********************************* NullCollider3D **********************************/

	NullCollider3D::NullCollider3D()
	{
	}

	GeomType NullCollider3D::GetType() const
	{
		return GeomType_Null;
	}

	void NullCollider3D::ComputeInertialMatrix(Vector3f* inertia, Vector3f* center) const
	{
		if (inertia)
			inertia->MakeUnit();

		if (center)
			center->MakeZero();
	}

	NewtonCollision* NullCollider3D::CreateHandle(PhysWorld* world) const
	{
		return NewtonCreateNull(world->GetHandle());
	}

	/******************************** SphereCollider3D *********************************/

	SphereCollider3D::SphereCollider3D(float radius, const Matrix4f& transformMatrix) :
	SphereCollider3D(radius, transformMatrix.GetTranslation())
	{
	}

	SphereCollider3D::SphereCollider3D(float radius, const Vector3f& translation, const Quaternionf& rotation) :
	m_position(translation),
	m_radius(radius)
	{
		NazaraUnused(rotation);
	}

	Boxf SphereCollider3D::ComputeAABB(const Matrix4f& offsetMatrix, const Vector3f& scale) const
	{
		Vector3f size(m_radius * NazaraSuffixMacro(M_SQRT3, f) * scale);
		Vector3f position(offsetMatrix.GetTranslation());

		return Boxf(position - size, position + size);
	}

	float SphereCollider3D::ComputeVolume() const
	{
		return float(M_PI) * m_radius * m_radius * m_radius / 3.f;
	}

	float SphereCollider3D::GetRadius() const
	{
		return m_radius;
	}

	GeomType SphereCollider3D::GetType() const
	{
		return GeomType_Sphere;
	}

	NewtonCollision* SphereCollider3D::CreateHandle(PhysWorld* world) const
	{
		return NewtonCreateSphere(world->GetHandle(), m_radius, 0, Matrix4f::Translate(m_position));
	}
}
