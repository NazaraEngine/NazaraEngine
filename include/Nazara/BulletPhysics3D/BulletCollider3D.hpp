// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
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
#include <Nazara/Math/Plane.hpp>
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
class btStaticPlaneShape;

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
			virtual BulletColliderType3D GetType() const = 0;

			BulletCollider3D& operator=(const BulletCollider3D&) = delete;
			BulletCollider3D& operator=(BulletCollider3D&&) = delete;

			static std::shared_ptr<BulletCollider3D> Build(const PrimitiveList& list);

		private:
			static std::shared_ptr<BulletCollider3D> CreateGeomFromPrimitive(const Primitive& primitive);
	};

	class NAZARA_BULLETPHYSICS3D_API BulletBoxCollider3D final : public BulletCollider3D
	{
		public:
			BulletBoxCollider3D(const Vector3f& lengths);
			~BulletBoxCollider3D();

			void BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const override;

			Vector3f GetLengths() const;
			btCollisionShape* GetShape() const override;
			BulletColliderType3D GetType() const override;

		private:
			std::unique_ptr<btBoxShape> m_shape;
			Vector3f m_lengths;
	};

	class NAZARA_BULLETPHYSICS3D_API BulletCapsuleCollider3D final : public BulletCollider3D
	{
		public:
			BulletCapsuleCollider3D(float length, float radius);
			~BulletCapsuleCollider3D();

			void BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const override;

			float GetLength() const;
			float GetRadius() const;
			btCollisionShape* GetShape() const override;
			BulletColliderType3D GetType() const override;

		private:
			std::unique_ptr<btCapsuleShape> m_shape;
			float m_length;
			float m_radius;
	};

	class NAZARA_BULLETPHYSICS3D_API BulletCompoundCollider3D final : public BulletCollider3D
	{
		public:
			struct ChildCollider;

			BulletCompoundCollider3D(std::vector<ChildCollider> childs);
			~BulletCompoundCollider3D();

			void BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const override;

			const std::vector<ChildCollider>& GetGeoms() const;
			btCollisionShape* GetShape() const override;
			BulletColliderType3D GetType() const override;

			struct ChildCollider
			{
				std::shared_ptr<BulletCollider3D> collider;
				Matrix4f offsetMatrix;
			};

		private:
			std::unique_ptr<btCompoundShape> m_shape;
			std::vector<ChildCollider> m_childs;
	};

	class NAZARA_BULLETPHYSICS3D_API BulletConeCollider3D final : public BulletCollider3D
	{
		public:
			BulletConeCollider3D(float length, float radius);
			~BulletConeCollider3D();

			void BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const override;

			float GetLength() const;
			float GetRadius() const;
			btCollisionShape* GetShape() const override;
			BulletColliderType3D GetType() const override;

		private:
			std::unique_ptr<btConeShape> m_shape;
			float m_length;
			float m_radius;
	};

	class NAZARA_BULLETPHYSICS3D_API BulletConvexCollider3D final : public BulletCollider3D
	{
		public:
			BulletConvexCollider3D(SparsePtr<const Vector3f> vertices, unsigned int vertexCount);
			~BulletConvexCollider3D();

			void BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const override;

			btCollisionShape* GetShape() const override;
			BulletColliderType3D GetType() const override;

		private:
			std::unique_ptr<btConvexHullShape> m_shape;
	};

	class NAZARA_BULLETPHYSICS3D_API BulletCylinderCollider3D final : public BulletCollider3D
	{
		public:
			BulletCylinderCollider3D(float length, float radius);
			~BulletCylinderCollider3D();

			void BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const override;

			float GetLength() const;
			float GetRadius() const;
			btCollisionShape* GetShape() const override;
			BulletColliderType3D GetType() const override;

		private:
			std::unique_ptr<btCylinderShape> m_shape;
			Matrix4f m_matrix;
			float m_length;
			float m_radius;
	};

	class NAZARA_BULLETPHYSICS3D_API BulletNullCollider3D final : public BulletCollider3D
	{
		public:
			BulletNullCollider3D();
			~BulletNullCollider3D();

			void BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const override;

			void ComputeInertia(float mass, Vector3f* inertia) const override;

			btCollisionShape* GetShape() const override;
			BulletColliderType3D GetType() const override;

		private:
			std::unique_ptr<btEmptyShape> m_shape;
	};

	class NAZARA_BULLETPHYSICS3D_API BulletSphereCollider3D final : public BulletCollider3D
	{
		public:
			BulletSphereCollider3D(float radius);
			~BulletSphereCollider3D();

			void BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const override;

			float GetRadius() const;
			btCollisionShape* GetShape() const override;
			BulletColliderType3D GetType() const override;

		private:
			std::unique_ptr<btSphereShape> m_shape;
			Vector3f m_position;
			float m_radius;
	};

	class NAZARA_BULLETPHYSICS3D_API BulletStaticPlaneCollider3D final : public BulletCollider3D
	{
		public:
			BulletStaticPlaneCollider3D(const Planef& plane);
			BulletStaticPlaneCollider3D(const Vector3f& normal, float distance);
			~BulletStaticPlaneCollider3D();

			void BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const override;

			float GetDistance() const;
			const Vector3f& GetNormal() const;
			btCollisionShape* GetShape() const override;
			BulletColliderType3D GetType() const override;

		private:
			std::unique_ptr<btStaticPlaneShape> m_shape;
			Vector3f m_normal;
			float m_distance;
	};
}

#include <Nazara/BulletPhysics3D/BulletCollider3D.inl>

#endif // NAZARA_BULLETPHYSICS3D_BULLETCOLLIDER3D_HPP
