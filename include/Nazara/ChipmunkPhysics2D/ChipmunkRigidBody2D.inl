// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/ChipmunkPhysics2D/Debug.hpp>

namespace Nz
{
	inline float ChipmunkRigidBody2D::GetAngularDamping() const
	{
		return GetMomentOfInertia();
	}

	inline Vector2f ChipmunkRigidBody2D::GetCenterOfGravity(CoordSys coordSys) const
	{
		return GetMassCenter(coordSys);
	}

	inline const Vector2f& ChipmunkRigidBody2D::GetPositionOffset() const
	{
		return m_positionOffset;
	}

	inline std::size_t ChipmunkRigidBody2D::GetShapeCount() const
	{
		return m_shapes.size();
	}

	inline void ChipmunkRigidBody2D::SetAngularDamping(float angularDamping)
	{
		SetMomentOfInertia(angularDamping);
	}
}

#include <Nazara/ChipmunkPhysics2D/DebugOff.hpp>
