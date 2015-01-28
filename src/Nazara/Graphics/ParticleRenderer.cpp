// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ParticleRenderer.hpp>
#include <Nazara/Graphics/Debug.hpp>

NzParticleRenderer::NzParticleRenderer(const NzParticleRenderer& renderer) :
NzRefCounted()
{
	NazaraUnused(renderer);
}

NzParticleRenderer::~NzParticleRenderer() = default;

bool NzParticleRenderer::Initialize()
{
	if (!NzParticleRendererLibrary::Initialize())
	{
		NazaraError("Failed to initialise library");
		return false;
	}

	return true;
}

void NzParticleRenderer::Uninitialize()
{
	NzParticleRendererLibrary::Uninitialize();
}

NzParticleRendererLibrary::LibraryMap NzParticleRenderer::s_library;
