// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_COLLIDER3D_HPP
#define NAZARA_COLLIDER3D_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/PrimitiveList.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Physics3D/Config.hpp>
#include <Nazara/Physics3D/Enums.hpp>
#include <unordered_map>

class NewtonCollision;

namespace Nz
{
	///TODO: CollisionModifier
	///TODO: HeightfieldGeom
	///TODO: PlaneGeom ?
	///TODO: SceneGeom
	///TODO: TreeGeom

	class Collider3D;
	class PhysWorld;

	using Collider3DConstRef = ObjectRef<const Collider3D>;
	using Collider3DLibrary = ObjectLibrary<Collider3D>;
	using Collider3DRef = ObjectRef<Collider3D>;

	class NAZARA_PHYSICS3D_API Collider3D : public RefCounted
	{
		friend Collider3DLibrary;
		friend class Physics;

		public:
			Collider3D() = default;
			Collider3D(const Collider3D&) = delete;
			Collider3D(Collider3D&&) = delete;
			virtual ~Collider3D();

			Boxf ComputeAABB(const Vector3f& translation, const Quaternionf& rotation, const Vector3f& scale) const;
			virtual Boxf ComputeAABB(const Matrix4f& offsetMatrix = Matrix4f::Identity(), const Vector3f& scale = Vector3f::Unit()) const;
			virtual void ComputeInertialMatrix(Vector3f* inertia, Vector3f* center) const;
			virtual float ComputeVolume() const;

			NewtonCollision* GetHandle(PhysWorld* world) const;
			virtual GeomType GetType() const = 0;

			Collider3D& operator=(const Collider3D&) = delete;
			Collider3D& operator=(Collider3D&&) = delete;

			static Collider3DRef Build(const PrimitiveList& list);

			// Signals:
			NazaraSignal(OnColliderRelease, const Collider3D* /*collider*/);

		protected:
			virtual NewtonCollision* CreateHandle(PhysWorld* world) const = 0;

			static bool Initialize();
			static void Uninitialize();

			mutable std::unordered_map<PhysWorld*, NewtonCollision*> m_handles;

			static Collider3DLibrary::LibraryMap s_library;
	};

	class BoxGeom;

	using BoxGeomConstRef = ObjectRef<const BoxGeom>;
	using BoxGeomRef = ObjectRef<BoxGeom>;

	class NAZARA_PHYSICS3D_API BoxGeom : public Collider3D
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

	class NAZARA_PHYSICS3D_API CapsuleGeom : public Collider3D
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

	class NAZARA_PHYSICS3D_API CompoundGeom : public Collider3D
	{
		public:
			CompoundGeom(Collider3D** geoms, std::size_t geomCount);

			const std::vector<Collider3DRef>& GetGeoms() const;
			GeomType GetType() const override;

			template<typename... Args> static CompoundGeomRef New(Args&&... args);

		private:
			NewtonCollision* CreateHandle(PhysWorld* world) const override;

			std::vector<Collider3DRef> m_geoms;
	};

	class ConeGeom;

	using ConeGeomConstRef = ObjectRef<const ConeGeom>;
	using ConeGeomRef = ObjectRef<ConeGeom>;

	class NAZARA_PHYSICS3D_API ConeGeom : public Collider3D
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

	class NAZARA_PHYSICS3D_API ConvexHullGeom : public Collider3D
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

	class NAZARA_PHYSICS3D_API CylinderGeom : public Collider3D
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

	class NAZARA_PHYSICS3D_API NullGeom : public Collider3D
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

	class NAZARA_PHYSICS3D_API SphereGeom : public Collider3D
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

#include <Nazara/Physics3D/Collider3D.inl>

#endif // NAZARA_COLLIDER3D_HPP
