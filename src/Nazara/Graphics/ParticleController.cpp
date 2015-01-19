// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ParticleController.hpp>
#include <Nazara/Graphics/Debug.hpp>

NzParticleController::NzParticleController(const NzParticleController& controller) :
NzResource()
{
	NazaraUnused(controller);
}

NzParticleController::~NzParticleController() = default;
