// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Physics3D/PhysWorld3DStepListener.hpp>

namespace Nz
{
	PhysWorld3DStepListener::~PhysWorld3DStepListener() = default;

	void PhysWorld3DStepListener::PostSimulate(float /*elapsedTime*/)
	{
	}

	void PhysWorld3DStepListener::PreSimulate(float /*elapsedTime*/)
	{
	}
}
