// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/ChipmunkPhysics2D/Debug.hpp>

namespace Nz
{
	template<typename... Args>
	ChipmunkRigidBody2DComponent ChipmunkPhysics2DSystem::CreateRigidBody(Args&&... args)
	{
		return ChipmunkRigidBody2DComponent(&m_physWorld, std::forward<Args>(args)...);
	}

	inline ChipmunkPhysWorld2D& ChipmunkPhysics2DSystem::GetPhysWorld()
	{
		return m_physWorld;
	}

	inline const ChipmunkPhysWorld2D& ChipmunkPhysics2DSystem::GetPhysWorld() const
	{
		return m_physWorld;
	}
}

#include <Nazara/ChipmunkPhysics2D/DebugOff.hpp>
