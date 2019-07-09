// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NDK/Components/ParticleEmitterComponent.hpp>
#include <Nazara/Graphics/ParticleGroup.hpp>

namespace Ndk
{
	/*!
	* \ingroup NDK
	* \class Ndk::ParticleEmitterComponent
	* \brief NDK class that represents the component emitter of particles
	*/

	/*!
	* \brief Sets up the particles
	*
	* \param mapper Mapper containing layout information of each particle
	* \param count Number of particles
	*/

	void ParticleEmitterComponent::SetupParticles(Nz::ParticleMapper& mapper, unsigned int count) const
	{
		if (m_isActive && m_setupFunc)
			m_setupFunc(m_entity, mapper, count);
	}

	ComponentIndex ParticleEmitterComponent::componentIndex;
}
