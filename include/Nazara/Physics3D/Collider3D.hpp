// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 3D module"
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
#include <Nazara/Core/SparsePtr.hpp>
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
	class PhysWorld3D;

	using Collider3DConstRef = ObjectRef<const Collider3D>;
	using Collider3DLibrary = ObjectLibrary<Collider3D>;
	using Collider3DRef = ObjectRef<Collider3D>;

	class NAZARA_PHYSICS3D_API Collider3D : public RefCounted
	{
		friend Collider3DLibrary;
		friend class Physics3D;

		public:
			Collider3D() = default;
			Collider3D(const Collider3D&) = delete;
			Collider3D(Collider3D&&) = delete;
			virtual ~Collider3D();

			Boxf ComputeAABB(const Vector3f& translation, const Quaternionf& rotation, const Vector3f& scale) const;
			virtual Boxf ComputeAABB(const Matrix4f& offsetMatrix = Matrix4f::Identity(), const Vector3f& scale = Vector3f::Unit()) const;
			virtual void ComputeInertialMatrix(Vector3f* inertia, Vector3f* center) const;
			virtual float ComputeVolume() const;

			NewtonCollision* GetHandle(PhysWorld3D* world) const;
			virtual ColliderType3D GetType() const = 0;

			Collider3D& operator=(const Collider3D&) = delete;
			Collider3D& operator=(Collider3D&&) = delete;

			static Collider3DRef Build(const PrimitiveList& list);

			// Signals:
			NazaraSignal(OnColliderRelease, const Collider3D* /*collider*/);

		protected:
			virtual NewtonCollision* CreateHandle(PhysWorld3D* world) const = 0;

			static bool Initialize();
			static void Uninitialize();

			mutable std::unordered_map<PhysWorld3D*, NewtonCollision*> m_handles;

			static Collider3DLibrary::LibraryMap s_library;
	};

	class BoxCollider3D;

	using BoxCollider3DConstRef = ObjectRef<const BoxCollider3D>;
	using BoxCollider3DRef = ObjectRef<BoxCollider3D>;

	class NAZARA_PHYSICS3D_API BoxCollider3D : public Collider3D
	{
		public:
			BoxCollider3D(const Vector3f& lengths, const Matrix4f& transformMatrix = Matrix4f::Identity());
			BoxCollider3D(const Vector3f& lengths, const Vector3f& translation, const Quaternionf& rotation = Quaternionf::Identity());

			Boxf ComputeAABB(const Matrix4f& offsetMatrix = Matrix4f::Identity(), const Vector3f& scale = Vector3f::Unit()) const override;
			float ComputeVolume() const override;

			Vector3f GetLengths() const;
			ColliderType3D GetType() const override;

			template<typename... Args> static BoxCollider3DRef New(Args&&... args);

		private:
			NewtonCollision* CreateHandle(PhysWorld3D* world) const override;

			Matrix4f m_matrix;
			Vector3f m_lengths;
	};

	class CapsuleCollider3D;

	using CapsuleCollider3DConstRef = ObjectRef<const CapsuleCollider3D>;
	using CapsuleCollider3DRef = ObjectRef<CapsuleCollider3D>;

	class NAZARA_PHYSICS3D_API CapsuleCollider3D : public Collider3D
	{
		public:
			CapsuleCollider3D(float length, float radius, const Matrix4f& transformMatrix = Matrix4f::Identity());
			CapsuleCollider3D(float length, float radius, const Vector3f& translation, const Quaternionf& rotation = Quaternionf::Identity());

			float GetLength() const;
			float GetRadius() const;
			ColliderType3D GetType() const override;

			template<typename... Args> static CapsuleCollider3DRef New(Args&&... args);

		private:
			NewtonCollision* CreateHandle(PhysWorld3D* world) const override;

			Matrix4f m_matrix;
			float m_length;
			float m_radius;
	};

	class CompoundCollider3D;

	using CompoundCollider3DConstRef = ObjectRef<const CompoundCollider3D>;
	using CompoundCollider3DRef = ObjectRef<CompoundCollider3D>;

	class NAZARA_PHYSICS3D_API CompoundCollider3D : public Collider3D
	{
		public:
			CompoundCollider3D(Collider3D** geoms, std::size_t geomCount);

			const std::vector<Collider3DRef>& GetGeoms() const;
			ColliderType3D GetType() const override;

			template<typename... Args> static CompoundCollider3DRef New(Args&&... args);

		private:
			NewtonCollision* CreateHandle(PhysWorld3D* world) const override;

			std::vector<Collider3DRef> m_geoms;
	};

	class ConeCollider3D;

	using ConeCollider3DConstRef = ObjectRef<const ConeCollider3D>;
	using ConeCollider3DRef = ObjectRef<ConeCollider3D>;

	class NAZARA_PHYSICS3D_API ConeCollider3D : public Collider3D
	{
		public:
			ConeCollider3D(float length, float radius, const Matrix4f& transformMatrix = Matrix4f::Identity());
			ConeCollider3D(float length, float radius, const Vector3f& translation, const Quaternionf& rotation = Quaternionf::Identity());

			float GetLength() const;
			float GetRadius() const;
			ColliderType3D GetType() const override;

			template<typename... Args> static ConeCollider3DRef New(Args&&... args);

		private:
			NewtonCollision* CreateHandle(PhysWorld3D* world) const override;

			Matrix4f m_matrix;
			float m_length;
			float m_radius;
	};

	class ConvexCollider3D;

	using ConvexCollider3DConstRef = ObjectRef<const ConvexCollider3D>;
	using ConvexCollider3DRef = ObjectRef<ConvexCollider3D>;

	class NAZARA_PHYSICS3D_API ConvexCollider3D : public Collider3D
	{
		public:
			ConvexCollider3D(SparsePtr<const Vector3f> vertices, unsigned int vertexCount, float tolerance = 0.002f, const Matrix4f& transformMatrix = Matrix4f::Identity());
			ConvexCollider3D(SparsePtr<const Vector3f> vertices, unsigned int vertexCount, float tolerance, const Vector3f& translation, const Quaternionf& rotation = Quaternionf::Identity());

			ColliderType3D GetType() const override;

			template<typename... Args> static ConvexCollider3DRef New(Args&&... args);

		private:
			NewtonCollision* CreateHandle(PhysWorld3D* world) const override;

			std::vector<Vector3f> m_vertices;
			Matrix4f m_matrix;
			float m_tolerance;
	};

	class CylinderCollider3D;

	using CylinderCollider3DConstRef = ObjectRef<const CylinderCollider3D>;
	using CylinderCollider3DRef = ObjectRef<CylinderCollider3D>;

	class NAZARA_PHYSICS3D_API CylinderCollider3D : public Collider3D
	{
		public:
			CylinderCollider3D(float length, float radius, const Matrix4f& transformMatrix = Matrix4f::Identity());
			CylinderCollider3D(float length, float radius, const Vector3f& translation, const Quaternionf& rotation = Quaternionf::Identity());

			float GetLength() const;
			float GetRadius() const;
			ColliderType3D GetType() const override;

			template<typename... Args> static CylinderCollider3DRef New(Args&&... args);

		private:
			NewtonCollision* CreateHandle(PhysWorld3D* world) const override;

			Matrix4f m_matrix;
			float m_length;
			float m_radius;
	};

	class NullCollider3D;

	using NullCollider3DConstRef = ObjectRef<const NullCollider3D>;
	using NullCollider3DRef = ObjectRef<NullCollider3D>;

	class NAZARA_PHYSICS3D_API NullCollider3D : public Collider3D
	{
		public:
			NullCollider3D();

			void ComputeInertialMatrix(Vector3f* inertia, Vector3f* center) const override;

			ColliderType3D GetType() const override;

			template<typename... Args> static NullCollider3DRef New(Args&&... args);

		private:
			NewtonCollision* CreateHandle(PhysWorld3D* world) const override;
	};

	class SphereCollider3D;

	using SphereCollider3DConstRef = ObjectRef<const SphereCollider3D>;
	using SphereCollider3DRef = ObjectRef<SphereCollider3D>;

	class NAZARA_PHYSICS3D_API SphereCollider3D : public Collider3D
	{
		public:
			SphereCollider3D(float radius, const Matrix4f& transformMatrix = Matrix4f::Identity());
			SphereCollider3D(float radius, const Vector3f& translation, const Quaternionf& rotation = Quaternionf::Identity());

			Boxf ComputeAABB(const Matrix4f& offsetMatrix = Matrix4f::Identity(), const Vector3f& scale = Vector3f::Unit()) const override;
			float ComputeVolume() const override;

			float GetRadius() const;
			ColliderType3D GetType() const override;

			template<typename... Args> static SphereCollider3DRef New(Args&&... args);

		private:
			NewtonCollision* CreateHandle(PhysWorld3D* world) const override;

			Vector3f m_position;
			float m_radius;
	};
}

#include <Nazara/Physics3D/Collider3D.inl>

#endif // NAZARA_COLLIDER3D_HPP
