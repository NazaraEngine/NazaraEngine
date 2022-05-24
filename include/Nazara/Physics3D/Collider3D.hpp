// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PHYSICS3D_COLLIDER3D_HPP
#define NAZARA_PHYSICS3D_COLLIDER3D_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Physics3D/Config.hpp>
#include <Nazara/Physics3D/Enums.hpp>
#include <Nazara/Utils/Signal.hpp>
#include <Nazara/Utils/SparsePtr.hpp>
#include <unordered_map>

class NewtonCollision;

namespace Nz
{
	///TODO: CollisionModifier
	///TODO: HeightfieldGeom
	///TODO: PlaneGeom ?
	///TODO: SceneGeom
	///TODO: TreeGeom

	class PrimitiveList;
	class PhysWorld3D;
	class StaticMesh;

	class NAZARA_PHYSICS3D_API Collider3D
	{
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

			virtual void ForEachPolygon(const std::function<void(const Vector3f* vertices, std::size_t vertexCount)>& callback) const;

			virtual std::shared_ptr<StaticMesh> GenerateMesh() const;

			NewtonCollision* GetHandle(PhysWorld3D* world) const;
			virtual ColliderType3D GetType() const = 0;

			Collider3D& operator=(const Collider3D&) = delete;
			Collider3D& operator=(Collider3D&&) = delete;

			static std::shared_ptr<Collider3D> Build(const PrimitiveList& list);

			// Signals:
			NazaraSignal(OnColliderRelease, const Collider3D* /*collider*/);

		protected:
			virtual NewtonCollision* CreateHandle(PhysWorld3D* world) const = 0;

			mutable std::unordered_map<PhysWorld3D*, NewtonCollision*> m_handles;
	};

	class NAZARA_PHYSICS3D_API BoxCollider3D : public Collider3D
	{
		public:
			BoxCollider3D(const Vector3f& lengths, const Matrix4f& transformMatrix = Matrix4f::Identity());
			BoxCollider3D(const Vector3f& lengths, const Vector3f& translation, const Quaternionf& rotation = Quaternionf::Identity());

			Boxf ComputeAABB(const Matrix4f& offsetMatrix = Matrix4f::Identity(), const Vector3f& scale = Vector3f::Unit()) const override;
			float ComputeVolume() const override;

			Vector3f GetLengths() const;
			ColliderType3D GetType() const override;

		private:
			NewtonCollision* CreateHandle(PhysWorld3D* world) const override;

			Matrix4f m_matrix;
			Vector3f m_lengths;
	};

	class NAZARA_PHYSICS3D_API CapsuleCollider3D : public Collider3D
	{
		public:
			CapsuleCollider3D(float length, float radius, const Matrix4f& transformMatrix = Matrix4f::Identity());
			CapsuleCollider3D(float length, float radius, const Vector3f& translation, const Quaternionf& rotation = Quaternionf::Identity());

			float GetLength() const;
			float GetRadius() const;
			ColliderType3D GetType() const override;

		private:
			NewtonCollision* CreateHandle(PhysWorld3D* world) const override;

			Matrix4f m_matrix;
			float m_length;
			float m_radius;
	};

	class NAZARA_PHYSICS3D_API CompoundCollider3D : public Collider3D
	{
		public:
			CompoundCollider3D(std::vector<std::shared_ptr<Collider3D>> geoms);

			const std::vector<std::shared_ptr<Collider3D>>& GetGeoms() const;
			ColliderType3D GetType() const override;

		private:
			NewtonCollision* CreateHandle(PhysWorld3D* world) const override;

			std::vector<std::shared_ptr<Collider3D>> m_geoms;
	};

	class NAZARA_PHYSICS3D_API ConeCollider3D : public Collider3D
	{
		public:
			ConeCollider3D(float length, float radius, const Matrix4f& transformMatrix = Matrix4f::Identity());
			ConeCollider3D(float length, float radius, const Vector3f& translation, const Quaternionf& rotation = Quaternionf::Identity());

			float GetLength() const;
			float GetRadius() const;
			ColliderType3D GetType() const override;

		private:
			NewtonCollision* CreateHandle(PhysWorld3D* world) const override;

			Matrix4f m_matrix;
			float m_length;
			float m_radius;
	};

	class NAZARA_PHYSICS3D_API ConvexCollider3D : public Collider3D
	{
		public:
			ConvexCollider3D(SparsePtr<const Vector3f> vertices, unsigned int vertexCount, float tolerance = 0.002f, const Matrix4f& transformMatrix = Matrix4f::Identity());
			ConvexCollider3D(SparsePtr<const Vector3f> vertices, unsigned int vertexCount, float tolerance, const Vector3f& translation, const Quaternionf& rotation = Quaternionf::Identity());

			ColliderType3D GetType() const override;

		private:
			NewtonCollision* CreateHandle(PhysWorld3D* world) const override;

			std::vector<Vector3f> m_vertices;
			Matrix4f m_matrix;
			float m_tolerance;
	};

	class NAZARA_PHYSICS3D_API CylinderCollider3D : public Collider3D
	{
		public:
			CylinderCollider3D(float length, float radius, const Matrix4f& transformMatrix = Matrix4f::Identity());
			CylinderCollider3D(float length, float radius, const Vector3f& translation, const Quaternionf& rotation = Quaternionf::Identity());

			float GetLength() const;
			float GetRadius() const;
			ColliderType3D GetType() const override;

		private:
			NewtonCollision* CreateHandle(PhysWorld3D* world) const override;

			Matrix4f m_matrix;
			float m_length;
			float m_radius;
	};

	class NAZARA_PHYSICS3D_API NullCollider3D : public Collider3D
	{
		public:
			NullCollider3D();

			void ComputeInertialMatrix(Vector3f* inertia, Vector3f* center) const override;

			ColliderType3D GetType() const override;

		private:
			NewtonCollision* CreateHandle(PhysWorld3D* world) const override;
	};

	class NAZARA_PHYSICS3D_API SphereCollider3D : public Collider3D
	{
		public:
			SphereCollider3D(float radius, const Matrix4f& transformMatrix = Matrix4f::Identity());
			SphereCollider3D(float radius, const Vector3f& translation, const Quaternionf& rotation = Quaternionf::Identity());

			Boxf ComputeAABB(const Matrix4f& offsetMatrix = Matrix4f::Identity(), const Vector3f& scale = Vector3f::Unit()) const override;
			float ComputeVolume() const override;

			float GetRadius() const;
			ColliderType3D GetType() const override;

		private:
			NewtonCollision* CreateHandle(PhysWorld3D* world) const override;

			Vector3f m_position;
			float m_radius;
	};
}

#include <Nazara/Physics3D/Collider3D.inl>

#endif // NAZARA_PHYSICS3D_COLLIDER3D_HPP
