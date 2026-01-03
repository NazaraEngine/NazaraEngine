// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics2D module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <memory>

namespace Nz
{
	inline Collider2D::Collider2D() :
	m_categoryMask(0xFFFFFFFF),
	m_collisionGroup(0),
	m_collisionMask(0xFFFFFFFF),
	m_surfaceVelocity(Vector2f::Zero()),
	m_trigger(false),
	m_elasticity(0.f),
	m_friction(0.f),
	m_collisionId(0)
	{
	}

	inline UInt32 Collider2D::GetCategoryMask() const
	{
		return m_categoryMask;
	}

	inline UInt32 Collider2D::GetCollisionGroup() const
	{
		return m_collisionGroup;
	}

	inline unsigned int Collider2D::GetCollisionId() const
	{
		return m_collisionId;
	}

	inline UInt32 Collider2D::GetCollisionMask() const
	{
		return m_collisionMask;
	}

	inline float Collider2D::GetElasticity() const
	{
		return m_elasticity;
	}

	inline float Collider2D::GetFriction() const
	{
		return m_friction;
	}

	inline Vector2f Collider2D::GetSurfaceVelocity() const
	{
		return m_surfaceVelocity;
	}

	inline bool Collider2D::IsTrigger() const
	{
		return m_trigger;
	}

	inline void Collider2D::SetCategoryMask(UInt32 categoryMask)
	{
		m_categoryMask = categoryMask;
	}

	inline void Collider2D::SetCollisionGroup(UInt32 groupId)
	{
		m_collisionGroup = groupId;
	}

	inline void Collider2D::SetCollisionId(unsigned int typeId)
	{
		m_collisionId = typeId;
	}

	inline void Collider2D::SetCollisionMask(UInt32 mask)
	{
		m_collisionMask = mask;
	}

	inline void Collider2D::SetElasticity(float elasticity)
	{
		m_elasticity = elasticity;
	}

	inline void Collider2D::SetFriction(float friction)
	{
		m_friction = friction;
	}

	inline void Collider2D::SetSurfaceVelocity(const Vector2f& surfaceVelocity)
	{
		m_surfaceVelocity = surfaceVelocity;
	}

	inline void Collider2D::SetTrigger(bool trigger)
	{
		m_trigger = trigger;
	}

	inline float BoxCollider2D::GetRadius() const
	{
		return m_radius;
	}

	inline const Rectf& BoxCollider2D::GetRect() const
	{
		return m_rect;
	}

	inline Vector2f BoxCollider2D::GetSize() const
	{
		return m_rect.GetLengths();
	}


	inline const Vector2f& CircleCollider2D::GetOffset() const
	{
		return m_offset;
	}

	inline float CircleCollider2D::GetRadius() const
	{
		return m_radius;
	}


	inline bool Nz::CompoundCollider2D::DoesOverrideCollisionProperties() const
	{
		return m_doesOverrideCollisionProperties;
	}

	inline const std::vector<std::shared_ptr<Collider2D>>& CompoundCollider2D::GetColliders() const
	{
		return m_colliders;
	}

	inline void Nz::CompoundCollider2D::OverridesCollisionProperties(bool shouldOverride)
	{
		m_doesOverrideCollisionProperties = shouldOverride;
	}


	inline const std::vector<Vector2d>& ConvexCollider2D::GetVertices() const
	{
		return m_vertices;
	}


	SegmentCollider2D::SegmentCollider2D(const Vector2f& first, const Vector2f& second, float thickness) :
	SegmentCollider2D(first, first, second, second, thickness)
	{
	}

	inline SegmentCollider2D::SegmentCollider2D(const Vector2f& first, const Vector2f& firstNeighbor, const Vector2f& second, const Vector2f& secondNeighbor, float thickness) :
	m_first(first),
	m_firstNeighbor(firstNeighbor),
	m_second(second),
	m_secondNeighbor(secondNeighbor),
	m_thickness(thickness)
	{
	}

	inline const Vector2f& SegmentCollider2D::GetFirstPoint() const
	{
		return m_first;
	}

	inline const Vector2f& SegmentCollider2D::GetFirstPointNeighbor() const
	{
		return m_firstNeighbor;
	}

	inline float SegmentCollider2D::GetLength() const
	{
		return m_first.Distance(m_second);
	}

	inline const Vector2f& SegmentCollider2D::GetSecondPoint() const
	{
		return m_second;
	}

	inline const Vector2f& SegmentCollider2D::GetSecondPointNeighbor() const
	{
		return m_secondNeighbor;
	}

	inline float SegmentCollider2D::GetThickness() const
	{
		return m_thickness;
	}
}
