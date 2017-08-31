// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ParticleGenerator.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup graphics
	* \class Nz::ParticleGenerator
	* \brief Graphics class which generates particles
	*
	* \remark This class is abstract
	*/

	/*!
	* \brief Constructs a ParticleGenerator object by assignation
	*
	* \param generator ParticleGenerator to copy into this
	*/

	ParticleGenerator::ParticleGenerator(const ParticleGenerator& generator) :
	RefCounted()
	{
		NazaraUnused(generator);
	}

	/*!
	* \brief Destructs the object and calls OnParticleGeneratorRelease
	*
	* \see OnParticleGeneratorRelease
	*/

	ParticleGenerator::~ParticleGenerator()
	{
		OnParticleGeneratorRelease(this);
	}

	/*!
	* \brief Initializes the particle generator librairies
	* \return true If successful
	*
	* \remark Produces a NazaraError if the particle generator library failed to be initialized
	*/

	bool ParticleGenerator::Initialize()
	{
		if (!ParticleGeneratorLibrary::Initialize())
		{
			NazaraError("Failed to initialise library");
			return false;
		}

		return true;
	}

	/*!
	* \brief Uninitializes the particle generator librairies
	*/

	void ParticleGenerator::Uninitialize()
	{
		ParticleGeneratorLibrary::Uninitialize();
	}

	ParticleGeneratorLibrary::LibraryMap ParticleGenerator::s_library;
}
