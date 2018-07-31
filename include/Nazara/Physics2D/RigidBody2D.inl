// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics2D/RigidBody2D.hpp>
#include <Nazara/Physics2D/Debug.hpp>

namespace Nz
{
	inline float RigidBody2D::GetAngularDamping() const
	{
		return GetMomentOfInertia();
	}

	inline void RigidBody2D::SetAngularDamping(float angularDamping)
	{
		SetMomentOfInertia(angularDamping);
	}
}

#include <Nazara/Physics2D/DebugOff.hpp>
