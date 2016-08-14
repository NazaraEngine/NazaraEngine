// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ParticleFunctionController.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup graphics
	* \class Nz::ParticleFunctionController
	* \brief Helper class used to provide a function as a particle controller without going in the process of making a new class
	*/

	/*!
	* \brief Calls the controller function
	*
	* \param group Particle group responsible of the particles
	* \param mapper Particle mapper, allowing access to the particle data
	* \param startId The first ID of the particle to update (inclusive)
	* \param endId The last ID of the particle to update (inclusive)
	* \param elapsedTime Elapsed time in seconds since the last update
	*/
	void ParticleFunctionController::Apply(ParticleGroup& group, ParticleMapper& mapper, unsigned int startId, unsigned int endId, float elapsedTime)
	{
		m_controller(group, mapper, startId, endId, elapsedTime);
	}
}
