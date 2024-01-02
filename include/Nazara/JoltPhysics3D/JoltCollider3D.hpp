// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - JoltPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_JOLTPHYSICS3D_JOLTCOLLIDER3D_HPP
#define NAZARA_JOLTPHYSICS3D_JOLTCOLLIDER3D_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/JoltPhysics3D/Config.hpp>
#include <Nazara/JoltPhysics3D/Enums.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <NazaraUtils/Signal.hpp>
#include <NazaraUtils/SparsePtr.hpp>
#include <memory>

namespace JPH
{
	class ShapeSettings;
	class BoxShapeSettings;
}

namespace Nz
{
	class PrimitiveList;
	class StaticMesh;
	struct Primitive;

	class NAZARA_JOLTPHYSICS3D_API JoltCollider3D
	{
		public:
			JoltCollider3D();
			JoltCollider3D(const JoltCollider3D&) = delete;
			JoltCollider3D(JoltCollider3D&&) = delete;
			virtual ~JoltCollider3D();

			virtual void BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix = Matrix4f::Identity()) const = 0;

			virtual std::shared_ptr<StaticMesh> GenerateDebugMesh() const;

			inline JPH::ShapeSettings* GetShapeSettings() const;
			virtual JoltColliderType3D GetType() const = 0;

			JoltCollider3D& operator=(const JoltCollider3D&) = delete;
			JoltCollider3D& operator=(JoltCollider3D&&) = delete;

			static std::shared_ptr<JoltCollider3D> Build(const PrimitiveList& list);

		protected:
			template<typename T> const T* GetShapeSettingsAs() const;
			void ResetShapeSettings();
			void SetupShapeSettings(std::unique_ptr<JPH::ShapeSettings>&& shapeSettings);

		private:
			static std::shared_ptr<JoltCollider3D> CreateGeomFromPrimitive(const Primitive& primitive);

			std::unique_ptr<JPH::ShapeSettings> m_shapeSettings;
	};

	/*********************************** Shapes ******************************************/

	class NAZARA_JOLTPHYSICS3D_API JoltBoxCollider3D final : public JoltCollider3D
	{
		public:
			JoltBoxCollider3D(const Vector3f& lengths, float convexRadius = 0.01f);
			~JoltBoxCollider3D() = default;

			void BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const override;

			Vector3f GetLengths() const;
			JoltColliderType3D GetType() const override;
	};

	class NAZARA_JOLTPHYSICS3D_API JoltCapsuleCollider3D final : public JoltCollider3D
	{
		public:
			JoltCapsuleCollider3D(float height, float radius);
			~JoltCapsuleCollider3D() = default;

			void BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const override;

			float GetHeight() const;
			float GetRadius() const;
			JoltColliderType3D GetType() const override;
	};

	class NAZARA_JOLTPHYSICS3D_API JoltCompoundCollider3D final : public JoltCollider3D
	{
		public:
			struct ChildCollider;

			JoltCompoundCollider3D(std::vector<ChildCollider> childs);
			~JoltCompoundCollider3D();

			void BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const override;

			const std::vector<ChildCollider>& GetGeoms() const;
			JoltColliderType3D GetType() const override;

			struct ChildCollider
			{
				std::shared_ptr<JoltCollider3D> collider;
				Quaternionf rotation = Quaternionf::Identity();
				Vector3f offset = Vector3f::Zero();
			};

		private:
			std::vector<ChildCollider> m_childs;
	};
	
	class NAZARA_JOLTPHYSICS3D_API JoltConvexHullCollider3D final : public JoltCollider3D
	{
		public:
			JoltConvexHullCollider3D(SparsePtr<const Vector3f> vertices, std::size_t vertexCount, float hullTolerance = 0.001f, float convexRadius = 0.f, float maxErrorConvexRadius = 0.05f);
			~JoltConvexHullCollider3D() = default;

			void BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const override;

			JoltColliderType3D GetType() const override;
	};
	
	class NAZARA_JOLTPHYSICS3D_API JoltMeshCollider3D final : public JoltCollider3D
	{
		public:
			JoltMeshCollider3D(SparsePtr<const Vector3f> vertices, std::size_t vertexCount, SparsePtr<const UInt16> indices, std::size_t indexCount);
			JoltMeshCollider3D(SparsePtr<const Vector3f> vertices, std::size_t vertexCount, SparsePtr<const UInt32> indices, std::size_t indexCount);
			~JoltMeshCollider3D() = default;

			void BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const override;

			JoltColliderType3D GetType() const override;
	};

	class NAZARA_JOLTPHYSICS3D_API JoltSphereCollider3D final : public JoltCollider3D
	{
		public:
			JoltSphereCollider3D(float radius);
			~JoltSphereCollider3D() = default;

			void BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const override;

			float GetRadius() const;
			JoltColliderType3D GetType() const override;
	};

	/*********************************** Decorated ******************************************/
	
	class NAZARA_JOLTPHYSICS3D_API JoltTranslatedRotatedCollider3D final : public JoltCollider3D
	{
		public:
			inline JoltTranslatedRotatedCollider3D(std::shared_ptr<JoltCollider3D> collider, const Vector3f& translation);
			inline JoltTranslatedRotatedCollider3D(std::shared_ptr<JoltCollider3D> collider, const Quaternionf& rotation);
			JoltTranslatedRotatedCollider3D(std::shared_ptr<JoltCollider3D> collider, const Vector3f& translation, const Quaternionf& rotation);
			~JoltTranslatedRotatedCollider3D();

			void BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const override;

			JoltColliderType3D GetType() const override;

		private:
			std::shared_ptr<JoltCollider3D> m_collider;
	};
	
}

#include <Nazara/JoltPhysics3D/JoltCollider3D.inl>

#endif // NAZARA_JOLTPHYSICS3D_JOLTCOLLIDER3D_HPP
