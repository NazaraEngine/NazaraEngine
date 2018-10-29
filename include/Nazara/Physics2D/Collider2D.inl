// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics2D/Collider2D.hpp>
#include <memory>
#include <Nazara/Physics2D/Debug.hpp>

namespace Nz
{
	inline Collider2D::Collider2D() :
	m_categoryMask(0xFFFFFFFF),
	m_collisionGroup(0),
	m_collisionMask(0xFFFFFFFF),
	m_trigger(false),
	m_elasticity(0.f),
	m_friction(0.f),
	m_surfaceVelocity(Vector2f::Zero()),
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

	template<typename... Args>
	BoxCollider2DRef BoxCollider2D::New(Args&&... args)
	{
		std::unique_ptr<BoxCollider2D> object(new BoxCollider2D(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}

	inline const Vector2f& CircleCollider2D::GetOffset() const
	{
		return m_offset;
	}

	inline float CircleCollider2D::GetRadius() const
	{
		return m_radius;
	}

	template<typename... Args>
	CircleCollider2DRef CircleCollider2D::New(Args&&... args)
	{
		std::unique_ptr<CircleCollider2D> object(new CircleCollider2D(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}

	inline bool Nz::CompoundCollider2D::DoesOverrideCollisionProperties() const
	{
		return m_doesOverrideCollisionProperties;
	}

	inline const std::vector<Collider2DRef>& CompoundCollider2D::GetGeoms() const
	{
		return m_geoms;
	}

	inline void Nz::CompoundCollider2D::OverridesCollisionProperties(bool shouldOverride)
	{
		m_doesOverrideCollisionProperties = shouldOverride;
	}

	template<typename... Args>
	CompoundCollider2DRef CompoundCollider2D::New(Args&&... args)
	{
		std::unique_ptr<CompoundCollider2D> object(new CompoundCollider2D(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}

	inline const std::vector<Vector2d>& ConvexCollider2D::GetVertices() const
	{
		return m_vertices;
	}

	template<typename... Args>
	ConvexCollider2DRef ConvexCollider2D::New(Args&&... args)
	{
		std::unique_ptr<ConvexCollider2D> object(new ConvexCollider2D(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}

	template<typename... Args>
	NullCollider2DRef NullCollider2D::New(Args&&... args)
	{
		std::unique_ptr<NullCollider2D> object(new NullCollider2D(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}

	SegmentCollider2D::SegmentCollider2D(const Vector2f& first, const Vector2f& second, float thickness) :
	m_first(first),
	m_second(second),
	m_thickness(thickness)
	{
	}

	inline const Vector2f& SegmentCollider2D::GetFirstPoint() const
	{
		return m_first;
	}

	inline float SegmentCollider2D::GetLength() const
	{
		return m_first.Distance(m_second);
	}

	inline const Vector2f& SegmentCollider2D::GetSecondPoint() const
	{
		return m_second;
	}

	inline float SegmentCollider2D::GetThickness() const
	{
		return m_thickness;
	}

	template<typename... Args>
	SegmentCollider2DRef SegmentCollider2D::New(Args&&... args)
	{
		std::unique_ptr<SegmentCollider2D> object(new SegmentCollider2D(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}
}

#include <Nazara/Physics2D/DebugOff.hpp>
