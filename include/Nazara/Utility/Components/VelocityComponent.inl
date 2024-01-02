// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	inline VelocityComponent::VelocityComponent(const Vector3f& linearVelocity) :
	m_linearVelocity(linearVelocity)
	{
	}

	inline const Vector3f& VelocityComponent::GetLinearVelocity() const
	{
		return m_linearVelocity;
	}

	inline void VelocityComponent::UpdateLinearVelocity(const Vector3f& linearVelocity)
	{
		m_linearVelocity = linearVelocity;
	}
}

#include <Nazara/Utility/DebugOff.hpp>
