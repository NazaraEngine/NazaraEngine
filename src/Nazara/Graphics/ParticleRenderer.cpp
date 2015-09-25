// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ParticleRenderer.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	ParticleRenderer::ParticleRenderer(const ParticleRenderer& renderer) :
	RefCounted()
	{
		NazaraUnused(renderer);
	}

	ParticleRenderer::~ParticleRenderer()
	{
		OnParticleRendererRelease(this);
	}

	bool ParticleRenderer::Initialize()
	{
		if (!ParticleRendererLibrary::Initialize())
		{
			NazaraError("Failed to initialise library");
			return false;
		}

		return true;
	}

	void ParticleRenderer::Uninitialize()
	{
		ParticleRendererLibrary::Uninitialize();
	}

	ParticleRendererLibrary::LibraryMap ParticleRenderer::s_library;
}
