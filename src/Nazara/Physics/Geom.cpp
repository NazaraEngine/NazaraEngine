// Copyright (C) 2015 J√©r√¥me Leclercq
// This file is part of the "Nazara Engine - Physics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics/Geom.hpp>
#include <Nazara/Physics/PhysWorld.hpp>
#include <Newton/Newton.h>
#include <memory>
#include <Nazara/Physics/Debug.hpp>

namespace
{
	NzPhysGeomRef CreateGeomFromPrimitive(const NzPrimitive& primitive)
	{
		switch (primitive.type)
		{
			case nzPrimitiveType_Box:
				return NzBoxGeom::New(primitive.box.lengths, primitive.matrix);

			case nzPrimitiveType_Cone:
				return NzConeGeom::New(primitive.cone.length, primitive.cone.radius, primitive.matrix);

			case nzPrimitiveType_Plane:
				return NzBoxGeom::New(NzVector3f(primitive.plane.size.x, 0.01f, primitive.plane.size.y), primitive.matrix);
				///TODO: PlaneGeom?

			case nzPrimitiveType_Sphere:
				return NzSphereGeom::New(primitive.sphere.size, primitive.matrix.GetTranslation());
		}

		NazaraError("Primitive type not handled (0x" + NzString::Number(primitive.type, 16) + ')');
		return NzPhysGeomRef();
	}
}

NzPhysGeom::~NzPhysGeom()
{
	for (auto& pair : m_handles)
		NewtonDestroyCollision(pair.second);
}

NzBoxf NzPhysGeom::ComputeAABB(const NzVector3f& translation, const NzQuaternionf& rotation, const NzVector3f& scale) const
{
	return ComputeAABB(NzMatrix4f::Transform(translation, rotation), scale);
}

NzBoxf NzPhysGeom::ComputeAABB(const NzMatrix4f& offsetMatrix, const NzVector3f& scale) const
{
	NzVector3f min, max;

	// Si nous n'avons aucune instance, nous en crÈons une temporaire
	if (m_handles.empty())
	{
		NzPhysWorld world;

		NewtonCollision* collision = CreateHandle(&world);
		{
			NewtonCollisionCalculateAABB(collision, offsetMatrix, min, max);
		}
		NewtonDestroyCollision(collision);
	}
	else // Sinon on utilise une instance au hasard (elles sont toutes identiques de toute faÁon)
		NewtonCollisionCalculateAABB(m_handles.begin()->second, offsetMatrix, min, max);

	return NzBoxf(scale * min, scale * max);
}

void NzPhysGeom::ComputeInertialMatrix(NzVector3f* inertia, NzVector3f* center) const
{
	float inertiaMatrix[3];
	float origin[3];

	// Si nous n'avons aucune instance, nous en crÈons une temporaire
	if (m_handles.empty())
	{
		NzPhysWorld world;

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

float NzPhysGeom::ComputeVolume() const
{
	float volume;

	// Si nous n'avons aucune instance, nous en crÈons une temporaire
	if (m_handles.empty())
	{
		NzPhysWorld world;

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

NewtonCollision* NzPhysGeom::GetHandle(NzPhysWorld* world) const
{
	auto it = m_handles.find(world);
	if (it == m_handles.end())
		it = m_handles.insert(std::make_pair(world, CreateHandle(world))).first;

	return it->second;
}

NzPhysGeomRef NzPhysGeom::Build(const NzPrimitiveList& list)
{
	unsigned int primitiveCount = list.GetSize();

	#if NAZARA_PHYSICS_SAFE
	if (primitiveCount == 0)
	{
		NazaraError("PrimitiveList must have at least one primitive");
		return nullptr;
	}
	#endif

	if (primitiveCount > 1)
	{
		std::vector<NzPhysGeom*> geoms(primitiveCount);

		for (unsigned int i = 0; i < primitiveCount; ++i)
			geoms[i] = CreateGeomFromPrimitive(list.GetPrimitive(i));

		return NzCompoundGeom::New(&geoms[0], primitiveCount);
	}
	else
		return CreateGeomFromPrimitive(list.GetPrimitive(0));
}

NzPhysGeomLibrary::LibraryMap NzPhysGeom::s_library;

/********************************** BoxGeom **********************************/

NzBoxGeom::NzBoxGeom(const NzVector3f& lengths, const NzMatrix4f& transformMatrix) :
m_matrix(transformMatrix),
m_lengths(lengths)
{
}

NzBoxGeom::NzBoxGeom(const NzVector3f& lengths, const NzVector3f& translation, const NzQuaternionf& rotation) :
NzBoxGeom(lengths, NzMatrix4f::Transform(translation, rotation))
{
}

NzBoxf NzBoxGeom::ComputeAABB(const NzMatrix4f& offsetMatrix, const NzVector3f& scale) const
{
	NzVector3f halfLengths(m_lengths * 0.5f);

	NzBoxf aabb(-halfLengths.x, -halfLengths.y, -halfLengths.z, m_lengths.x, m_lengths.y, m_lengths.z);
	aabb.Transform(offsetMatrix, true);
	aabb *= scale;

	return aabb;
}

float NzBoxGeom::ComputeVolume() const
{
	return m_lengths.x * m_lengths.y * m_lengths.z;
}

NzVector3f NzBoxGeom::GetLengths() const
{
	return m_lengths;
}

nzGeomType NzBoxGeom::GetType() const
{
	return nzGeomType_Box;
}

NewtonCollision* NzBoxGeom::CreateHandle(NzPhysWorld* world) const
{
	return NewtonCreateBox(world->GetHandle(), m_lengths.x, m_lengths.y, m_lengths.z, 0, m_matrix);
}

/******************************** CapsuleGeom ********************************/

NzCapsuleGeom::NzCapsuleGeom(float length, float radius, const NzMatrix4f& transformMatrix) :
m_matrix(transformMatrix),
m_length(length),
m_radius(radius)
{
}

NzCapsuleGeom::NzCapsuleGeom(float length, float radius, const NzVector3f& translation, const NzQuaternionf& rotation) :
NzCapsuleGeom(length, radius, NzMatrix4f::Transform(translation, rotation))
{
}

float NzCapsuleGeom::GetLength() const
{
	return m_length;
}

float NzCapsuleGeom::GetRadius() const
{
	return m_radius;
}

nzGeomType NzCapsuleGeom::GetType() const
{
	return nzGeomType_Capsule;
}

NewtonCollision* NzCapsuleGeom::CreateHandle(NzPhysWorld* world) const
{
	return NewtonCreateCapsule(world->GetHandle(), m_radius, m_length, 0, m_matrix);
}

/******************************* CompoundGeom ********************************/

NzCompoundGeom::NzCompoundGeom(NzPhysGeom** geoms, unsigned int geomCount)
{
	m_geoms.reserve(geomCount);
	for (unsigned int i = 0; i < geomCount; ++i)
		m_geoms.emplace_back(geoms[i]);
}

const std::vector<NzPhysGeomRef>& NzCompoundGeom::GetGeoms() const
{
	return m_geoms;
}

nzGeomType NzCompoundGeom::GetType() const
{
	return nzGeomType_Compound;
}

NewtonCollision* NzCompoundGeom::CreateHandle(NzPhysWorld* world) const
{
	NewtonCollision* compoundCollision = NewtonCreateCompoundCollision(world->GetHandle(), 0);

	NewtonCompoundCollisionBeginAddRemove(compoundCollision);
	for (const NzPhysGeomRef& geom : m_geoms)
	{
		if (geom->GetType() == nzGeomType_Compound)
		{
			NzCompoundGeom* compoundGeom = static_cast<NzCompoundGeom*>(geom.Get());
			for (const NzPhysGeomRef& piece : compoundGeom->GetGeoms())
				NewtonCompoundCollisionAddSubCollision(compoundCollision, piece->GetHandle(world));
		}
		else
			NewtonCompoundCollisionAddSubCollision(compoundCollision, geom->GetHandle(world));
	}
	NewtonCompoundCollisionEndAddRemove(compoundCollision);

	return compoundCollision;
}

/********************************* ConeGeom **********************************/

NzConeGeom::NzConeGeom(float length, float radius, const NzMatrix4f& transformMatrix) :
m_matrix(transformMatrix),
m_length(length),
m_radius(radius)
{
}

NzConeGeom::NzConeGeom(float length, float radius, const NzVector3f& translation, const NzQuaternionf& rotation) :
NzConeGeom(length, radius, NzMatrix4f::Transform(translation, rotation))
{
}

float NzConeGeom::GetLength() const
{
	return m_length;
}

float NzConeGeom::GetRadius() const
{
	return m_radius;
}

nzGeomType NzConeGeom::GetType() const
{
	return nzGeomType_Cone;
}

NewtonCollision* NzConeGeom::CreateHandle(NzPhysWorld* world) const
{
	return NewtonCreateCone(world->GetHandle(), m_radius, m_length, 0, m_matrix);
}

/****************************** ConvexHullGeom *******************************/

NzConvexHullGeom::NzConvexHullGeom(const void* vertices, unsigned int vertexCount, unsigned int stride, float tolerance, const NzMatrix4f& transformMatrix) :
m_matrix(transformMatrix),
m_tolerance(tolerance),
m_vertexStride(stride)
{
	const nzUInt8* ptr = static_cast<const nzUInt8*>(vertices);

	m_vertices.resize(vertexCount);
	if (stride != sizeof(NzVector3f))
	{
		for (unsigned int i = 0; i < vertexCount; ++i)
			m_vertices[i] = *reinterpret_cast<const NzVector3f*>(ptr + stride*i);
	}
	else // Fast path
		std::memcpy(m_vertices.data(), vertices, vertexCount*sizeof(NzVector3f));
}

NzConvexHullGeom::NzConvexHullGeom(const void* vertices, unsigned int vertexCount, unsigned int stride, float tolerance, const NzVector3f& translation, const NzQuaternionf& rotation) :
NzConvexHullGeom(vertices, vertexCount, stride, tolerance, NzMatrix4f::Transform(translation, rotation))
{
}

nzGeomType NzConvexHullGeom::GetType() const
{
	return nzGeomType_Compound;
}

NewtonCollision* NzConvexHullGeom::CreateHandle(NzPhysWorld* world) const
{
	return NewtonCreateConvexHull(world->GetHandle(), m_vertices.size(), reinterpret_cast<const float*>(m_vertices.data()), sizeof(NzVector3f), m_tolerance, 0, m_matrix);
}

/******************************* CylinderGeom ********************************/

NzCylinderGeom::NzCylinderGeom(float length, float radius, const NzMatrix4f& transformMatrix) :
m_matrix(transformMatrix),
m_length(length),
m_radius(radius)
{
}

NzCylinderGeom::NzCylinderGeom(float length, float radius, const NzVector3f& translation, const NzQuaternionf& rotation) :
NzCylinderGeom(length, radius, NzMatrix4f::Transform(translation, rotation))
{
}

float NzCylinderGeom::GetLength() const
{
	return m_length;
}

float NzCylinderGeom::GetRadius() const
{
	return m_radius;
}

nzGeomType NzCylinderGeom::GetType() const
{
	return nzGeomType_Cylinder;
}

NewtonCollision* NzCylinderGeom::CreateHandle(NzPhysWorld* world) const
{
	return NewtonCreateCylinder(world->GetHandle(), m_radius, m_length, 0, m_matrix);
}

/********************************* NullGeom **********************************/

NzNullGeom::NzNullGeom()
{
}

nzGeomType NzNullGeom::GetType() const
{
	return nzGeomType_Null;
}

NewtonCollision* NzNullGeom::CreateHandle(NzPhysWorld* world) const
{
	return NewtonCreateNull(world->GetHandle());
}

/******************************** SphereGeom *********************************/

NzSphereGeom::NzSphereGeom(float radius, const NzMatrix4f& transformMatrix) :
NzSphereGeom(radius, transformMatrix.GetTranslation())
{
}

NzSphereGeom::NzSphereGeom(float radius, const NzVector3f& translation, const NzQuaternionf& rotation) :
m_position(translation),
m_radius(radius)
{
	NazaraUnused(rotation);
}

NzBoxf NzSphereGeom::ComputeAABB(const NzMatrix4f& offsetMatrix, const NzVector3f& scale) const
{
	NzVector3f size(m_radius * scale);
	NzVector3f position(offsetMatrix.GetTranslation());

	return NzBoxf(position - size, position + size);
}

float NzSphereGeom::ComputeVolume() const
{
	return M_PI * m_radius * m_radius * m_radius / 3.f;
}

float NzSphereGeom::GetRadius() const
{
	return m_radius;
}

nzGeomType NzSphereGeom::GetType() const
{
	return nzGeomType_Sphere;
}

NewtonCollision* NzSphereGeom::CreateHandle(NzPhysWorld* world) const
{
	return NewtonCreateSphere(world->GetHandle(), m_radius, 0, NzMatrix4f::Translate(m_position));
}
