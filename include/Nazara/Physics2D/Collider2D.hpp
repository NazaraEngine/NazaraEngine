// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics2D module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_PHYSICS2D_COLLIDER2D_HPP
#define NAZARA_PHYSICS2D_COLLIDER2D_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Physics2D/Enums.hpp>
#include <Nazara/Physics2D/Export.hpp>
#include <NazaraUtils/FunctionRef.hpp>
#include <NazaraUtils/Signal.hpp>
#include <NazaraUtils/SparsePtr.hpp>
#include <vector>

struct cpBody;
struct cpShape;

namespace Nz
{
	class RigidBody2D;

	class NAZARA_PHYSICS2D_API Collider2D
	{
		friend RigidBody2D;
		friend class CompoundCollider2D; //< See CompoundCollider2D::CreateShapes

		public:
			inline Collider2D();
			Collider2D(const Collider2D&) = delete;
			Collider2D(Collider2D&&) = delete;
			virtual ~Collider2D();

			virtual Vector2f ComputeCenterOfMass() const = 0;
			virtual float ComputeMomentOfInertia(float mass) const = 0;

			virtual void ForEachPolygon(const FunctionRef<void(const Vector2f* vertices, std::size_t vertexCount)>& callback) const;

			inline UInt32 GetCategoryMask() const;
			inline UInt32 GetCollisionGroup() const;
			inline unsigned int GetCollisionId() const;
			inline UInt32 GetCollisionMask() const;
			inline float GetElasticity() const;
			inline float GetFriction() const;
			inline Vector2f GetSurfaceVelocity() const;

			virtual ColliderType2D GetType() const = 0;

			inline bool IsTrigger() const;

			inline void SetCategoryMask(UInt32 categoryMask);
			inline void SetCollisionGroup(UInt32 groupId);
			inline void SetCollisionId(unsigned int typeId);
			inline void SetCollisionMask(UInt32 mask);
			inline void SetElasticity(float elasticity);
			inline void SetFriction(float friction);
			inline void SetSurfaceVelocity(const Vector2f& surfaceVelocity);
			inline void SetTrigger(bool trigger);

			Collider2D& operator=(const Collider2D&) = delete;
			Collider2D& operator=(Collider2D&&) = delete;

			// Signals:
			NazaraSignal(OnColliderRelease, const Collider2D* /*collider*/);

		protected:
			virtual std::size_t CreateShapes(cpBody* body, std::vector<cpShape*>* shapes) const = 0;

			UInt32 m_categoryMask;
			UInt32 m_collisionGroup;
			UInt32 m_collisionMask;
			Vector2f m_surfaceVelocity;
			bool m_trigger;
			float m_elasticity;
			float m_friction;
			unsigned int m_collisionId;

		private:
			virtual std::size_t GenerateShapes(cpBody* body, std::vector<cpShape*>* shapes) const;
	};

	class NAZARA_PHYSICS2D_API BoxCollider2D : public Collider2D
	{
		public:
			BoxCollider2D(const Vector2f& size, float radius = 0.f);
			BoxCollider2D(const Rectf& rect, float radius = 0.f);

			Nz::Vector2f ComputeCenterOfMass() const override;
			float ComputeMomentOfInertia(float mass) const override;

			inline float GetRadius() const;
			inline const Rectf& GetRect() const;
			inline Vector2f GetSize() const;
			ColliderType2D GetType() const override;

		private:
			std::size_t CreateShapes(cpBody* body, std::vector<cpShape*>* shapes) const override;

			Rectf m_rect;
			float m_radius;
	};

	class NAZARA_PHYSICS2D_API CircleCollider2D : public Collider2D
	{
		public:
			CircleCollider2D(float radius, const Vector2f& offset = Vector2f::Zero());

			Nz::Vector2f ComputeCenterOfMass() const override;
			float ComputeMomentOfInertia(float mass) const override;

			inline const Vector2f& GetOffset() const;
			inline float GetRadius() const;
			ColliderType2D GetType() const override;

		private:
			std::size_t CreateShapes(cpBody* body, std::vector<cpShape*>* shapes) const override;

			Vector2f m_offset;
			float m_radius;
	};

	class NAZARA_PHYSICS2D_API CompoundCollider2D : public Collider2D
	{
		public:
			CompoundCollider2D(std::vector<std::shared_ptr<Collider2D>> colliders);

			Nz::Vector2f ComputeCenterOfMass() const override;
			float ComputeMomentOfInertia(float mass) const override;

			inline bool DoesOverrideCollisionProperties() const;

			inline const std::vector<std::shared_ptr<Collider2D>>& GetColliders() const;
			ColliderType2D GetType() const override;

			inline void OverridesCollisionProperties(bool shouldOverride);

		private:
			std::size_t CreateShapes(cpBody* body, std::vector<cpShape*>* shapes) const override;
			std::size_t GenerateShapes(cpBody* body, std::vector<cpShape*>* shapes) const override;

			std::vector<std::shared_ptr<Collider2D>> m_colliders;
			bool m_doesOverrideCollisionProperties;
	};

	class NAZARA_PHYSICS2D_API ConvexCollider2D : public Collider2D
	{
		public:
			ConvexCollider2D(SparsePtr<const Vector2f> vertices, std::size_t vertexCount, float radius = 0.f);

			Nz::Vector2f ComputeCenterOfMass() const override;
			float ComputeMomentOfInertia(float mass) const override;

			ColliderType2D GetType() const override;
			inline const std::vector<Vector2d>& GetVertices() const;

		private:
			std::size_t CreateShapes(cpBody* body, std::vector<cpShape*>* shapes) const override;

			std::vector<Vector2d> m_vertices;
			float m_radius;
	};

	class NAZARA_PHYSICS2D_API NullCollider2D : public Collider2D
	{
		public:
			NullCollider2D() = default;

			Nz::Vector2f ComputeCenterOfMass() const override;
			float ComputeMomentOfInertia(float mass) const override;

			ColliderType2D GetType() const override;

		private:
			std::size_t CreateShapes(cpBody* body, std::vector<cpShape*>* shapes) const override;
	};

	class NAZARA_PHYSICS2D_API SegmentCollider2D : public Collider2D
	{
		public:
			inline SegmentCollider2D(const Vector2f& first, const Vector2f& second, float thickness = 1.f);
			inline SegmentCollider2D(const Vector2f& first, const Vector2f& firstNeighbor, const Vector2f& second, const Vector2f& secondNeighbor, float thickness = 1.f);

			Nz::Vector2f ComputeCenterOfMass() const override;
			float ComputeMomentOfInertia(float mass) const override;

			inline const Vector2f& GetFirstPoint() const;
			inline const Vector2f& GetFirstPointNeighbor() const;
			inline float GetLength() const;
			inline const Vector2f& GetSecondPoint() const;
			inline const Vector2f& GetSecondPointNeighbor() const;
			inline float GetThickness() const;
			ColliderType2D GetType() const override;

		private:
			std::size_t CreateShapes(cpBody* body, std::vector<cpShape*>* shapes) const override;

			Vector2f m_first;
			Vector2f m_firstNeighbor;
			Vector2f m_second;
			Vector2f m_secondNeighbor;
			float m_thickness;
	};
}

#include <Nazara/Physics2D/Collider2D.inl>

#endif // NAZARA_PHYSICS2D_COLLIDER2D_HPP
