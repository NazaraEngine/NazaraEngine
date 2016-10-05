// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Components/ParticleEmitterComponent.hpp>

namespace Ndk
{
	/*!
	* \brief Constructs an ParticleEmitterComponent object by default
	*/

	inline ParticleEmitterComponent::ParticleEmitterComponent() :
	m_isActive(true)
	{
	}

	/*!
	* \brief Enables the emission of particles
	*
	* \param active Should the emitter be active
	*/

	inline void Ndk::ParticleEmitterComponent::Enable(bool active)
	{
		m_isActive = active;
	}

	/*!
	* \brief Checks whether the emission of particles is activated
	* \param true If it is the case
	*/

	inline bool ParticleEmitterComponent::IsActive() const
	{
		return m_isActive;
	}

	/*!
	* \brief Sets the function use for setting up particles
	*
	* \param func Function to set up particles
	*/

	inline void Ndk::ParticleEmitterComponent::SetSetupFunc(SetupFunc func)
	{
		m_setupFunc = std::move(func);
	}
}
