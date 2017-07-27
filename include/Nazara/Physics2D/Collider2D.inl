// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics2D/Collider2D.hpp>
#include <memory>
#include <Nazara/Physics2D/Debug.hpp>

namespace Nz
{
	inline Collider2D::Collider2D() :
	m_trigger(false),
	m_categoryMask(0xFFFFFFFF),
	m_collisionGroup(0),
	m_collisionId(0),
	m_collisionMask(0xFFFFFFFF)
	{
	}

	inline Nz::UInt32 Collider2D::GetCategoryMask() const
	{
		return m_categoryMask;
	}

	inline Nz::UInt32 Collider2D::GetCollisionGroup() const
	{
		return m_collisionGroup;
	}

	inline unsigned int Collider2D::GetCollisionId() const
	{
		return m_collisionId;
	}

	inline Nz::UInt32 Collider2D::GetCollisionMask() const
	{
		return m_collisionMask;
	}

	inline bool Collider2D::IsTrigger() const
	{
		return m_trigger;
	}

	inline void Collider2D::SetCategoryMask(Nz::UInt32 categoryMask)
	{
		m_categoryMask = categoryMask;
	}

	inline void Collider2D::SetCollisionGroup(Nz::UInt32 groupId)
	{
		m_collisionGroup = groupId;
	}

	inline void Collider2D::SetCollisionId(unsigned int typeId)
	{
		m_collisionId = typeId;
	}

	inline void Collider2D::SetCollisionMask(Nz::UInt32 mask)
	{
		m_collisionMask = mask;
	}

	inline void Collider2D::SetTrigger(bool trigger)
	{
		m_trigger = trigger;
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

	inline const std::vector<Collider2DRef>& Nz::CompoundCollider2D::GetGeoms() const
	{
		return m_geoms;
	}

	template<typename... Args>
	CompoundCollider2DRef CompoundCollider2D::New(Args&&... args)
	{
		std::unique_ptr<CompoundCollider2D> object(new CompoundCollider2D(std::forward<Args>(args)...));
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

	template<typename... Args>
	SegmentCollider2DRef SegmentCollider2D::New(Args&&... args)
	{
		std::unique_ptr<SegmentCollider2D> object(new SegmentCollider2D(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}
}

#include <Nazara/Physics2D/DebugOff.hpp>
