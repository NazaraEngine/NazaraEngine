// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics/Geom.hpp>
#include <Nazara/Physics/PhysWorld.hpp>
#include <Newton/Newton.h>
#include <memory>
#include <Nazara/Physics/Debug.hpp>

NzBaseGeom::NzBaseGeom(NzPhysWorld* physWorld) :
m_world(physWorld)
{
}

NzBaseGeom::~NzBaseGeom()
{
	NewtonReleaseCollision(m_world->GetHandle(), m_collision);
}

NzCubef NzBaseGeom::ComputeAABB(const NzVector3f& translation, const NzQuaternionf& rotation, const NzVector3f& scale) const
{
	NzVector3f min, max;
	NewtonCollisionCalculateAABB(m_collision, NzMatrix4f::Transform(translation, rotation), min, max);

	// Et on applique le scale à la fin
	return NzCubef(scale*min, scale*max);
}

NzCubef NzBaseGeom::ComputeAABB(const NzMatrix4f& offsetMatrix) const
{
	NzVector3f min, max;
	NewtonCollisionCalculateAABB(m_collision, offsetMatrix, min, max);

	return NzCubef(min, max);
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

/********************************** BoxGeom **********************************/

NzBoxGeom::NzBoxGeom(NzPhysWorld* physWorld, const NzVector3f& lengths, const NzVector3f& translation, const NzQuaternionf& rotation) :
NzBaseGeom(physWorld),
m_lengths(lengths)
{
	m_collision = NewtonCreateBox(physWorld->GetHandle(), lengths.x, lengths.y, lengths.z, 0, NzMatrix4f::Transform(translation, rotation));
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

NzCapsuleGeom::NzCapsuleGeom(NzPhysWorld* physWorld, float length, float radius, const NzVector3f& translation, const NzQuaternionf& rotation) :
NzBaseGeom(physWorld),
m_length(length),
m_radius(radius)
{
	m_collision = NewtonCreateCapsule(physWorld->GetHandle(), radius, length, 0, NzMatrix4f::Transform(translation, rotation));
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

NzCompoundGeom::NzCompoundGeom(NzPhysWorld* physWorld, NzBaseGeom* geoms, unsigned int geomCount) :
NzBaseGeom(physWorld)
{
	std::vector<NewtonCollision*> collisions;
	collisions.reserve(geomCount);

	for (unsigned int i = 0; i < geomCount; ++i)
	{
		if (geoms[i].GetType() == nzGeomType_Compound)
		{
			NewtonCollisionInfoRecord info;
			NewtonCollisionGetInfo(geoms[i].GetHandle(), &info);

			unsigned int count = info.m_compoundCollision.m_chidrenCount;
			for (unsigned int j = 0; j < count; ++j)
				collisions.push_back(info.m_compoundCollision.m_chidren[j]);
		}
		else
			collisions.push_back(geoms[i].GetHandle());
	}

	m_collision = NewtonCreateCompoundCollision(physWorld->GetHandle(), collisions.size(), &collisions[0], 0);
}

nzGeomType NzCompoundGeom::GetType() const
{
	return nzGeomType_Compound;
}

/********************************* ConeGeom **********************************/

NzConeGeom::NzConeGeom(NzPhysWorld* physWorld, float length, float radius, const NzVector3f& translation, const NzQuaternionf& rotation) :
NzBaseGeom(physWorld),
m_length(length),
m_radius(radius)
{
	m_collision = NewtonCreateCone(physWorld->GetHandle(), radius, length, 0, NzMatrix4f::Transform(translation, rotation));
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

NzConvexHullGeom::NzConvexHullGeom(NzPhysWorld* physWorld, const NzVector3f* vertices, unsigned int vertexCount, unsigned int stride, float tolerance, const NzVector3f& translation, const NzQuaternionf& rotation) :
NzBaseGeom(physWorld)
{
	m_collision = NewtonCreateConvexHull(physWorld->GetHandle(), vertexCount, reinterpret_cast<const float*>(vertices), stride, tolerance, 0, NzMatrix4f::Transform(translation, rotation));
}

nzGeomType NzConvexHullGeom::GetType() const
{
	return nzGeomType_Compound;
}

/******************************* CylinderGeom ********************************/

NzCylinderGeom::NzCylinderGeom(NzPhysWorld* physWorld, float length, float radius, const NzVector3f& translation, const NzQuaternionf& rotation) :
NzBaseGeom(physWorld),
m_length(length),
m_radius(radius)
{
	m_collision = NewtonCreateCylinder(physWorld->GetHandle(), radius, length, 0, NzMatrix4f::Transform(translation, rotation));
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

NzSphereGeom::NzSphereGeom(NzPhysWorld* physWorld, const NzVector3f& radius, const NzVector3f& translation) :
NzBaseGeom(physWorld),
m_radius(radius)
{
	m_collision = NewtonCreateSphere(physWorld->GetHandle(), radius.x, radius.y, radius.z, 0, NzMatrix4f::Translate(translation));
}

NzSphereGeom::NzSphereGeom(NzPhysWorld* physWorld, float radius, const NzVector3f& translation) :
NzSphereGeom(physWorld, NzVector3f(radius), translation)
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
