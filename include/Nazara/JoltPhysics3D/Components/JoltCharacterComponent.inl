// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - JoltPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/JoltPhysics3D/Debug.hpp>

namespace Nz
{
	inline JoltCharacterComponent::JoltCharacterComponent(const JoltCharacter::Settings& settings)
	{
		m_settings = std::make_unique<JoltCharacter::Settings>(settings);
	}

	inline void JoltCharacterComponent::Construct(JoltPhysWorld3D& world)
	{
		assert(m_settings);
		Create(world, *m_settings);
		m_settings.reset();
	}
}

#include <Nazara/JoltPhysics3D/DebugOff.hpp>
