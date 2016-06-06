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

class NewtonCollision;

namespace Nz
{
	///TODO: CollisionModifier
	///TODO: HeightfieldGeom
	///TODO: PlaneGeom ?
	///TODO: SceneGeom
	///TODO: TreeGeom

	class PhysGeom;
	class PhysWorld;

	using PhysGeomConstRef = ObjectRef<const PhysGeom>;
	using PhysGeomLibrary = ObjectLibrary<PhysGeom>;
	using PhysGeomRef = ObjectRef<PhysGeom>;

	class NAZARA_PHYSICS_API PhysGeom : public RefCounted
	{
		friend PhysGeomLibrary;
		friend class Physics;

		public:
			PhysGeom() = default;
			PhysGeom(const PhysGeom&) = delete;
			PhysGeom(PhysGeom&&) = delete;
			virtual ~PhysGeom();

			Boxf ComputeAABB(const Vector3f& translation, const Quaternionf& rotation, const Vector3f& scale) const;
			virtual Boxf ComputeAABB(const Matrix4f& offsetMatrix = Matrix4f::Identity(), const Vector3f& scale = Vector3f::Unit()) const;
			virtual void ComputeInertialMatrix(Vector3f* inertia, Vector3f* center) const;
			virtual float ComputeVolume() const;

			NewtonCollision* GetHandle(PhysWorld* world) const;
			virtual GeomType GetType() const = 0;

			PhysGeom& operator=(const PhysGeom&) = delete;
			PhysGeom& operator=(PhysGeom&&) = delete;

			static PhysGeomRef Build(const PrimitiveList& list);

			// Signals:
			NazaraSignal(OnPhysGeomRelease, const PhysGeom* /*physGeom*/);

		protected:
			virtual NewtonCollision* CreateHandle(PhysWorld* world) const = 0;

			static bool Initialize();
			static void Uninitialize();

			mutable std::unordered_map<PhysWorld*, NewtonCollision*> m_handles;

			static PhysGeomLibrary::LibraryMap s_library;
	};

	class BoxGeom;

	using BoxGeomConstRef = ObjectRef<const BoxGeom>;
	using BoxGeomRef = ObjectRef<BoxGeom>;

	class NAZARA_PHYSICS_API BoxGeom : public PhysGeom
	{
		public:
			BoxGeom(const Vector3f& lengths, const Matrix4f& transformMatrix = Matrix4f::Identity());
			BoxGeom(const Vector3f& lengths, const Vector3f& translation, const Quaternionf& rotation = Quaternionf::Identity());

			Boxf ComputeAABB(const Matrix4f& offsetMatrix = Matrix4f::Identity(), const Vector3f& scale = Vector3f::Unit()) const override;
			float ComputeVolume() const override;

			Vector3f GetLengths() const;
			GeomType GetType() const override;

			template<typename... Args> static BoxGeomRef New(Args&&... args);

		private:
			NewtonCollision* CreateHandle(PhysWorld* world) const override;

			Matrix4f m_matrix;
			Vector3f m_lengths;
	};

	class CapsuleGeom;

	using CapsuleGeomConstRef = ObjectRef<const CapsuleGeom>;
	using CapsuleGeomRef = ObjectRef<CapsuleGeom>;

	class NAZARA_PHYSICS_API CapsuleGeom : public PhysGeom
	{
		public:
			CapsuleGeom(float length, float radius, const Matrix4f& transformMatrix = Matrix4f::Identity());
			CapsuleGeom(float length, float radius, const Vector3f& translation, const Quaternionf& rotation = Quaternionf::Identity());

			float GetLength() const;
			float GetRadius() const;
			GeomType GetType() const override;

			template<typename... Args> static CapsuleGeomRef New(Args&&... args);

		private:
			NewtonCollision* CreateHandle(PhysWorld* world) const override;

			Matrix4f m_matrix;
			float m_length;
			float m_radius;
	};

	class CompoundGeom;

	using CompoundGeomConstRef = ObjectRef<const CompoundGeom>;
	using CompoundGeomRef = ObjectRef<CompoundGeom>;

	class NAZARA_PHYSICS_API CompoundGeom : public PhysGeom
	{
		public:
			CompoundGeom(PhysGeom** geoms, unsigned int geomCount);

			const std::vector<PhysGeomRef>& GetGeoms() const;
			GeomType GetType() const override;

			template<typename... Args> static CompoundGeomRef New(Args&&... args);

		private:
			NewtonCollision* CreateHandle(PhysWorld* world) const override;

			std::vector<PhysGeomRef> m_geoms;
	};

	class ConeGeom;

	using ConeGeomConstRef = ObjectRef<const ConeGeom>;
	using ConeGeomRef = ObjectRef<ConeGeom>;

	class NAZARA_PHYSICS_API ConeGeom : public PhysGeom
	{
		public:
			ConeGeom(float length, float radius, const Matrix4f& transformMatrix = Matrix4f::Identity());
			ConeGeom(float length, float radius, const Vector3f& translation, const Quaternionf& rotation = Quaternionf::Identity());

			float GetLength() const;
			float GetRadius() const;
			GeomType GetType() const override;

			template<typename... Args> static ConeGeomRef New(Args&&... args);

		private:
			NewtonCollision* CreateHandle(PhysWorld* world) const override;

			Matrix4f m_matrix;
			float m_length;
			float m_radius;
	};

	class ConvexHullGeom;

	using ConvexHullGeomConstRef = ObjectRef<const ConvexHullGeom>;
	using ConvexHullGeomRef = ObjectRef<ConvexHullGeom>;

	class NAZARA_PHYSICS_API ConvexHullGeom : public PhysGeom
	{
		public:
			ConvexHullGeom(const void* vertices, unsigned int vertexCount, unsigned int stride = sizeof(Vector3f), float tolerance = 0.002f, const Matrix4f& transformMatrix = Matrix4f::Identity());
			ConvexHullGeom(const void* vertices, unsigned int vertexCount, unsigned int stride, float tolerance, const Vector3f& translation, const Quaternionf& rotation = Quaternionf::Identity());

			GeomType GetType() const override;

			template<typename... Args> static ConvexHullGeomRef New(Args&&... args);

		private:
			NewtonCollision* CreateHandle(PhysWorld* world) const override;

			std::vector<Vector3f> m_vertices;
			Matrix4f m_matrix;
			float m_tolerance;
			unsigned int m_vertexStride;
	};

	class CylinderGeom;

	using CylinderGeomConstRef = ObjectRef<const CylinderGeom>;
	using CylinderGeomRef = ObjectRef<CylinderGeom>;

	class NAZARA_PHYSICS_API CylinderGeom : public PhysGeom
	{
		public:
			CylinderGeom(float length, float radius, const Matrix4f& transformMatrix = Matrix4f::Identity());
			CylinderGeom(float length, float radius, const Vector3f& translation, const Quaternionf& rotation = Quaternionf::Identity());

			float GetLength() const;
			float GetRadius() const;
			GeomType GetType() const override;

			template<typename... Args> static CylinderGeomRef New(Args&&... args);

		private:
			NewtonCollision* CreateHandle(PhysWorld* world) const override;

			Matrix4f m_matrix;
			float m_length;
			float m_radius;
	};

	class NullGeom;

	using NullGeomConstRef = ObjectRef<const NullGeom>;
	using NullGeomRef = ObjectRef<NullGeom>;

	class NAZARA_PHYSICS_API NullGeom : public PhysGeom
	{
		public:
			NullGeom();

			void ComputeInertialMatrix(Vector3f* inertia, Vector3f* center) const override;

			GeomType GetType() const override;

			template<typename... Args> static NullGeomRef New(Args&&... args);

		private:
			NewtonCollision* CreateHandle(PhysWorld* world) const override;
	};

	class SphereGeom;

	using SphereGeomConstRef = ObjectRef<const SphereGeom>;
	using SphereGeomRef = ObjectRef<SphereGeom>;

	class NAZARA_PHYSICS_API SphereGeom : public PhysGeom
	{
		public:
			SphereGeom(float radius, const Matrix4f& transformMatrix = Matrix4f::Identity());
			SphereGeom(float radius, const Vector3f& translation, const Quaternionf& rotation = Quaternionf::Identity());

			Boxf ComputeAABB(const Matrix4f& offsetMatrix = Matrix4f::Identity(), const Vector3f& scale = Vector3f::Unit()) const override;
			float ComputeVolume() const override;

			float GetRadius() const;
			GeomType GetType() const override;

			template<typename... Args> static SphereGeomRef New(Args&&... args);

		private:
			NewtonCollision* CreateHandle(PhysWorld* world) const override;

			Vector3f m_position;
			float m_radius;
	};
}

#include <Nazara/Physics/Geom.inl>

#endif // NAZARA_PHYSWORLD_HPP
