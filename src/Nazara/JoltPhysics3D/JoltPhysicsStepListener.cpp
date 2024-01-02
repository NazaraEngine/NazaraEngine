// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - JoltPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/JoltPhysics3D/JoltPhysicsStepListener.hpp>
#include <Nazara/JoltPhysics3D/Debug.hpp>

namespace Nz
{
	JoltPhysicsStepListener::~JoltPhysicsStepListener() = default;

	void JoltPhysicsStepListener::PostSimulate(float /*elapsedTime*/)
	{
	}

	void JoltPhysicsStepListener::PreSimulate(float /*elapsedTime*/)
	{
	}
}
