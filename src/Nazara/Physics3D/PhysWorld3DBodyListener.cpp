// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Physics3D/PhysWorld3DBodyListener.hpp>

namespace Nz
{
	PhysWorld3DBodyListener::~PhysWorld3DBodyListener() = default;

	void PhysWorld3DBodyListener::OnBodyActivated(UInt32 /*bodyIndex*/)
	{
	}

	void PhysWorld3DBodyListener::OnBodyDeactivated(UInt32 /*bodyIndex*/)
	{
	}
}
