// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ParticleFunctionRenderer.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup graphics
	* \class Nz::ParticleFunctionRenderer
	* \brief Helper class used to provide a function as a particle renderer without going in the process of making a new class
	*/

	/*!
	* \brief Calls the renderer function
	*
	* \param group Particle group responsible of the particles
	* \param mapper Particle mapper, allowing constant access to the particle data
	* \param startId The first ID of the particle to update (inclusive)
	* \param endId The last ID of the particle to update (inclusive)
	* \param renderQueue The concerned render queue that will receive drawable informations
	*/
	void ParticleFunctionRenderer::Render(const ParticleGroup& group, const ParticleMapper& mapper, unsigned int startId, unsigned int endId, AbstractRenderQueue* renderQueue)
	{
		m_renderer(group, mapper, startId, endId, renderQueue);
	}
}
