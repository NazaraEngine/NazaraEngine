// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	inline LifetimeComponent::LifetimeComponent(Time lifetime) :
	m_remainingLifetime(lifetime)
	{
	}

	inline void LifetimeComponent::DecreaseLifetime(Time elapsedTime)
	{
		m_remainingLifetime -= elapsedTime;
	}

	inline Time LifetimeComponent::GetRemainingLifeTime() const
	{
		return m_remainingLifetime;
	}

	inline bool LifetimeComponent::IsAlive() const
	{
		return m_remainingLifetime >= Time::Zero();
	}
}

#include <Nazara/Core/DebugOff.hpp>
