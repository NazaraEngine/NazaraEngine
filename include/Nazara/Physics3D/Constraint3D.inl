// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics3D/Constraint3D.hpp>
#include <Nazara/Physics3D/Debug.hpp>
#include <Nazara/Physics3D/Debug.hpp>

namespace Nz
{
	inline bool Constraint3D::IsBodyCollisionEnabled() const
	{
		return m_bodyCollisionEnabled;
	}

	template<typename T>
	T* Constraint3D::GetConstraint()
	{
		return SafeCast<T*>(m_constraint.get());
	}

	template<typename T>
	const T* Constraint3D::GetConstraint() const
	{
		return SafeCast<const T*>(m_constraint.get());
	}
}

#include <Nazara/Physics3D/DebugOff.hpp>
