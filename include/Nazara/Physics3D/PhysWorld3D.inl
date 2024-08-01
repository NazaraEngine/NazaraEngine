// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	inline bool PhysWorld3D::IsBodyActive(UInt32 bodyIndex) const
	{
		UInt32 blockIndex = bodyIndex / 64;
		UInt32 localIndex = bodyIndex % 64;

		return m_activeBodies[blockIndex] & (UInt64(1u) << localIndex);
	}

	inline bool PhysWorld3D::IsBodyRegistered(UInt32 bodyIndex) const
	{
		UInt32 blockIndex = bodyIndex / 64;
		UInt32 localIndex = bodyIndex % 64;

		return m_registeredBodies[blockIndex] & (UInt64(1u) << localIndex);
	}

	inline void PhysWorld3D::RegisterStepListener(PhysWorld3DStepListener* stepListener)
	{
		auto it = std::lower_bound(m_stepListeners.begin(), m_stepListeners.end(), stepListener);
		m_stepListeners.insert(it, stepListener);
	}

	inline void PhysWorld3D::UnregisterStepListener(PhysWorld3DStepListener* stepListener)
	{
		auto it = std::lower_bound(m_stepListeners.begin(), m_stepListeners.end(), stepListener);
		assert(*it == stepListener);
		m_stepListeners.erase(it);
	}
}

