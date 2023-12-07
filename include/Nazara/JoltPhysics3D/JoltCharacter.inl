// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - JoltPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/JoltPhysics3D/Debug.hpp>

namespace Nz
{
	inline void JoltCharacter::DisableSleeping()
	{
		return EnableSleeping(false);
	}

	inline const std::shared_ptr<JoltCollider3D>& JoltCharacter::GetCollider() const
	{
		return m_collider;
	}

	inline JoltPhysWorld3D& JoltCharacter::GetPhysWorld()
	{
		return *m_world;
	}

	inline const JoltPhysWorld3D& JoltCharacter::GetPhysWorld() const
	{
		return *m_world;
	}

	inline void JoltCharacter::SetImpl(std::shared_ptr<JoltCharacterImpl> characterImpl)
	{
		m_impl = std::move(characterImpl);
	}
}

#include <Nazara/JoltPhysics3D/DebugOff.hpp>
