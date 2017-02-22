// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <memory>
#include <Nazara/Physics2D/Debug.hpp>

namespace Nz
{
	inline Collider2D::Collider2D() :
	m_collisionId(0),
	m_trigger(false)
	{
	}

	inline unsigned int Collider2D::GetCollisionId() const
	{
		return m_collisionId;
	}

	inline bool Collider2D::IsTrigger() const
	{
		return m_trigger;
	}

	inline void Collider2D::SetCollisionId(unsigned long typeId)
	{
		m_collisionId = typeId;
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
#include "Collider2D.hpp"
