// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_PHYSICS3D_COLLIDER3D_HPP
#define NAZARA_PHYSICS3D_COLLIDER3D_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Physics3D/Enums.hpp>
#include <Nazara/Physics3D/Export.hpp>
#include <NazaraUtils/SparsePtr.hpp>
#include <memory>
#include <variant>

namespace JPH
{
	class Shape;
	class ShapeSettings;
	class BoxShapeSettings;
}

namespace Nz
{
	class PrimitiveList;
	class StaticMesh;
	struct Primitive;

	class NAZARA_PHYSICS3D_API Collider3D
	{
		public:
			Collider3D();
			Collider3D(const Collider3D&) = delete;
			Collider3D(Collider3D&&) = delete;
			virtual ~Collider3D();

			virtual void BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix = Matrix4f::Identity()) const;

			bool CollisionQuery(const Vector3f& point) const;

			virtual std::shared_ptr<StaticMesh> GenerateDebugMesh() const;

			Boxf GetBoundingBox() const;
			Vector3f GetCenterOfMass() const;
			inline JPH::ShapeSettings* GetShapeSettings() const;
			const Collider3D* GetSubCollider(UInt32 subShapeID, UInt32& remainder) const;
			virtual ColliderType3D GetType() const = 0;

			Collider3D& operator=(const Collider3D&) = delete;
			Collider3D& operator=(Collider3D&&) = delete;

			static std::shared_ptr<Collider3D> Build(const PrimitiveList& list);

		protected:
			const JPH::Shape* GetShape() const;
			template<typename T> const T* GetShapeAs() const;
			template<typename T> const T* GetShapeSettingsAs() const;
			void ResetShapeSettings();
			void SetupShapeSettings(std::unique_ptr<JPH::ShapeSettings>&& shapeSettings);

		private:
			static std::shared_ptr<Collider3D> CreateGeomFromPrimitive(const Primitive& primitive);

			std::unique_ptr<JPH::ShapeSettings> m_shapeSettings;
	};

	/*********************************** Shapes ******************************************/

	class NAZARA_PHYSICS3D_API BoxCollider3D final : public Collider3D
	{
		public:
			BoxCollider3D(const Vector3f& lengths, float convexRadius = 0.01f);
			~BoxCollider3D() = default;

			void BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const override;

			Vector3f GetLengths() const;
			ColliderType3D GetType() const override;
	};

	class NAZARA_PHYSICS3D_API CapsuleCollider3D final : public Collider3D
	{
		public:
			CapsuleCollider3D(float height, float radius);
			~CapsuleCollider3D() = default;

			void BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const override;

			float GetHeight() const;
			float GetRadius() const;
			ColliderType3D GetType() const override;
	};

	class NAZARA_PHYSICS3D_API CompoundCollider3D final : public Collider3D
	{
		public:
			struct ChildCollider;

			CompoundCollider3D(std::vector<ChildCollider> childs);
			~CompoundCollider3D();

			void BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const override;

			const std::vector<ChildCollider>& GetGeoms() const;
			ColliderType3D GetType() const override;

			struct ChildCollider
			{
				std::shared_ptr<Collider3D> collider;
				Quaternionf rotation = Quaternionf::Identity();
				Vector3f offset = Vector3f::Zero();
			};

		private:
			std::vector<ChildCollider> m_childs;
	};

	class NAZARA_PHYSICS3D_API ConvexHullCollider3D final : public Collider3D
	{
		public:
			ConvexHullCollider3D(SparsePtr<const Vector3f> vertices, std::size_t vertexCount, float hullTolerance = 0.001f, float convexRadius = 0.f, float maxErrorConvexRadius = 0.05f);
			~ConvexHullCollider3D() = default;

			void BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const override;

			ColliderType3D GetType() const override;
	};

	class NAZARA_PHYSICS3D_API EmptyCollider3D final : public Collider3D
	{
		public:
			EmptyCollider3D();
			~EmptyCollider3D() = default;

			void BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const override;

			ColliderType3D GetType() const override;

			static std::shared_ptr<EmptyCollider3D> Get();
	};

	class NAZARA_PHYSICS3D_API MeshCollider3D final : public Collider3D
	{
		public:
			struct Settings;

			MeshCollider3D(const Settings& meshSettings);
			~MeshCollider3D() = default;

			UInt32 GetTriangleUserData(UInt32 subShapeID) const;
			ColliderType3D GetType() const override;

			struct Settings
			{
				std::size_t indexCount;
				std::size_t vertexCount;
				std::variant<std::nullptr_t, SparsePtr<const UInt16>, SparsePtr<const UInt32>> indices;
				SparsePtr<const Vector3f> vertices;
				SparsePtr<const UInt32> triangleMaterials;
				SparsePtr<const UInt32> triangleUserdata;
			};
	};

	class NAZARA_PHYSICS3D_API SphereCollider3D final : public Collider3D
	{
		public:
			SphereCollider3D(float radius);
			~SphereCollider3D() = default;

			float GetRadius() const;
			ColliderType3D GetType() const override;
	};

	/*********************************** Decorated ******************************************/

	class NAZARA_PHYSICS3D_API ScaledCollider3D final : public Collider3D
	{
		public:
			inline ScaledCollider3D(std::shared_ptr<Collider3D> collider, const Vector3f& scale);
			~ScaledCollider3D();

			void BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const override;

			ColliderType3D GetType() const override;

		private:
			std::shared_ptr<Collider3D> m_collider;
	};

	class NAZARA_PHYSICS3D_API TranslatedRotatedCollider3D final : public Collider3D
	{
		public:
			inline TranslatedRotatedCollider3D(std::shared_ptr<Collider3D> collider, const Vector3f& translation);
			inline TranslatedRotatedCollider3D(std::shared_ptr<Collider3D> collider, const Quaternionf& rotation);
			TranslatedRotatedCollider3D(std::shared_ptr<Collider3D> collider, const Vector3f& translation, const Quaternionf& rotation);
			~TranslatedRotatedCollider3D();

			void BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const override;

			ColliderType3D GetType() const override;

		private:
			std::shared_ptr<Collider3D> m_collider;
	};
}

#include <Nazara/Physics3D/Collider3D.inl>

#endif // NAZARA_PHYSICS3D_COLLIDER3D_HPP
