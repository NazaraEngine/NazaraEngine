// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ParticleController.hpp>
#include <Nazara/Graphics/Debug.hpp>

NzParticleController::NzParticleController(const NzParticleController& controller) :
NzRefCounted()
{
	NazaraUnused(controller);
}

NzParticleController::~NzParticleController() = default;

bool NzParticleController::Initialize()
{
	if (!NzParticleControllerLibrary::Initialize())
	{
		NazaraError("Failed to initialise library");
		return false;
	}

	return true;
}

void NzParticleController::Uninitialize()
{
	NzParticleControllerLibrary::Uninitialize();
}

NzParticleControllerLibrary::LibraryMap NzParticleController::s_library;
