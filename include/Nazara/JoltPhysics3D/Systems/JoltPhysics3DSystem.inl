// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - JoltPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/JoltPhysics3D/Debug.hpp>

namespace Nz
{
	inline JoltPhysWorld3D& JoltPhysics3DSystem::GetPhysWorld()
	{
		return m_physWorld;
	}

	inline const JoltPhysWorld3D& JoltPhysics3DSystem::GetPhysWorld() const
	{
		return m_physWorld;
	}

	inline entt::handle JoltPhysics3DSystem::GetRigidBodyEntity(UInt32 bodyIndex) const
	{
		return entt::handle(m_registry, m_bodyIndicesToEntity[bodyIndex]);
	}
}

#include <Nazara/JoltPhysics3D/DebugOff.hpp>
