// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GEOM_HPP
#define NAZARA_GEOM_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/NonCopyable.hpp>
#include <Nazara/Core/PrimitiveList.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/ObjectListenerWrapper.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Physics/Enums.hpp>

///TODO: CollisionModifier
///TODO: HeightfieldGeom
///TODO: PlaneGeom ?
///TODO: SceneGeom
///TODO: TreeGeom

class NzPhysGeom;
class NzPhysWorld;
struct NewtonCollision;

using NzPhysGeomConstListener = NzObjectListenerWrapper<const NzPhysGeom>;
using NzPhysGeomConstRef = NzObjectRef<const NzPhysGeom>;
using NzPhysGeomLibrary = NzObjectLibrary<NzPhysGeom>;
using NzPhysGeomListener = NzObjectListenerWrapper<NzPhysGeom>;
using NzPhysGeomRef = NzObjectRef<NzPhysGeom>;

class NAZARA_API NzPhysGeom : public NzRefCounted, NzNonCopyable
{
	public:
		NzPhysGeom(NzPhysWorld* physWorld);
		virtual ~NzPhysGeom();

		virtual NzBoxf ComputeAABB(const NzVector3f& translation, const NzQuaternionf& rotation, const NzVector3f& scale) const;
		virtual NzBoxf ComputeAABB(const NzMatrix4f& offsetMatrix = NzMatrix4f::Identity()) const;
		virtual void ComputeInertialMatrix(NzVector3f* inertia, NzVector3f* center) const;
		virtual float ComputeVolume() const;

		NewtonCollision* GetHandle() const;
		virtual nzGeomType GetType() const = 0;

		NzPhysWorld* GetWorld() const;

		static NzPhysGeomRef Build(NzPhysWorld* physWorld, const NzPrimitiveList& list);

	protected:
		NewtonCollision* m_collision;
		NzPhysWorld* m_world;

		static NzPhysGeomLibrary::LibraryMap s_library;
};

class NzBoxGeom;

using NzBoxGeomConstListener = NzObjectListenerWrapper<const NzBoxGeom>;
using NzBoxGeomConstRef = NzObjectRef<const NzBoxGeom>;
using NzBoxGeomListener = NzObjectListenerWrapper<NzBoxGeom>;
using NzBoxGeomRef = NzObjectRef<NzBoxGeom>;

class NAZARA_API NzBoxGeom : public NzPhysGeom
{
	public:
		NzBoxGeom(NzPhysWorld* physWorld, const NzVector3f& lengths, const NzMatrix4f& transformMatrix = NzMatrix4f::Identity());
		NzBoxGeom(NzPhysWorld* physWorld, const NzVector3f& lengths, const NzVector3f& translation, const NzQuaternionf& rotation = NzQuaternionf::Identity());

		NzVector3f GetLengths() const;
		nzGeomType GetType() const override;

		template<typename... Args> static NzBoxGeomRef New(Args&&... args);

	private:
		NzVector3f m_lengths;
};

class NzCapsuleGeom;

using NzCapsuleGeomConstListener = NzObjectListenerWrapper<const NzCapsuleGeom>;
using NzCapsuleGeomConstRef = NzObjectRef<const NzCapsuleGeom>;
using NzCapsuleGeomListener = NzObjectListenerWrapper<NzCapsuleGeom>;
using NzCapsuleGeomRef = NzObjectRef<NzCapsuleGeom>;

class NAZARA_API NzCapsuleGeom : public NzPhysGeom
{
	public:
		NzCapsuleGeom(NzPhysWorld* physWorld, float length, float radius, const NzMatrix4f& transformMatrix = NzMatrix4f::Identity());
		NzCapsuleGeom(NzPhysWorld* physWorld, float length, float radius, const NzVector3f& translation, const NzQuaternionf& rotation = NzQuaternionf::Identity());

		float GetLength() const;
		float GetRadius() const;
		nzGeomType GetType() const override;

		template<typename... Args> static NzCapsuleGeomRef New(Args&&... args);

	private:
		float m_length;
		float m_radius;
};

class NzCompoundGeom;

using NzCompoundGeomConstListener = NzObjectListenerWrapper<const NzCompoundGeom>;
using NzCompoundGeomConstRef = NzObjectRef<const NzCompoundGeom>;
using NzCompoundGeomListener = NzObjectListenerWrapper<NzCompoundGeom>;
using NzCompoundGeomRef = NzObjectRef<NzCompoundGeom>;

class NAZARA_API NzCompoundGeom : public NzPhysGeom
{
	public:
		NzCompoundGeom(NzPhysWorld* physWorld, NzPhysGeom** geoms, unsigned int geomCount);

		const std::vector<NzPhysGeomRef>& GetGeoms() const;
		nzGeomType GetType() const override;

		template<typename... Args> static NzCompoundGeomRef New(Args&&... args);

	private:
		std::vector<NzPhysGeomRef> m_geoms;
};

class NzConeGeom;

using NzConeGeomConstListener = NzObjectListenerWrapper<const NzConeGeom>;
using NzConeGeomConstRef = NzObjectRef<const NzConeGeom>;
using NzConeGeomListener = NzObjectListenerWrapper<NzConeGeom>;
using NzConeGeomRef = NzObjectRef<NzConeGeom>;

class NAZARA_API NzConeGeom : public NzPhysGeom
{
	public:
		NzConeGeom(NzPhysWorld* physWorld, float length, float radius, const NzMatrix4f& transformMatrix = NzMatrix4f::Identity());
		NzConeGeom(NzPhysWorld* physWorld, float length, float radius, const NzVector3f& translation, const NzQuaternionf& rotation = NzQuaternionf::Identity());

		float GetLength() const;
		float GetRadius() const;
		nzGeomType GetType() const override;

		template<typename... Args> static NzConeGeomRef New(Args&&... args);

	private:
		float m_length;
		float m_radius;
};

class NzConvexHullGeom;

using NzConvexHullGeomConstListener = NzObjectListenerWrapper<const NzConvexHullGeom>;
using NzConvexHullGeomConstRef = NzObjectRef<const NzConvexHullGeom>;
using NzConvexHullGeomListener = NzObjectListenerWrapper<NzConvexHullGeom>;
using NzConvexHullGeomRef = NzObjectRef<NzConvexHullGeom>;

class NAZARA_API NzConvexHullGeom : public NzPhysGeom
{
	public:
		NzConvexHullGeom(NzPhysWorld* physWorld, const void* vertices, unsigned int vertexCount, unsigned int stride = sizeof(NzVector3f), float tolerance = 0.002f, const NzMatrix4f& transformMatrix = NzMatrix4f::Identity());
		NzConvexHullGeom(NzPhysWorld* physWorld, const void* vertices, unsigned int vertexCount, unsigned int stride, float tolerance, const NzVector3f& translation, const NzQuaternionf& rotation = NzQuaternionf::Identity());

		nzGeomType GetType() const override;

		template<typename... Args> static NzConvexHullGeomRef New(Args&&... args);
};

class NzCylinderGeom;

using NzCylinderGeomConstListener = NzObjectListenerWrapper<const NzCylinderGeom>;
using NzCylinderGeomConstRef = NzObjectRef<const NzCylinderGeom>;
using NzCylinderGeomListener = NzObjectListenerWrapper<NzCylinderGeom>;
using NzCylinderGeomRef = NzObjectRef<NzCylinderGeom>;

class NAZARA_API NzCylinderGeom : public NzPhysGeom
{
	public:
		NzCylinderGeom(NzPhysWorld* physWorld, float length, float radius, const NzMatrix4f& transformMatrix = NzMatrix4f::Identity());
		NzCylinderGeom(NzPhysWorld* physWorld, float length, float radius, const NzVector3f& translation, const NzQuaternionf& rotation = NzQuaternionf::Identity());

		float GetLength() const;
		float GetRadius() const;
		nzGeomType GetType() const override;

		template<typename... Args> static NzCylinderGeomRef New(Args&&... args);

	private:
		float m_length;
		float m_radius;
};

class NzNullGeom;

using NzNullGeomConstListener = NzObjectListenerWrapper<const NzNullGeom>;
using NzNullGeomConstRef = NzObjectRef<const NzNullGeom>;
using NzNullGeomListener = NzObjectListenerWrapper<NzNullGeom>;
using NzNullGeomRef = NzObjectRef<NzNullGeom>;

class NAZARA_API NzNullGeom : public NzPhysGeom
{
	public:
		NzNullGeom(NzPhysWorld* physWorld);

		nzGeomType GetType() const override;

		template<typename... Args> static NzNullGeomRef New(Args&&... args);
};

class NzSphereGeom;

using NzSphereGeomConstListener = NzObjectListenerWrapper<const NzSphereGeom>;
using NzSphereGeomConstRef = NzObjectRef<const NzSphereGeom>;
using NzSphereGeomListener = NzObjectListenerWrapper<NzSphereGeom>;
using NzSphereGeomRef = NzObjectRef<NzSphereGeom>;

class NAZARA_API NzSphereGeom : public NzPhysGeom
{
	public:
		NzSphereGeom(NzPhysWorld* physWorld, float radius, const NzMatrix4f& transformMatrix = NzMatrix4f::Identity());
		NzSphereGeom(NzPhysWorld* physWorld, float radius, const NzVector3f& translation, const NzQuaternionf& rotation = NzQuaternionf::Identity());

		NzVector3f GetRadius() const;
		nzGeomType GetType() const override;

		template<typename... Args> static NzSphereGeomRef New(Args&&... args);

	private:
		NzVector3f m_radius;
};

#include <Nazara/Physics/Geom.inl>

#endif // NAZARA_PHYSWORLD_HPP
