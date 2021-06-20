// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NazaraSDK/Components/ConstraintComponent2D.hpp>

namespace Ndk
{
	ConstraintComponent2D::ConstraintComponent2D(const ConstraintComponent2D& /*joint*/)
	{
	}

	bool ConstraintComponent2D::RemoveConstraint(Nz::Constraint2D* constraintPtr)
	{
		auto it = std::find_if(m_constraints.begin(), m_constraints.end(), [constraintPtr](const ConstraintData& constraintData) { return constraintData.constraint.get() == constraintPtr; });
		if (it != m_constraints.end())
			m_constraints.erase(it);

		return !m_constraints.empty();
	}

	ComponentIndex ConstraintComponent2D::componentIndex;
}
