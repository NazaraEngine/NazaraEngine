// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - ChipmunkPhysics2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <memory>
#include <Nazara/ChipmunkPhysics2D/Debug.hpp>

namespace Nz
{
	inline ChipmunkCollider2D::ChipmunkCollider2D() :
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

	inline UInt32 ChipmunkCollider2D::GetCategoryMask() const
	{
		return m_categoryMask;
	}

	inline UInt32 ChipmunkCollider2D::GetCollisionGroup() const
	{
		return m_collisionGroup;
	}

	inline unsigned int ChipmunkCollider2D::GetCollisionId() const
	{
		return m_collisionId;
	}

	inline UInt32 ChipmunkCollider2D::GetCollisionMask() const
	{
		return m_collisionMask;
	}

	inline float ChipmunkCollider2D::GetElasticity() const
	{
		return m_elasticity;
	}

	inline float ChipmunkCollider2D::GetFriction() const
	{
		return m_friction;
	}

	inline Vector2f ChipmunkCollider2D::GetSurfaceVelocity() const
	{
		return m_surfaceVelocity;
	}

	inline bool ChipmunkCollider2D::IsTrigger() const
	{
		return m_trigger;
	}

	inline void ChipmunkCollider2D::SetCategoryMask(UInt32 categoryMask)
	{
		m_categoryMask = categoryMask;
	}

	inline void ChipmunkCollider2D::SetCollisionGroup(UInt32 groupId)
	{
		m_collisionGroup = groupId;
	}

	inline void ChipmunkCollider2D::SetCollisionId(unsigned int typeId)
	{
		m_collisionId = typeId;
	}

	inline void ChipmunkCollider2D::SetCollisionMask(UInt32 mask)
	{
		m_collisionMask = mask;
	}

	inline void ChipmunkCollider2D::SetElasticity(float elasticity)
	{
		m_elasticity = elasticity;
	}

	inline void ChipmunkCollider2D::SetFriction(float friction)
	{
		m_friction = friction;
	}

	inline void ChipmunkCollider2D::SetSurfaceVelocity(const Vector2f& surfaceVelocity)
	{
		m_surfaceVelocity = surfaceVelocity;
	}

	inline void ChipmunkCollider2D::SetTrigger(bool trigger)
	{
		m_trigger = trigger;
	}

	inline float ChipmunkBoxCollider2D::GetRadius() const
	{
		return m_radius;
	}

	inline const Rectf& ChipmunkBoxCollider2D::GetRect() const
	{
		return m_rect;
	}

	inline Vector2f ChipmunkBoxCollider2D::GetSize() const
	{
		return m_rect.GetLengths();
	}


	inline const Vector2f& ChipmunkCircleCollider2D::GetOffset() const
	{
		return m_offset;
	}

	inline float ChipmunkCircleCollider2D::GetRadius() const
	{
		return m_radius;
	}


	inline bool Nz::ChipmunkCompoundCollider2D::DoesOverrideCollisionProperties() const
	{
		return m_doesOverrideCollisionProperties;
	}

	inline const std::vector<std::shared_ptr<ChipmunkCollider2D>>& ChipmunkCompoundCollider2D::GetGeoms() const
	{
		return m_geoms;
	}

	inline void Nz::ChipmunkCompoundCollider2D::OverridesCollisionProperties(bool shouldOverride)
	{
		m_doesOverrideCollisionProperties = shouldOverride;
	}


	inline const std::vector<Vector2d>& ChipmunkConvexCollider2D::GetVertices() const
	{
		return m_vertices;
	}


	ChipmunkSegmentCollider2D::ChipmunkSegmentCollider2D(const Vector2f& first, const Vector2f& second, float thickness) :
	ChipmunkSegmentCollider2D(first, first, second, second, thickness)
	{
	}

	inline ChipmunkSegmentCollider2D::ChipmunkSegmentCollider2D(const Vector2f& first, const Vector2f& firstNeighbor, const Vector2f& second, const Vector2f& secondNeighbor, float thickness) :
	m_first(first),
	m_firstNeighbor(firstNeighbor),
	m_second(second),
	m_secondNeighbor(secondNeighbor),
	m_thickness(thickness)
	{
	}

	inline const Vector2f& ChipmunkSegmentCollider2D::GetFirstPoint() const
	{
		return m_first;
	}

	inline const Vector2f& ChipmunkSegmentCollider2D::GetFirstPointNeighbor() const
	{
		return m_firstNeighbor;
	}

	inline float ChipmunkSegmentCollider2D::GetLength() const
	{
		return m_first.Distance(m_second);
	}

	inline const Vector2f& ChipmunkSegmentCollider2D::GetSecondPoint() const
	{
		return m_second;
	}

	inline const Vector2f& ChipmunkSegmentCollider2D::GetSecondPointNeighbor() const
	{
		return m_secondNeighbor;
	}

	inline float ChipmunkSegmentCollider2D::GetThickness() const
	{
		return m_thickness;
	}
}

#include <Nazara/ChipmunkPhysics2D/DebugOff.hpp>
