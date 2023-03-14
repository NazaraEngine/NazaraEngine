// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - BulletPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_BULLETPHYSICS3D_BULLETCOLLIDER3D_HPP
#define NAZARA_BULLETPHYSICS3D_BULLETCOLLIDER3D_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/BulletPhysics3D/Config.hpp>
#include <Nazara/BulletPhysics3D/Enums.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <NazaraUtils/Signal.hpp>
#include <NazaraUtils/SparsePtr.hpp>
#include <memory>

class btBoxShape;
class btCapsuleShape;
class btCompoundShape;
class btCollisionShape;
class btConeShape;
class btConvexHullShape;
class btCylinderShape;
class btEmptyShape;
class btSphereShape;

namespace Nz
{
	class PrimitiveList;
	class StaticMesh;
	struct Primitive;

	class NAZARA_BULLETPHYSICS3D_API BulletCollider3D
	{
		public:
			BulletCollider3D() = default;
			BulletCollider3D(const BulletCollider3D&) = delete;
			BulletCollider3D(BulletCollider3D&&) = delete;
			virtual ~BulletCollider3D();

			virtual void BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix = Matrix4f::Identity()) const = 0;

			Boxf ComputeAABB(const Vector3f& translation, const Quaternionf& rotation, const Vector3f& scale) const;
			virtual Boxf ComputeAABB(const Matrix4f& offsetMatrix = Matrix4f::Identity(), const Vector3f& scale = Vector3f::Unit()) const;
			virtual void ComputeInertia(float mass, Vector3f* inertia) const;

			virtual std::shared_ptr<StaticMesh> GenerateDebugMesh() const;

			virtual btCollisionShape* GetShape() const = 0;
			virtual ColliderType3D GetType() const = 0;

			BulletCollider3D& operator=(const BulletCollider3D&) = delete;
			BulletCollider3D& operator=(BulletCollider3D&&) = delete;

			static std::shared_ptr<BulletCollider3D> Build(const PrimitiveList& list);

		private:
			static std::shared_ptr<BulletCollider3D> CreateGeomFromPrimitive(const Primitive& primitive);
	};

	class NAZARA_BULLETPHYSICS3D_API BoxCollider3D final : public BulletCollider3D
	{
		public:
			BoxCollider3D(const Vector3f& lengths);
			~BoxCollider3D();

			void BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const override;

			Vector3f GetLengths() const;
			btCollisionShape* GetShape() const override;
			ColliderType3D GetType() const override;

		private:
			std::unique_ptr<btBoxShape> m_shape;
			Vector3f m_lengths;
	};

	class NAZARA_BULLETPHYSICS3D_API CapsuleCollider3D final : public BulletCollider3D
	{
		public:
			CapsuleCollider3D(float length, float radius);
			~CapsuleCollider3D();

			void BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const override;

			float GetLength() const;
			float GetRadius() const;
			btCollisionShape* GetShape() const override;
			ColliderType3D GetType() const override;

		private:
			std::unique_ptr<btCapsuleShape> m_shape;
			float m_length;
			float m_radius;
	};

	class NAZARA_BULLETPHYSICS3D_API CompoundCollider3D final : public BulletCollider3D
	{
		public:
			struct ChildCollider;

			CompoundCollider3D(std::vector<ChildCollider> childs);
			~CompoundCollider3D();

			void BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const override;

			const std::vector<ChildCollider>& GetGeoms() const;
			btCollisionShape* GetShape() const override;
			ColliderType3D GetType() const override;

			struct ChildCollider
			{
				std::shared_ptr<BulletCollider3D> collider;
				Matrix4f offsetMatrix;
			};

		private:
			std::unique_ptr<btCompoundShape> m_shape;
			std::vector<ChildCollider> m_childs;
	};

	class NAZARA_BULLETPHYSICS3D_API ConeCollider3D final : public BulletCollider3D
	{
		public:
			ConeCollider3D(float length, float radius);
			~ConeCollider3D();

			void BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const override;

			float GetLength() const;
			float GetRadius() const;
			btCollisionShape* GetShape() const override;
			ColliderType3D GetType() const override;

		private:
			std::unique_ptr<btConeShape> m_shape;
			float m_length;
			float m_radius;
	};

	class NAZARA_BULLETPHYSICS3D_API ConvexCollider3D final : public BulletCollider3D
	{
		public:
			ConvexCollider3D(SparsePtr<const Vector3f> vertices, unsigned int vertexCount);
			~ConvexCollider3D();

			void BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const override;

			btCollisionShape* GetShape() const override;
			ColliderType3D GetType() const override;

		private:
			std::unique_ptr<btConvexHullShape> m_shape;
	};

	class NAZARA_BULLETPHYSICS3D_API CylinderCollider3D final : public BulletCollider3D
	{
		public:
			CylinderCollider3D(float length, float radius);
			~CylinderCollider3D();

			void BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const override;

			float GetLength() const;
			float GetRadius() const;
			btCollisionShape* GetShape() const override;
			ColliderType3D GetType() const override;

		private:
			std::unique_ptr<btCylinderShape> m_shape;
			Matrix4f m_matrix;
			float m_length;
			float m_radius;
	};

	class NAZARA_BULLETPHYSICS3D_API NullCollider3D final : public BulletCollider3D
	{
		public:
			NullCollider3D();
			~NullCollider3D();

			void BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const override;

			void ComputeInertia(float mass, Vector3f* inertia) const override;

			btCollisionShape* GetShape() const override;
			ColliderType3D GetType() const override;

		private:
			std::unique_ptr<btEmptyShape> m_shape;
	};

	class NAZARA_BULLETPHYSICS3D_API SphereCollider3D final : public BulletCollider3D
	{
		public:
			SphereCollider3D(float radius);
			~SphereCollider3D();

			void BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const override;

			float GetRadius() const;
			btCollisionShape* GetShape() const override;
			ColliderType3D GetType() const override;

		private:
			std::unique_ptr<btSphereShape> m_shape;
			Vector3f m_position;
			float m_radius;
	};
}

#include <Nazara/BulletPhysics3D/BulletCollider3D.inl>

#endif // NAZARA_BULLETPHYSICS3D_BULLETCOLLIDER3D_HPP
