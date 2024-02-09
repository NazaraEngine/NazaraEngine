// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics3D/Physiscs3DStepListener.hpp>
#include <Nazara/Physics3D/Debug.hpp>

namespace Nz
{
	Physiscs3DStepListener::~Physiscs3DStepListener() = default;

	void Physiscs3DStepListener::PostSimulate(float /*elapsedTime*/)
	{
	}

	void Physiscs3DStepListener::PreSimulate(float /*elapsedTime*/)
	{
	}
}
