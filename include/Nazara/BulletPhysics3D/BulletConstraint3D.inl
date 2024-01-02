// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - BulletPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/BulletPhysics3D/Debug.hpp>

namespace Nz
{
	inline bool BulletConstraint3D::IsBodyCollisionEnabled() const
	{
		return m_bodyCollisionEnabled;
	}

	template<typename T>
	T* BulletConstraint3D::GetConstraint()
	{
		return SafeCast<T*>(m_constraint.get());
	}

	template<typename T>
	const T* BulletConstraint3D::GetConstraint() const
	{
		return SafeCast<const T*>(m_constraint.get());
	}
}

#include <Nazara/BulletPhysics3D/DebugOff.hpp>
