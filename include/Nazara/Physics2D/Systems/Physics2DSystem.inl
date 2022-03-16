// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics2D/Systems/Physics2DSystem.hpp>
#include <Nazara/Physics2D/Debug.hpp>

namespace Nz
{
	template<typename... Args>
	RigidBody2DComponent Physics2DSystem::CreateRigidBody(Args&&... args)
	{
		return RigidBody2DComponent(&m_physWorld, std::forward<Args>(args)...);
	}

	inline PhysWorld2D& Physics2DSystem::GetPhysWorld()
	{
		return m_physWorld;
	}

	inline const PhysWorld2D& Physics2DSystem::GetPhysWorld() const
	{
		return m_physWorld;
	}
}

#include <Nazara/Physics2D/DebugOff.hpp>
