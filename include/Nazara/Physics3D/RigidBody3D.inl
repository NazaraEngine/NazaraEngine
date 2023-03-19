// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics3D/Debug.hpp>

namespace Nz
{
	inline void RigidBody3D::DisableSleeping()
	{
		return EnableSleeping(false);
	}
}

#include <Nazara/Physics3D/DebugOff.hpp>
