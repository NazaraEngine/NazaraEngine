// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GEOM_HPP
#define NAZARA_GEOM_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/PrimitiveList.hpp>
#include <Nazara/Core/NonCopyable.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Physics/Enums.hpp>

///TODO: CollisionModifier
///TODO: HeightfieldGeom
///TODO: PlaneGeom ?
///TODO: SceneGeom
///TODO: TreeGeom

class NzPhysWorld;
struct NewtonCollision;

class NAZARA_API NzBaseGeom : NzNonCopyable
{
	public:
		NzBaseGeom(NzPhysWorld* physWorld);
		virtual ~NzBaseGeom();

		virtual NzBoxf ComputeAABB(const NzVector3f& translation, const NzQuaternionf& rotation, const NzVector3f& scale) const;
		virtual NzBoxf ComputeAABB(const NzMatrix4f& offsetMatrix = NzMatrix4f::Identity()) const;
		virtual void ComputeInertialMatrix(NzVector3f* inertia, NzVector3f* center) const;
		virtual float ComputeVolume() const;

		NewtonCollision* GetHandle() const;
		virtual nzGeomType GetType() const = 0;

		NzPhysWorld* GetWorld() const;

		static NzBaseGeom* Build(NzPhysWorld* physWorld, const NzPrimitiveList& list);

	protected:
		NewtonCollision* m_collision;
		NzPhysWorld* m_world;
};

class NAZARA_API NzBoxGeom : public NzBaseGeom
{
	public:
		NzBoxGeom(NzPhysWorld* physWorld, const NzVector3f& lengths, const NzMatrix4f& transformMatrix = NzMatrix4f::Identity());
		NzBoxGeom(NzPhysWorld* physWorld, const NzVector3f& lengths, const NzVector3f& translation, const NzQuaternionf& rotation = NzQuaternionf::Identity());

		NzVector3f GetLengths() const;
		nzGeomType GetType() const override;

	private:
		NzVector3f m_lengths;
};

class NAZARA_API NzCapsuleGeom : public NzBaseGeom
{
	public:
		NzCapsuleGeom(NzPhysWorld* physWorld, float length, float radius, const NzMatrix4f& transformMatrix = NzMatrix4f::Identity());
		NzCapsuleGeom(NzPhysWorld* physWorld, float length, float radius, const NzVector3f& translation, const NzQuaternionf& rotation = NzQuaternionf::Identity());

		float GetLength() const;
		float GetRadius() const;
		nzGeomType GetType() const override;

	private:
		float m_length;
		float m_radius;
};

class NAZARA_API NzCompoundGeom : public NzBaseGeom
{
	public:
		NzCompoundGeom(NzPhysWorld* physWorld, NzBaseGeom** geoms, unsigned int geomCount);

		nzGeomType GetType() const override;
};

class NAZARA_API NzConeGeom : public NzBaseGeom
{
	public:
		NzConeGeom(NzPhysWorld* physWorld, float length, float radius, const NzMatrix4f& transformMatrix = NzMatrix4f::Identity());
		NzConeGeom(NzPhysWorld* physWorld, float length, float radius, const NzVector3f& translation, const NzQuaternionf& rotation = NzQuaternionf::Identity());

		float GetLength() const;
		float GetRadius() const;
		nzGeomType GetType() const override;

	private:
		float m_length;
		float m_radius;
};

class NAZARA_API NzConvexHullGeom : public NzBaseGeom
{
	public:
		NzConvexHullGeom(NzPhysWorld* physWorld, const void* vertices, unsigned int vertexCount, unsigned int stride = sizeof(NzVector3f), float tolerance = 0.002f, const NzMatrix4f& transformMatrix = NzMatrix4f::Identity());
		NzConvexHullGeom(NzPhysWorld* physWorld, const void* vertices, unsigned int vertexCount, unsigned int stride, float tolerance, const NzVector3f& translation, const NzQuaternionf& rotation = NzQuaternionf::Identity());

		nzGeomType GetType() const override;
};

class NAZARA_API NzCylinderGeom : public NzBaseGeom
{
	public:
		NzCylinderGeom(NzPhysWorld* physWorld, float length, float radius, const NzMatrix4f& transformMatrix = NzMatrix4f::Identity());
		NzCylinderGeom(NzPhysWorld* physWorld, float length, float radius, const NzVector3f& translation, const NzQuaternionf& rotation = NzQuaternionf::Identity());

		float GetLength() const;
		float GetRadius() const;
		nzGeomType GetType() const override;

	private:
		float m_length;
		float m_radius;
};

class NAZARA_API NzNullGeom : public NzBaseGeom
{
	public:
		NzNullGeom(NzPhysWorld* physWorld);

		nzGeomType GetType() const override;
};

class NAZARA_API NzSphereGeom : public NzBaseGeom
{
	public:
		NzSphereGeom(NzPhysWorld* physWorld, float radius, const NzMatrix4f& transformMatrix = NzMatrix4f::Identity());
		NzSphereGeom(NzPhysWorld* physWorld, float radius, const NzVector3f& translation, const NzQuaternionf& rotation = NzQuaternionf::Identity());

		NzVector3f GetRadius() const;
		nzGeomType GetType() const override;

	private:
		NzVector3f m_radius;
};

#endif // NAZARA_PHYSWORLD_HPP
