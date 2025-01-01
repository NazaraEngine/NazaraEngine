// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	inline std::size_t PhysWorld3D::GetMaxStepCount() const
	{
		return m_maxStepCount;
	}

	inline Time PhysWorld3D::GetStepSize() const
	{
		return m_stepSize;
	}

	inline Time PhysWorld3D::GetTimestepAccumulator() const
	{
		return m_timestepAccumulator;
	}

	inline void PhysWorld3D::SetMaxStepCount(std::size_t maxStepCount)
	{
		m_maxStepCount = maxStepCount;
	}

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
