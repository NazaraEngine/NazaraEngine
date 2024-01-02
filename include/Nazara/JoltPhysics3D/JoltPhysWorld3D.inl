// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - JoltPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/JoltPhysics3D/Debug.hpp>

namespace Nz
{
	inline bool JoltPhysWorld3D::IsBodyActive(UInt32 bodyIndex) const
	{
		UInt32 blockIndex = bodyIndex / 64;
		UInt32 localIndex = bodyIndex % 64;

		return m_activeBodies[blockIndex] & (UInt64(1u) << localIndex);
	}

	inline bool JoltPhysWorld3D::IsBodyRegistered(UInt32 bodyIndex) const
	{
		UInt32 blockIndex = bodyIndex / 64;
		UInt32 localIndex = bodyIndex % 64;

		return m_registeredBodies[blockIndex] & (UInt64(1u) << localIndex);
	}

	inline void JoltPhysWorld3D::RegisterStepListener(JoltPhysicsStepListener* stepListener)
	{
		auto it = std::lower_bound(m_stepListeners.begin(), m_stepListeners.end(), stepListener);
		m_stepListeners.insert(it, stepListener);
	}

	inline void JoltPhysWorld3D::UnregisterStepListener(JoltPhysicsStepListener* stepListener)
	{
		auto it = std::lower_bound(m_stepListeners.begin(), m_stepListeners.end(), stepListener);
		assert(*it == stepListener);
		m_stepListeners.erase(it);
	}
}

#include <Nazara/JoltPhysics3D/DebugOff.hpp>
