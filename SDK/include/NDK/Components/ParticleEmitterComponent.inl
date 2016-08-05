// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Components/ParticleEmitterComponent.hpp>

namespace Ndk
{
	inline ParticleEmitterComponent::ParticleEmitterComponent() :
	m_isActive(true)
	{
	}

	inline void Ndk::ParticleEmitterComponent::Enable(bool active)
	{
		m_isActive = active;
	}

	inline bool ParticleEmitterComponent::IsActive() const
	{
		return m_isActive;
	}

	inline void Ndk::ParticleEmitterComponent::SetSetupFunc(SetupFunc func)
	{
		m_setupFunc = std::move(func);
	}
}
