// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - BulletPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/BulletPhysics3D/Debug.hpp>

namespace Nz
{
	template<typename... Args>
	BulletRigidBody3DComponent BulletPhysics3DSystem::CreateRigidBody(Args&&... args)
	{
		return BulletRigidBody3DComponent(&m_physWorld, std::forward<Args>(args)...);
	}

	inline BulletPhysWorld3D& BulletPhysics3DSystem::GetPhysWorld()
	{
		return m_physWorld;
	}

	inline const BulletPhysWorld3D& BulletPhysics3DSystem::GetPhysWorld() const
	{
		return m_physWorld;
	}
}

#include <Nazara/BulletPhysics3D/DebugOff.hpp>
