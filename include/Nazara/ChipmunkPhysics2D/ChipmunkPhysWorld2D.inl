// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - ChipmunkPhysics2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <cassert>
#include <Nazara/ChipmunkPhysics2D/Debug.hpp>

namespace Nz
{
	inline UInt32 ChipmunkPhysWorld2D::RegisterBody(ChipmunkRigidBody2D& rigidBody)
	{
		std::size_t bodyIndex = m_freeBodyIndices.FindFirst();
		if (bodyIndex == m_freeBodyIndices.npos)
		{
			bodyIndex = m_freeBodyIndices.GetSize();
			m_freeBodyIndices.Resize(bodyIndex + FreeBodyIdGrowRate, true);
			m_bodies.resize(m_freeBodyIndices.GetSize());
		}

		assert(m_freeBodyIndices.Test(bodyIndex));
		m_freeBodyIndices.Set(bodyIndex, false);

		assert(!m_bodies[bodyIndex]);
		m_bodies[bodyIndex] = &rigidBody;

		return SafeCast<UInt32>(bodyIndex);
	}

	inline void ChipmunkPhysWorld2D::UnregisterBody(UInt32 bodyIndex)
	{
		assert(!m_freeBodyIndices.Test(bodyIndex));
		m_freeBodyIndices.Set(bodyIndex, true);
		assert(m_bodies[bodyIndex]);
		m_bodies[bodyIndex] = nullptr;

		m_rigidBodyPostSteps.erase(bodyIndex);
	}

	inline void ChipmunkPhysWorld2D::UpdateBodyPointer(ChipmunkRigidBody2D& rigidBody)
	{
		UInt32 bodyIndex = rigidBody.GetBodyIndex();
		assert(m_bodies[bodyIndex]);
		m_bodies[bodyIndex] = &rigidBody;
	}
}

#include <Nazara/ChipmunkPhysics2D/DebugOff.hpp>
