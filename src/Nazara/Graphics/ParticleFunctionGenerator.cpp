// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ParticleFunctionGenerator.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup graphics
	* \class Nz::ParticleFunctionGenerator
	* \brief Helper class used to provide a function as a particle generator without going in the process of making a new class
	*/

	/*!
	* \brief Calls the generator function
	*
	* \param group Particle group responsible of the particles
	* \param mapper Particle mapper, allowing access to the particle data
	* \param startId The first ID of the particle to update (inclusive)
	* \param endId The last ID of the particle to update (inclusive)
	*/
	void ParticleFunctionGenerator::Generate(ParticleGroup& group, ParticleMapper& mapper, unsigned int startId, unsigned int endId)
	{
		m_generator(group, mapper, startId, endId);
	}
}
