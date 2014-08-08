// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ParticleRenderer.hpp>
#include <Nazara/Graphics/Debug.hpp>

NzParticleRenderer::NzParticleRenderer(const NzParticleRenderer& renderer) :
NzResource()
{
	NazaraUnused(renderer);
}

NzParticleRenderer::~NzParticleRenderer() = default;
