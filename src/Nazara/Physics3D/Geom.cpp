// Copyright (C) 2015 J√©r√¥me Leclercq
// This file is part of the "Nazara Engine - Physics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics3D/Geom.hpp>
#include <Nazara/Physics3D/PhysWorld.hpp>
#include <Newton/Newton.h>
#include <memory>
#include <Nazara/Physics3D/Debug.hpp>

namespace Nz
{
	namespace
	{
		PhysGeomRef CreateGeomFromPrimitive(const Primitive& primitive)
		{
			switch (primitive.type)
			{
				case PrimitiveType_Box:
					return BoxGeom::New(primitive.box.lengths, primitive.matrix);

				case PrimitiveType_Cone:
					return ConeGeom::New(primitive.cone.length, primitive.cone.radius, primitive.matrix);

				case PrimitiveType_Plane:
					return BoxGeom::New(Vector3f(primitive.plane.size.x, 0.01f, primitive.plane.size.y), primitive.matrix);
					///TODO: PlaneGeom?

				case PrimitiveType_Sphere:
					return SphereGeom::New(primitive.sphere.size, primitive.matrix.GetTranslation());
			}

			NazaraError("Primitive type not handled (0x" + String::Number(primitive.type, 16) + ')');
			return PhysGeomRef();
		}
	}

	PhysGeom::~PhysGeom()
	{
		for (auto& pair : m_handles)
			NewtonDestroyCollision(pair.second);
	}

	Boxf PhysGeom::ComputeAABB(const Vector3f& translation, const Quaternionf& rotation, const Vector3f& scale) const
	{
		return ComputeAABB(Matrix4f::Transform(translation, rotation), scale);
	}

	Boxf PhysGeom::ComputeAABB(const Matrix4f& offsetMatrix, const Vector3f& scale) const
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

	void PhysGeom::ComputeInertialMatrix(Vector3f* inertia, Vector3f* center) const
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

	float PhysGeom::ComputeVolume() const
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

	NewtonCollision* PhysGeom::GetHandle(PhysWorld* world) const
	{
		auto it = m_handles.find(world);
		if (it == m_handles.end())
			it = m_handles.insert(std::make_pair(world, CreateHandle(world))).first;

		return it->second;
	}

	PhysGeomRef PhysGeom::Build(const PrimitiveList& list)
	{
		std::size_t primitiveCount = list.GetSize();
		if (primitiveCount > 1)
		{
			std::vector<PhysGeom*> geoms(primitiveCount);

			for (unsigned int i = 0; i < primitiveCount; ++i)
				geoms[i] = CreateGeomFromPrimitive(list.GetPrimitive(i));

			return CompoundGeom::New(&geoms[0], primitiveCount);
		}
		else if (primitiveCount > 0)
			return CreateGeomFromPrimitive(list.GetPrimitive(0));
		else
			return NullGeom::New();
	}

	bool PhysGeom::Initialize()
	{
		if (!PhysGeomLibrary::Initialize())
		{
			NazaraError("Failed to initialise library");
			return false;
		}

		return true;
	}

	void PhysGeom::Uninitialize()
	{
		PhysGeomLibrary::Uninitialize();
	}

	PhysGeomLibrary::LibraryMap PhysGeom::s_library;

	/********************************** BoxGeom **********************************/

	BoxGeom::BoxGeom(const Vector3f& lengths, const Matrix4f& transformMatrix) :
	m_matrix(transformMatrix),
	m_lengths(lengths)
	{
	}

	BoxGeom::BoxGeom(const Vector3f& lengths, const Vector3f& translation, const Quaternionf& rotation) :
	BoxGeom(lengths, Matrix4f::Transform(translation, rotation))
	{
	}

	Boxf BoxGeom::ComputeAABB(const Matrix4f& offsetMatrix, const Vector3f& scale) const
	{
		Vector3f halfLengths(m_lengths * 0.5f);

		Boxf aabb(-halfLengths.x, -halfLengths.y, -halfLengths.z, m_lengths.x, m_lengths.y, m_lengths.z);
		aabb.Transform(offsetMatrix, true);
		aabb *= scale;

		return aabb;
	}

	float BoxGeom::ComputeVolume() const
	{
		return m_lengths.x * m_lengths.y * m_lengths.z;
	}

	Vector3f BoxGeom::GetLengths() const
	{
		return m_lengths;
	}

	GeomType BoxGeom::GetType() const
	{
		return GeomType_Box;
	}

	NewtonCollision* BoxGeom::CreateHandle(PhysWorld* world) const
	{
		return NewtonCreateBox(world->GetHandle(), m_lengths.x, m_lengths.y, m_lengths.z, 0, m_matrix);
	}

	/******************************** CapsuleGeom ********************************/

	CapsuleGeom::CapsuleGeom(float length, float radius, const Matrix4f& transformMatrix) :
	m_matrix(transformMatrix),
	m_length(length),
	m_radius(radius)
	{
	}

	CapsuleGeom::CapsuleGeom(float length, float radius, const Vector3f& translation, const Quaternionf& rotation) :
	CapsuleGeom(length, radius, Matrix4f::Transform(translation, rotation))
	{
	}

	float CapsuleGeom::GetLength() const
	{
		return m_length;
	}

	float CapsuleGeom::GetRadius() const
	{
		return m_radius;
	}

	GeomType CapsuleGeom::GetType() const
	{
		return GeomType_Capsule;
	}

	NewtonCollision* CapsuleGeom::CreateHandle(PhysWorld* world) const
	{
		return NewtonCreateCapsule(world->GetHandle(), m_radius, m_length, 0, m_matrix);
	}

	/******************************* CompoundGeom ********************************/

	CompoundGeom::CompoundGeom(PhysGeom** geoms, std::size_t geomCount)
	{
		m_geoms.reserve(geomCount);
		for (std::size_t i = 0; i < geomCount; ++i)
			m_geoms.emplace_back(geoms[i]);
	}

	const std::vector<PhysGeomRef>& CompoundGeom::GetGeoms() const
	{
		return m_geoms;
	}

	GeomType CompoundGeom::GetType() const
	{
		return GeomType_Compound;
	}

	NewtonCollision* CompoundGeom::CreateHandle(PhysWorld* world) const
	{
		NewtonCollision* compoundCollision = NewtonCreateCompoundCollision(world->GetHandle(), 0);

		NewtonCompoundCollisionBeginAddRemove(compoundCollision);
		for (const PhysGeomRef& geom : m_geoms)
		{
			if (geom->GetType() == GeomType_Compound)
			{
				CompoundGeom* compoundGeom = static_cast<CompoundGeom*>(geom.Get());
				for (const PhysGeomRef& piece : compoundGeom->GetGeoms())
					NewtonCompoundCollisionAddSubCollision(compoundCollision, piece->GetHandle(world));
			}
			else
				NewtonCompoundCollisionAddSubCollision(compoundCollision, geom->GetHandle(world));
		}
		NewtonCompoundCollisionEndAddRemove(compoundCollision);

		return compoundCollision;
	}

	/********************************* ConeGeom **********************************/

	ConeGeom::ConeGeom(float length, float radius, const Matrix4f& transformMatrix) :
	m_matrix(transformMatrix),
	m_length(length),
	m_radius(radius)
	{
	}

	ConeGeom::ConeGeom(float length, float radius, const Vector3f& translation, const Quaternionf& rotation) :
	ConeGeom(length, radius, Matrix4f::Transform(translation, rotation))
	{
	}

	float ConeGeom::GetLength() const
	{
		return m_length;
	}

	float ConeGeom::GetRadius() const
	{
		return m_radius;
	}

	GeomType ConeGeom::GetType() const
	{
		return GeomType_Cone;
	}

	NewtonCollision* ConeGeom::CreateHandle(PhysWorld* world) const
	{
		return NewtonCreateCone(world->GetHandle(), m_radius, m_length, 0, m_matrix);
	}

	/****************************** ConvexHullGeom *******************************/

	ConvexHullGeom::ConvexHullGeom(const void* vertices, unsigned int vertexCount, unsigned int stride, float tolerance, const Matrix4f& transformMatrix) :
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

	ConvexHullGeom::ConvexHullGeom(const void* vertices, unsigned int vertexCount, unsigned int stride, float tolerance, const Vector3f& translation, const Quaternionf& rotation) :
	ConvexHullGeom(vertices, vertexCount, stride, tolerance, Matrix4f::Transform(translation, rotation))
	{
	}

	GeomType ConvexHullGeom::GetType() const
	{
		return GeomType_Compound;
	}

	NewtonCollision* ConvexHullGeom::CreateHandle(PhysWorld* world) const
	{
		return NewtonCreateConvexHull(world->GetHandle(), static_cast<int>(m_vertices.size()), reinterpret_cast<const float*>(m_vertices.data()), sizeof(Vector3f), m_tolerance, 0, m_matrix);
	}

	/******************************* CylinderGeom ********************************/

	CylinderGeom::CylinderGeom(float length, float radius, const Matrix4f& transformMatrix) :
	m_matrix(transformMatrix),
	m_length(length),
	m_radius(radius)
	{
	}

	CylinderGeom::CylinderGeom(float length, float radius, const Vector3f& translation, const Quaternionf& rotation) :
	CylinderGeom(length, radius, Matrix4f::Transform(translation, rotation))
	{
	}

	float CylinderGeom::GetLength() const
	{
		return m_length;
	}

	float CylinderGeom::GetRadius() const
	{
		return m_radius;
	}

	GeomType CylinderGeom::GetType() const
	{
		return GeomType_Cylinder;
	}

	NewtonCollision* CylinderGeom::CreateHandle(PhysWorld* world) const
	{
		return NewtonCreateCylinder(world->GetHandle(), m_radius, m_length, 0, m_matrix);
	}

	/********************************* NullGeom **********************************/

	NullGeom::NullGeom()
	{
	}

	GeomType NullGeom::GetType() const
	{
		return GeomType_Null;
	}

	void NullGeom::ComputeInertialMatrix(Vector3f* inertia, Vector3f* center) const
	{
		if (inertia)
			inertia->MakeUnit();

		if (center)
			center->MakeZero();
	}

	NewtonCollision* NullGeom::CreateHandle(PhysWorld* world) const
	{
		return NewtonCreateNull(world->GetHandle());
	}

	/******************************** SphereGeom *********************************/

	SphereGeom::SphereGeom(float radius, const Matrix4f& transformMatrix) :
	SphereGeom(radius, transformMatrix.GetTranslation())
	{
	}

	SphereGeom::SphereGeom(float radius, const Vector3f& translation, const Quaternionf& rotation) :
	m_position(translation),
	m_radius(radius)
	{
		NazaraUnused(rotation);
	}

	Boxf SphereGeom::ComputeAABB(const Matrix4f& offsetMatrix, const Vector3f& scale) const
	{
		Vector3f size(m_radius * NazaraSuffixMacro(M_SQRT3, f) * scale);
		Vector3f position(offsetMatrix.GetTranslation());

		return Boxf(position - size, position + size);
	}

	float SphereGeom::ComputeVolume() const
	{
		return float(M_PI) * m_radius * m_radius * m_radius / 3.f;
	}

	float SphereGeom::GetRadius() const
	{
		return m_radius;
	}

	GeomType SphereGeom::GetType() const
	{
		return GeomType_Sphere;
	}

	NewtonCollision* SphereGeom::CreateHandle(PhysWorld* world) const
	{
		return NewtonCreateSphere(world->GetHandle(), m_radius, 0, Matrix4f::Translate(m_position));
	}
}
