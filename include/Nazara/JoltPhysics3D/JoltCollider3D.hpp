// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - JoltPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_JOLTPHYSICS3D_JOLTCOLLIDER3D_HPP
#define NAZARA_JOLTPHYSICS3D_JOLTCOLLIDER3D_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/JoltPhysics3D/Config.hpp>
#include <Nazara/JoltPhysics3D/Enums.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Utils/Signal.hpp>
#include <Nazara/Utils/SparsePtr.hpp>
#include <memory>

namespace JPH
{
	class ShapeSettings;
	class BoxShapeSettings;
	class CompoundShapeSettings;
	class SphereShapeSettings;
}

namespace Nz
{
	class PrimitiveList;
	class StaticMesh;
	struct Primitive;

	class NAZARA_JOLTPHYSICS3D_API JoltCollider3D
	{
		public:
			JoltCollider3D() = default;
			JoltCollider3D(const JoltCollider3D&) = delete;
			JoltCollider3D(JoltCollider3D&&) = delete;
			virtual ~JoltCollider3D();

			virtual void BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix = Matrix4f::Identity()) const = 0;

			virtual std::shared_ptr<StaticMesh> GenerateDebugMesh() const;

			virtual JPH::ShapeSettings* GetShapeSettings() const = 0;
			virtual JoltColliderType3D GetType() const = 0;

			JoltCollider3D& operator=(const JoltCollider3D&) = delete;
			JoltCollider3D& operator=(JoltCollider3D&&) = delete;

			static std::shared_ptr<JoltCollider3D> Build(const PrimitiveList& list);

		private:
			static std::shared_ptr<JoltCollider3D> CreateGeomFromPrimitive(const Primitive& primitive);
	};

	class NAZARA_JOLTPHYSICS3D_API JoltBoxCollider3D final : public JoltCollider3D
	{
		public:
			JoltBoxCollider3D(const Vector3f& lengths, float convexRadius = 0.f);
			~JoltBoxCollider3D();

			void BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const override;

			Vector3f GetLengths() const;
			JPH::ShapeSettings* GetShapeSettings() const override;
			JoltColliderType3D GetType() const override;

		private:
			std::unique_ptr<JPH::BoxShapeSettings> m_shapeSettings;
			Vector3f m_lengths;
	};
	
	class NAZARA_JOLTPHYSICS3D_API JoltCompoundCollider3D final : public JoltCollider3D
	{
		public:
			struct ChildCollider;

			JoltCompoundCollider3D(std::vector<ChildCollider> childs);
			~JoltCompoundCollider3D();

			void BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const override;

			const std::vector<ChildCollider>& GetGeoms() const;
			JPH::ShapeSettings* GetShapeSettings() const override;
			JoltColliderType3D GetType() const override;

			struct ChildCollider
			{
				std::shared_ptr<JoltCollider3D> collider;
				Quaternionf rotation = Quaternionf::Identity();
				Vector3f offset = Vector3f::Zero();
			};

		private:
			std::unique_ptr<JPH::CompoundShapeSettings> m_shapeSettings;
			std::vector<ChildCollider> m_childs;
	};

	class NAZARA_JOLTPHYSICS3D_API JoltSphereCollider3D final : public JoltCollider3D
	{
		public:
			JoltSphereCollider3D(float radius);
			~JoltSphereCollider3D();

			void BuildDebugMesh(std::vector<Vector3f>& vertices, std::vector<UInt16>& indices, const Matrix4f& offsetMatrix) const override;

			float GetRadius() const;
			JPH::ShapeSettings* GetShapeSettings() const override;
			JoltColliderType3D GetType() const override;

		private:
			std::unique_ptr<JPH::SphereShapeSettings> m_shapeSettings;
			Vector3f m_position;
			float m_radius;
	};
}

#include <Nazara/JoltPhysics3D/JoltCollider3D.inl>

#endif // NAZARA_JOLTPHYSICS3D_JOLTCOLLIDER3D_HPP
