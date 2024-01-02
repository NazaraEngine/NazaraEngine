// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - ChipmunkPhysics2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CHIPMUNKPHYSICS2D_CHIPMUNKCOLLIDER2D_HPP
#define NAZARA_CHIPMUNKPHYSICS2D_CHIPMUNKCOLLIDER2D_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/ChipmunkPhysics2D/Config.hpp>
#include <Nazara/ChipmunkPhysics2D/Enums.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <NazaraUtils/FunctionRef.hpp>
#include <NazaraUtils/Signal.hpp>
#include <NazaraUtils/SparsePtr.hpp>
#include <vector>

struct cpBody;
struct cpShape;

namespace Nz
{
	class ChipmunkRigidBody2D;

	class NAZARA_CHIPMUNKPHYSICS2D_API ChipmunkCollider2D
	{
		friend ChipmunkRigidBody2D;
		friend class ChipmunkCompoundCollider2D; //< See CompoundCollider2D::CreateShapes

		public:
			inline ChipmunkCollider2D();
			ChipmunkCollider2D(const ChipmunkCollider2D&) = delete;
			ChipmunkCollider2D(ChipmunkCollider2D&&) = delete;
			virtual ~ChipmunkCollider2D();

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

			virtual ChipmunkColliderType2D GetType() const = 0;

			inline bool IsTrigger() const;

			inline void SetCategoryMask(UInt32 categoryMask);
			inline void SetCollisionGroup(UInt32 groupId);
			inline void SetCollisionId(unsigned int typeId);
			inline void SetCollisionMask(UInt32 mask);
			inline void SetElasticity(float elasticity);
			inline void SetFriction(float friction);
			inline void SetSurfaceVelocity(const Vector2f& surfaceVelocity);
			inline void SetTrigger(bool trigger);

			ChipmunkCollider2D& operator=(const ChipmunkCollider2D&) = delete;
			ChipmunkCollider2D& operator=(ChipmunkCollider2D&&) = delete;

			// Signals:
			NazaraSignal(OnColliderRelease, const ChipmunkCollider2D* /*collider*/);

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

	class NAZARA_CHIPMUNKPHYSICS2D_API ChipmunkBoxCollider2D : public ChipmunkCollider2D
	{
		public:
			ChipmunkBoxCollider2D(const Vector2f& size, float radius = 0.f);
			ChipmunkBoxCollider2D(const Rectf& rect, float radius = 0.f);

			Nz::Vector2f ComputeCenterOfMass() const override;
			float ComputeMomentOfInertia(float mass) const override;

			inline float GetRadius() const;
			inline const Rectf& GetRect() const;
			inline Vector2f GetSize() const;
			ChipmunkColliderType2D GetType() const override;

		private:
			std::size_t CreateShapes(cpBody* body, std::vector<cpShape*>* shapes) const override;

			Rectf m_rect;
			float m_radius;
	};

	class NAZARA_CHIPMUNKPHYSICS2D_API ChipmunkCircleCollider2D : public ChipmunkCollider2D
	{
		public:
			ChipmunkCircleCollider2D(float radius, const Vector2f& offset = Vector2f::Zero());

			Nz::Vector2f ComputeCenterOfMass() const override;
			float ComputeMomentOfInertia(float mass) const override;

			inline const Vector2f& GetOffset() const;
			inline float GetRadius() const;
			ChipmunkColliderType2D GetType() const override;

		private:
			std::size_t CreateShapes(cpBody* body, std::vector<cpShape*>* shapes) const override;

			Vector2f m_offset;
			float m_radius;
	};

	class NAZARA_CHIPMUNKPHYSICS2D_API ChipmunkCompoundCollider2D : public ChipmunkCollider2D
	{
		public:
			ChipmunkCompoundCollider2D(std::vector<std::shared_ptr<ChipmunkCollider2D>> geoms);

			Nz::Vector2f ComputeCenterOfMass() const override;
			float ComputeMomentOfInertia(float mass) const override;

			inline bool DoesOverrideCollisionProperties() const;

			inline const std::vector<std::shared_ptr<ChipmunkCollider2D>>& GetGeoms() const;
			ChipmunkColliderType2D GetType() const override;

			inline void OverridesCollisionProperties(bool shouldOverride);

		private:
			std::size_t CreateShapes(cpBody* body, std::vector<cpShape*>* shapes) const override;
			std::size_t GenerateShapes(cpBody* body, std::vector<cpShape*>* shapes) const override;

			std::vector<std::shared_ptr<ChipmunkCollider2D>> m_geoms;
			bool m_doesOverrideCollisionProperties;
	};

	class NAZARA_CHIPMUNKPHYSICS2D_API ChipmunkConvexCollider2D : public ChipmunkCollider2D
	{
		public:
			ChipmunkConvexCollider2D(SparsePtr<const Vector2f> vertices, std::size_t vertexCount, float radius = 0.f);

			Nz::Vector2f ComputeCenterOfMass() const override;
			float ComputeMomentOfInertia(float mass) const override;

			ChipmunkColliderType2D GetType() const override;
			inline const std::vector<Vector2d>& GetVertices() const;

		private:
			std::size_t CreateShapes(cpBody* body, std::vector<cpShape*>* shapes) const override;

			std::vector<Vector2d> m_vertices;
			float m_radius;
	};

	class NAZARA_CHIPMUNKPHYSICS2D_API ChipmunkNullCollider2D : public ChipmunkCollider2D
	{
		public:
			ChipmunkNullCollider2D() = default;

			Nz::Vector2f ComputeCenterOfMass() const override;
			float ComputeMomentOfInertia(float mass) const override;

			ChipmunkColliderType2D GetType() const override;

		private:
			std::size_t CreateShapes(cpBody* body, std::vector<cpShape*>* shapes) const override;
	};

	class NAZARA_CHIPMUNKPHYSICS2D_API ChipmunkSegmentCollider2D : public ChipmunkCollider2D
	{
		public:
			inline ChipmunkSegmentCollider2D(const Vector2f& first, const Vector2f& second, float thickness = 1.f);
			inline ChipmunkSegmentCollider2D(const Vector2f& first, const Vector2f& firstNeighbor, const Vector2f& second, const Vector2f& secondNeighbor, float thickness = 1.f);

			Nz::Vector2f ComputeCenterOfMass() const override;
			float ComputeMomentOfInertia(float mass) const override;

			inline const Vector2f& GetFirstPoint() const;
			inline const Vector2f& GetFirstPointNeighbor() const;
			inline float GetLength() const;
			inline const Vector2f& GetSecondPoint() const;
			inline const Vector2f& GetSecondPointNeighbor() const;
			inline float GetThickness() const;
			ChipmunkColliderType2D GetType() const override;

		private:
			std::size_t CreateShapes(cpBody* body, std::vector<cpShape*>* shapes) const override;

			Vector2f m_first;
			Vector2f m_firstNeighbor;
			Vector2f m_second;
			Vector2f m_secondNeighbor;
			float m_thickness;
	};
}

#include <Nazara/ChipmunkPhysics2D/ChipmunkCollider2D.inl>

#endif // NAZARA_CHIPMUNKPHYSICS2D_CHIPMUNKCOLLIDER2D_HPP
