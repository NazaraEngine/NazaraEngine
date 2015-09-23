// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GEOM_HPP
#define NAZARA_GEOM_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/PrimitiveList.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Physics/Config.hpp>
#include <Nazara/Physics/Enums.hpp>
#include <unordered_map>

///TODO: CollisionModifier
///TODO: HeightfieldGeom
///TODO: PlaneGeom ?
///TODO: SceneGeom
///TODO: TreeGeom

class NzPhysGeom;
class NzPhysWorld;
struct NewtonCollision;

using NzPhysGeomConstRef = NzObjectRef<const NzPhysGeom>;
using NzPhysGeomLibrary = NzObjectLibrary<NzPhysGeom>;
using NzPhysGeomRef = NzObjectRef<NzPhysGeom>;

class NAZARA_PHYSICS_API NzPhysGeom : public NzRefCounted
{
	public:
		NzPhysGeom() = default;
        NzPhysGeom(const NzPhysGeom&) = delete;
        NzPhysGeom(NzPhysGeom&&) = delete;
		virtual ~NzPhysGeom();

		NzBoxf ComputeAABB(const NzVector3f& translation, const NzQuaternionf& rotation, const NzVector3f& scale) const;
		virtual NzBoxf ComputeAABB(const NzMatrix4f& offsetMatrix = NzMatrix4f::Identity(), const NzVector3f& scale = NzVector3f::Unit()) const;
		virtual void ComputeInertialMatrix(NzVector3f* inertia, NzVector3f* center) const;
		virtual float ComputeVolume() const;

		NewtonCollision* GetHandle(NzPhysWorld* world) const;
		virtual nzGeomType GetType() const = 0;

        NzPhysGeom& operator=(const NzPhysGeom&) = delete;
        NzPhysGeom& operator=(NzPhysGeom&&) = delete;

		static NzPhysGeomRef Build(const NzPrimitiveList& list);

		// Signals:
		NazaraSignal(OnPhysGeomRelease, const NzPhysGeom* /*physGeom*/);

	protected:
		virtual NewtonCollision* CreateHandle(NzPhysWorld* world) const = 0;

		mutable std::unordered_map<NzPhysWorld*, NewtonCollision*> m_handles;

		static NzPhysGeomLibrary::LibraryMap s_library;
};

class NzBoxGeom;

using NzBoxGeomConstRef = NzObjectRef<const NzBoxGeom>;
using NzBoxGeomRef = NzObjectRef<NzBoxGeom>;

class NAZARA_PHYSICS_API NzBoxGeom : public NzPhysGeom
{
	public:
		NzBoxGeom(const NzVector3f& lengths, const NzMatrix4f& transformMatrix = NzMatrix4f::Identity());
		NzBoxGeom(const NzVector3f& lengths, const NzVector3f& translation, const NzQuaternionf& rotation = NzQuaternionf::Identity());

		NzBoxf ComputeAABB(const NzMatrix4f& offsetMatrix = NzMatrix4f::Identity(), const NzVector3f& scale = NzVector3f::Unit()) const override;
		float ComputeVolume() const override;

		NzVector3f GetLengths() const;
		nzGeomType GetType() const override;

		template<typename... Args> static NzBoxGeomRef New(Args&&... args);

	private:
		NewtonCollision* CreateHandle(NzPhysWorld* world) const override;

		NzMatrix4f m_matrix;
		NzVector3f m_lengths;
};

class NzCapsuleGeom;

using NzCapsuleGeomConstRef = NzObjectRef<const NzCapsuleGeom>;
using NzCapsuleGeomRef = NzObjectRef<NzCapsuleGeom>;

class NAZARA_PHYSICS_API NzCapsuleGeom : public NzPhysGeom
{
	public:
		NzCapsuleGeom(float length, float radius, const NzMatrix4f& transformMatrix = NzMatrix4f::Identity());
		NzCapsuleGeom(float length, float radius, const NzVector3f& translation, const NzQuaternionf& rotation = NzQuaternionf::Identity());

		float GetLength() const;
		float GetRadius() const;
		nzGeomType GetType() const override;

		template<typename... Args> static NzCapsuleGeomRef New(Args&&... args);

	private:
		NewtonCollision* CreateHandle(NzPhysWorld* world) const override;

		NzMatrix4f m_matrix;
		float m_length;
		float m_radius;
};

class NzCompoundGeom;

using NzCompoundGeomConstRef = NzObjectRef<const NzCompoundGeom>;
using NzCompoundGeomRef = NzObjectRef<NzCompoundGeom>;

class NAZARA_PHYSICS_API NzCompoundGeom : public NzPhysGeom
{
	public:
		NzCompoundGeom(NzPhysGeom** geoms, unsigned int geomCount);

		const std::vector<NzPhysGeomRef>& GetGeoms() const;
		nzGeomType GetType() const override;

		template<typename... Args> static NzCompoundGeomRef New(Args&&... args);

	private:
		NewtonCollision* CreateHandle(NzPhysWorld* world) const override;

		std::vector<NzPhysGeomRef> m_geoms;
};

class NzConeGeom;

using NzConeGeomConstRef = NzObjectRef<const NzConeGeom>;
using NzConeGeomRef = NzObjectRef<NzConeGeom>;

class NAZARA_PHYSICS_API NzConeGeom : public NzPhysGeom
{
	public:
		NzConeGeom(float length, float radius, const NzMatrix4f& transformMatrix = NzMatrix4f::Identity());
		NzConeGeom(float length, float radius, const NzVector3f& translation, const NzQuaternionf& rotation = NzQuaternionf::Identity());

		float GetLength() const;
		float GetRadius() const;
		nzGeomType GetType() const override;

		template<typename... Args> static NzConeGeomRef New(Args&&... args);

	private:
		NewtonCollision* CreateHandle(NzPhysWorld* world) const override;

		NzMatrix4f m_matrix;
		float m_length;
		float m_radius;
};

class NzConvexHullGeom;

using NzConvexHullGeomConstRef = NzObjectRef<const NzConvexHullGeom>;
using NzConvexHullGeomRef = NzObjectRef<NzConvexHullGeom>;

class NAZARA_PHYSICS_API NzConvexHullGeom : public NzPhysGeom
{
	public:
		NzConvexHullGeom(const void* vertices, unsigned int vertexCount, unsigned int stride = sizeof(NzVector3f), float tolerance = 0.002f, const NzMatrix4f& transformMatrix = NzMatrix4f::Identity());
		NzConvexHullGeom(const void* vertices, unsigned int vertexCount, unsigned int stride, float tolerance, const NzVector3f& translation, const NzQuaternionf& rotation = NzQuaternionf::Identity());

		nzGeomType GetType() const override;

		template<typename... Args> static NzConvexHullGeomRef New(Args&&... args);

	private:
		NewtonCollision* CreateHandle(NzPhysWorld* world) const override;

		std::vector<NzVector3f> m_vertices;
		NzMatrix4f m_matrix;
		float m_tolerance;
		unsigned int m_vertexStride;
};

class NzCylinderGeom;

using NzCylinderGeomConstRef = NzObjectRef<const NzCylinderGeom>;
using NzCylinderGeomRef = NzObjectRef<NzCylinderGeom>;

class NAZARA_PHYSICS_API NzCylinderGeom : public NzPhysGeom
{
	public:
		NzCylinderGeom(float length, float radius, const NzMatrix4f& transformMatrix = NzMatrix4f::Identity());
		NzCylinderGeom(float length, float radius, const NzVector3f& translation, const NzQuaternionf& rotation = NzQuaternionf::Identity());

		float GetLength() const;
		float GetRadius() const;
		nzGeomType GetType() const override;

		template<typename... Args> static NzCylinderGeomRef New(Args&&... args);

	private:
		NewtonCollision* CreateHandle(NzPhysWorld* world) const override;

		NzMatrix4f m_matrix;
		float m_length;
		float m_radius;
};

class NzNullGeom;

using NzNullGeomConstRef = NzObjectRef<const NzNullGeom>;
using NzNullGeomRef = NzObjectRef<NzNullGeom>;

class NAZARA_PHYSICS_API NzNullGeom : public NzPhysGeom
{
	public:
		NzNullGeom();

		void ComputeInertialMatrix(NzVector3f* inertia, NzVector3f* center) const;

		nzGeomType GetType() const override;

		template<typename... Args> static NzNullGeomRef New(Args&&... args);

	private:
		NewtonCollision* CreateHandle(NzPhysWorld* world) const override;
};

class NzSphereGeom;

using NzSphereGeomConstRef = NzObjectRef<const NzSphereGeom>;
using NzSphereGeomRef = NzObjectRef<NzSphereGeom>;

class NAZARA_PHYSICS_API NzSphereGeom : public NzPhysGeom
{
	public:
		NzSphereGeom(float radius, const NzMatrix4f& transformMatrix = NzMatrix4f::Identity());
		NzSphereGeom(float radius, const NzVector3f& translation, const NzQuaternionf& rotation = NzQuaternionf::Identity());

		NzBoxf ComputeAABB(const NzMatrix4f& offsetMatrix = NzMatrix4f::Identity(), const NzVector3f& scale = NzVector3f::Unit()) const override;
		float ComputeVolume() const override;

		float GetRadius() const;
		nzGeomType GetType() const override;

		template<typename... Args> static NzSphereGeomRef New(Args&&... args);

	private:
		NewtonCollision* CreateHandle(NzPhysWorld* world) const override;

		NzVector3f m_position;
		float m_radius;
};

#include <Nazara/Physics/Geom.inl>

#endif // NAZARA_PHYSWORLD_HPP
