// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics2D module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <cassert>

namespace Nz
{
	inline UInt32 PhysWorld2D::RegisterBody(RigidBody2D& rigidBody)
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

	inline void PhysWorld2D::UnregisterBody(UInt32 bodyIndex)
	{
		assert(!m_freeBodyIndices.Test(bodyIndex));
		m_freeBodyIndices.Set(bodyIndex, true);
		assert(m_bodies[bodyIndex]);
		m_bodies[bodyIndex] = nullptr;

		m_rigidBodyPostSteps.erase(bodyIndex);
	}

	inline void PhysWorld2D::UpdateBodyPointer(RigidBody2D& rigidBody)
	{
		UInt32 bodyIndex = rigidBody.GetBodyIndex();
		assert(m_bodies[bodyIndex]);
		m_bodies[bodyIndex] = &rigidBody;
	}
}
