// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Components/ParticleEmitterComponent.hpp>
#include <Nazara/Graphics/ParticleGroup.hpp>

namespace Ndk
{
	void ParticleEmitterComponent::SetupParticles(Nz::ParticleMapper& mapper, unsigned int count) const
	{
		if (m_isActive && m_setupFunc)
			m_setupFunc(m_entity, mapper, count);
	}

	ComponentIndex ParticleEmitterComponent::componentIndex;
}
