// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ParticleController.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup graphics
	* \class Nz::ParticleController
	* \brief Graphics class which controls a flow of particles
	*
	* \remark This class is abstract
	*/

	/*!
	* \brief Constructs a ParticleController object by assignation
	*
	* \param controller ParticleController to copy into this
	*/

	ParticleController::ParticleController(const ParticleController& controller) :
	RefCounted()
	{
		NazaraUnused(controller);
	}

	/*!
	* \brief Destructs the object and calls OnParticleControllerRelease
	*
	* \see OnParticleControllerRelease
	*/

	ParticleController::~ParticleController()
	{
		OnParticleControllerRelease(this);
	}

	/*!
	* \brief Initializes the particle controller librairies
	* \return true If successful
	*
	* \remark Produces a NazaraError if the particle controller library failed to be initialized
	*/

	bool ParticleController::Initialize()
	{
		if (!ParticleControllerLibrary::Initialize())
		{
			NazaraError("Failed to initialise library");
			return false;
		}

		return true;
	}

	/*!
	* \brief Uninitializes the particle controller librairies
	*/

	void ParticleController::Uninitialize()
	{
		ParticleControllerLibrary::Uninitialize();
	}

	ParticleControllerLibrary::LibraryMap ParticleController::s_library;
}
