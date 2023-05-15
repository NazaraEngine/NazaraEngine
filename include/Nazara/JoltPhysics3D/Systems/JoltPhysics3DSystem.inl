// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - JoltPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/JoltPhysics3D/Debug.hpp>

namespace Nz
{
	template<typename... Args>
	JoltCharacterComponent JoltPhysics3DSystem::CreateCharacter(Args&& ...args)
	{
		return JoltCharacterComponent(m_physWorld, std::forward<Args>(args)...);
	}

	template<typename... Args>
	JoltRigidBody3DComponent JoltPhysics3DSystem::CreateRigidBody(Args&&... args)
	{
		return JoltRigidBody3DComponent(m_physWorld, std::forward<Args>(args)...);
	}

	inline JoltPhysWorld3D& JoltPhysics3DSystem::GetPhysWorld()
	{
		return m_physWorld;
	}

	inline const JoltPhysWorld3D& JoltPhysics3DSystem::GetPhysWorld() const
	{
		return m_physWorld;
	}
}

#include <Nazara/JoltPhysics3D/DebugOff.hpp>
