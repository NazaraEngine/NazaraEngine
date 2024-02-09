// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics3D/Debug.hpp>

namespace Nz
{
	inline PhysWorld3D& Physics3DSystem::GetPhysWorld()
	{
		return m_physWorld;
	}

	inline const PhysWorld3D& Physics3DSystem::GetPhysWorld() const
	{
		return m_physWorld;
	}

	inline entt::handle Physics3DSystem::GetRigidBodyEntity(UInt32 bodyIndex) const
	{
		return entt::handle(m_registry, m_bodyIndicesToEntity[bodyIndex]);
	}
}

#include <Nazara/Physics3D/DebugOff.hpp>
