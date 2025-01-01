// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp


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
