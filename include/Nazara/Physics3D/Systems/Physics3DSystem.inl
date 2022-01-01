// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics3D/Systems/Physics3DSystem.hpp>
#include <Nazara/Physics3D/Debug.hpp>

namespace Nz
{
	template<typename... Args>
	RigidBody3DComponent Physics3DSystem::CreateRigidBody(Args&&... args)
	{
		return RigidBody3DComponent(&m_physWorld, std::forward<Args>(args)...);
	}

	inline PhysWorld3D& Physics3DSystem::GetPhysWorld()
	{
		return m_physWorld;
	}

	inline const PhysWorld3D& Physics3DSystem::GetPhysWorld() const
	{
		return m_physWorld;
	}
}

#include <Nazara/Physics3D/DebugOff.hpp>
