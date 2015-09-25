// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ParticleGenerator.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	ParticleGenerator::ParticleGenerator(const ParticleGenerator& generator) :
	RefCounted()
	{
		NazaraUnused(generator);
	}

	ParticleGenerator::~ParticleGenerator()
	{
		OnParticleGeneratorRelease(this);
	}

	bool ParticleGenerator::Initialize()
	{
		if (!ParticleGeneratorLibrary::Initialize())
		{
			NazaraError("Failed to initialise library");
			return false;
		}

		return true;
	}

	void ParticleGenerator::Uninitialize()
	{
		ParticleGeneratorLibrary::Uninitialize();
	}

	ParticleGeneratorLibrary::LibraryMap ParticleGenerator::s_library;
}
