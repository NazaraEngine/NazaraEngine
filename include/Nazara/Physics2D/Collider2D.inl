// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <memory>
#include <Nazara/Physics2D/Debug.hpp>

namespace Nz
{
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

}

#include <Nazara/Physics2D/DebugOff.hpp>
