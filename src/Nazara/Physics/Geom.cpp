// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics/Geom.hpp>
#include <Nazara/Physics/PhysWorld.hpp>
#include <Newton/Newton.h>
#include <memory>
#include <Nazara/Physics/Debug.hpp>

namespace
{
	NzBaseGeom* CreateGeomFromPrimitive(NzPhysWorld* physWorld, const NzPrimitive& primitive)
	{
		switch (primitive.type)
		{
			case nzPrimitiveType_Box:
				return new NzBoxGeom(physWorld, primitive.box.lengths, primitive.matrix);

			case nzPrimitiveType_Plane:
				return new NzBoxGeom(physWorld, NzVector3f(primitive.plane.size.x, 0.01f, primitive.plane.size.y), primitive.matrix);
				///TODO: PlaneGeom?

			case nzPrimitiveType_Sphere:
				return new NzSphereGeom(physWorld, primitive.sphere.size, primitive.matrix.GetTranslation());
		}

		NazaraError("Primitive type not handled (0x" + NzString::Number(primitive.type, 16) + ')');
		return nullptr;
	}
}

NzBaseGeom::NzBaseGeom(NzPhysWorld* physWorld) :
m_world(physWorld)
{
}

NzBaseGeom::~NzBaseGeom()
{
	NewtonDestroyCollision(m_collision);
}

NzBoxf NzBaseGeom::ComputeAABB(const NzVector3f& translation, const NzQuaternionf& rotation, const NzVector3f& scale) const
{
	NzVector3f min, max;
	NewtonCollisionCalculateAABB(m_collision, NzMatrix4f::Transform(translation, rotation), min, max);

	// Et on applique le scale à la fin
	return NzBoxf(scale*min, scale*max);
}

NzBoxf NzBaseGeom::ComputeAABB(const NzMatrix4f& offsetMatrix) const
{
	NzVector3f min, max;
	NewtonCollisionCalculateAABB(m_collision, offsetMatrix, min, max);

	return NzBoxf(min, max);
}

void NzBaseGeom::ComputeInertialMatrix(NzVector3f* inertia, NzVector3f* center) const
{
	float inertiaMatrix[3];
	float origin[3];

	NewtonConvexCollisionCalculateInertialMatrix(m_collision, inertiaMatrix, origin);

	if (inertia)
		inertia->Set(inertiaMatrix);

	if (center)
		center->Set(origin);
}

float NzBaseGeom::ComputeVolume() const
{
	return NewtonConvexCollisionCalculateVolume(m_collision);
}

NewtonCollision* NzBaseGeom::GetHandle() const
{
	return m_collision;
}

NzPhysWorld* NzBaseGeom::GetWorld() const
{
	return m_world;
}

NzBaseGeom* NzBaseGeom::Build(NzPhysWorld* physWorld, const NzPrimitiveList& list)
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
		std::vector<NzBaseGeom*> geoms(primitiveCount);

		for (unsigned int i = 0; i < primitiveCount; ++i)
			geoms[i] = CreateGeomFromPrimitive(physWorld, list.GetPrimitive(i));

		return new NzCompoundGeom(physWorld, &geoms[0], primitiveCount);
	}
	else
		return CreateGeomFromPrimitive(physWorld, list.GetPrimitive(0));
}

/********************************** BoxGeom **********************************/

NzBoxGeom::NzBoxGeom(NzPhysWorld* physWorld, const NzVector3f& lengths, const NzMatrix4f& transformMatrix) :
NzBaseGeom(physWorld),
m_lengths(lengths)
{
	m_collision = NewtonCreateBox(physWorld->GetHandle(), lengths.x, lengths.y, lengths.z, 0, transformMatrix);
}

NzBoxGeom::NzBoxGeom(NzPhysWorld* physWorld, const NzVector3f& lengths, const NzVector3f& translation, const NzQuaternionf& rotation) :
NzBoxGeom(physWorld, lengths, NzMatrix4f::Transform(translation, rotation))
{
}

NzVector3f NzBoxGeom::GetLengths() const
{
	return m_lengths;
}

nzGeomType NzBoxGeom::GetType() const
{
	return nzGeomType_Box;
}

/******************************** CapsuleGeom ********************************/

NzCapsuleGeom::NzCapsuleGeom(NzPhysWorld* physWorld, float length, float radius, const NzMatrix4f& transformMatrix) :
NzBaseGeom(physWorld),
m_length(length),
m_radius(radius)
{
	m_collision = NewtonCreateCapsule(physWorld->GetHandle(), radius, length, 0, transformMatrix);
}

NzCapsuleGeom::NzCapsuleGeom(NzPhysWorld* physWorld, float length, float radius, const NzVector3f& translation, const NzQuaternionf& rotation) :
NzCapsuleGeom(physWorld, length, radius, NzMatrix4f::Transform(translation, rotation))
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

/******************************* CompoundGeom ********************************/

NzCompoundGeom::NzCompoundGeom(NzPhysWorld* physWorld, NzBaseGeom** geoms, unsigned int geomCount) :
NzBaseGeom(physWorld)
{
	m_collision = NewtonCreateCompoundCollision(physWorld->GetHandle(), 0);
	NewtonCompoundCollisionBeginAddRemove(m_collision);

	for (unsigned int i = 0; i < geomCount; ++i)
	{
		if (geoms[i]->GetType() == nzGeomType_Compound)
			NazaraError("Cannot add compound geoms to other compound geoms");
		else
			NewtonCompoundCollisionAddSubCollision(m_collision, geoms[i]->GetHandle());
	}

	NewtonCompoundCollisionEndAddRemove(m_collision);
}

nzGeomType NzCompoundGeom::GetType() const
{
	return nzGeomType_Compound;
}

/********************************* ConeGeom **********************************/

NzConeGeom::NzConeGeom(NzPhysWorld* physWorld, float length, float radius, const NzMatrix4f& transformMatrix) :
NzBaseGeom(physWorld),
m_length(length),
m_radius(radius)
{
	m_collision = NewtonCreateCone(physWorld->GetHandle(), radius, length, 0, transformMatrix);
}

NzConeGeom::NzConeGeom(NzPhysWorld* physWorld, float length, float radius, const NzVector3f& translation, const NzQuaternionf& rotation) :
NzConeGeom(physWorld, length, radius, NzMatrix4f::Transform(translation, rotation))
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

/****************************** ConvexHullGeom *******************************/

NzConvexHullGeom::NzConvexHullGeom(NzPhysWorld* physWorld, const void* vertices, unsigned int vertexCount, unsigned int stride, float tolerance, const NzMatrix4f& transformMatrix) :
NzBaseGeom(physWorld)
{
	m_collision = NewtonCreateConvexHull(physWorld->GetHandle(), vertexCount, reinterpret_cast<const float*>(vertices), stride, tolerance, 0, transformMatrix);
}

NzConvexHullGeom::NzConvexHullGeom(NzPhysWorld* physWorld, const void* vertices, unsigned int vertexCount, unsigned int stride, float tolerance, const NzVector3f& translation, const NzQuaternionf& rotation) :
NzConvexHullGeom(physWorld, vertices, vertexCount, stride, tolerance, NzMatrix4f::Transform(translation, rotation))
{
}

nzGeomType NzConvexHullGeom::GetType() const
{
	return nzGeomType_Compound;
}

/******************************* CylinderGeom ********************************/

NzCylinderGeom::NzCylinderGeom(NzPhysWorld* physWorld, float length, float radius, const NzMatrix4f& transformMatrix) :
NzBaseGeom(physWorld),
m_length(length),
m_radius(radius)
{
	m_collision = NewtonCreateCylinder(physWorld->GetHandle(), radius, length, 0, transformMatrix);
}

NzCylinderGeom::NzCylinderGeom(NzPhysWorld* physWorld, float length, float radius, const NzVector3f& translation, const NzQuaternionf& rotation) :
NzCylinderGeom(physWorld, length, radius, NzMatrix4f::Transform(translation, rotation))
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

/********************************* NullGeom **********************************/

NzNullGeom::NzNullGeom(NzPhysWorld* physWorld) :
NzBaseGeom(physWorld)
{
	m_collision = NewtonCreateNull(physWorld->GetHandle());
}

nzGeomType NzNullGeom::GetType() const
{
	return nzGeomType_Null;
}

/******************************** SphereGeom *********************************/

NzSphereGeom::NzSphereGeom(NzPhysWorld* physWorld, float radius, const NzMatrix4f& transformMatrix) :
NzBaseGeom(physWorld),
m_radius(radius)
{
	m_collision = NewtonCreateSphere(physWorld->GetHandle(), radius, 0, transformMatrix);
}

NzSphereGeom::NzSphereGeom(NzPhysWorld* physWorld, float radius, const NzVector3f& translation, const NzQuaternionf& rotation) :
NzSphereGeom(physWorld, radius, NzMatrix4f::Transform(translation, rotation))
{
}

NzVector3f NzSphereGeom::GetRadius() const
{
	return m_radius;
}

nzGeomType NzSphereGeom::GetType() const
{
	return nzGeomType_Sphere;
}
