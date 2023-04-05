// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - JoltPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/JoltPhysics3D/Debug.hpp>

namespace Nz
{
	inline bool JoltPhysWorld3D::IsBodyActive(UInt32 bodyIndex) const
	{
		UInt32 blockIndex = bodyIndex / 64;
		UInt32 localIndex = bodyIndex % 64;

		return m_activeBodies[blockIndex] & (UInt64(1u) << localIndex);
	}

	inline bool JoltPhysWorld3D::IsBodyRegistered(UInt32 bodyIndex) const
	{
		UInt32 blockIndex = bodyIndex / 64;
		UInt32 localIndex = bodyIndex % 64;

		return m_registeredBodies[blockIndex] & (UInt64(1u) << localIndex);
	}

	inline void JoltPhysWorld3D::RegisterCharacter(JoltCharacter* character)
	{
		auto it = std::lower_bound(m_characters.begin(), m_characters.end(), character);
		m_characters.insert(it, character);
	}

	inline void JoltPhysWorld3D::UnregisterCharacter(JoltCharacter* character)
	{
		auto it = std::lower_bound(m_characters.begin(), m_characters.end(), character);
		assert(*it == character);
		m_characters.erase(it);
	}
}

#include <Nazara/JoltPhysics3D/DebugOff.hpp>
