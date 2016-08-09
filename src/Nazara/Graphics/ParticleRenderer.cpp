// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ParticleRenderer.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup graphics
	* \class Nz::ParticleRenderer
	* \brief Graphics class that represents the rendering of the particle
	*/

	/*!
	* \brief Constructs a ParticleRenderer object by assignation
	*
	* \param renderer ParticleRenderer to copy into this
	*/

	ParticleRenderer::ParticleRenderer(const ParticleRenderer& renderer) :
	RefCounted()
	{
		NazaraUnused(renderer);
	}

	/*!
	* \brief Destructs the object and calls OnParticleRendererRelease
	*
	* \see OnParticleRendererRelease
	*/

	ParticleRenderer::~ParticleRenderer()
	{
		OnParticleRendererRelease(this);
	}

	/*!
	* \brief Initializes the particle renderer librairies
	* \return true If successful
	*
	* \remark Produces a NazaraError if the particle renderer library failed to be initialized
	*/

	bool ParticleRenderer::Initialize()
	{
		if (!ParticleRendererLibrary::Initialize())
		{
			NazaraError("Failed to initialise library");
			return false;
		}

		return true;
	}

	/*!
	* \brief Uninitializes the particle renderer librairies
	*/

	void ParticleRenderer::Uninitialize()
	{
		ParticleRendererLibrary::Uninitialize();
	}

	ParticleRendererLibrary::LibraryMap ParticleRenderer::s_library;
}
