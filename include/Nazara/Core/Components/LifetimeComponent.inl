// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Components/LifetimeComponent.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	inline LifetimeComponent::LifetimeComponent(float lifetime) :
	m_remainingLifetime(lifetime)
	{
	}

	inline void LifetimeComponent::DecreaseLifetime(float elapsedTime)
	{
		m_remainingLifetime -= elapsedTime;
	}

	inline float LifetimeComponent::GetRemainingLifeTime() const
	{
		return m_remainingLifetime;
	}

	inline bool LifetimeComponent::IsAlive() const
	{
		return m_remainingLifetime >= 0.f;
	}
}

#include <Nazara/Core/DebugOff.hpp>
