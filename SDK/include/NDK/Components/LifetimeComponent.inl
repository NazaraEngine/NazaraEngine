// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NDK/Components/LifetimeComponent.hpp>

namespace Ndk
{
	inline LifetimeComponent::LifetimeComponent(float lifetime) :
	m_lifetime(lifetime)
	{
	}

	inline float Ndk::LifetimeComponent::GetRemainingTime() const
	{
		return m_lifetime;
	}

	inline bool LifetimeComponent::UpdateLifetime(float elapsedTime)
	{
		m_lifetime -= elapsedTime;
		return m_lifetime < 0.f;
	}
}
