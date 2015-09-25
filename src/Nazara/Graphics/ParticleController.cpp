// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ParticleController.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	ParticleController::ParticleController(const ParticleController& controller) :
	RefCounted()
	{
		NazaraUnused(controller);
	}

	ParticleController::~ParticleController()
	{
		OnParticleControllerRelease(this);
	}

	bool ParticleController::Initialize()
	{
		if (!ParticleControllerLibrary::Initialize())
		{
			NazaraError("Failed to initialise library");
			return false;
		}

		return true;
	}

	void ParticleController::Uninitialize()
	{
		ParticleControllerLibrary::Uninitialize();
	}

	ParticleControllerLibrary::LibraryMap ParticleController::s_library;
}
