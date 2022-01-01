// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics2D/RigidBody2D.hpp>
#include <Nazara/Physics2D/Debug.hpp>

namespace Nz
{
	inline float RigidBody2D::GetAngularDamping() const
	{
		return GetMomentOfInertia();
	}

	inline Vector2f RigidBody2D::GetCenterOfGravity(CoordSys coordSys) const
	{
		return GetMassCenter(coordSys);
	}

	inline const Vector2f& RigidBody2D::GetPositionOffset() const
	{
		return m_positionOffset;
	}

	inline std::size_t RigidBody2D::GetShapeCount() const
	{
		return m_shapes.size();
	}

	inline void RigidBody2D::SetAngularDamping(float angularDamping)
	{
		SetMomentOfInertia(angularDamping);
	}
}

#include <Nazara/Physics2D/DebugOff.hpp>
