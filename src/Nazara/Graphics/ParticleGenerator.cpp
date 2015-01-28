// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ParticleGenerator.hpp>
#include <Nazara/Graphics/Debug.hpp>

NzParticleGenerator::NzParticleGenerator(const NzParticleGenerator& generator) :
NzRefCounted()
{
	NazaraUnused(generator);
}

NzParticleGenerator::~NzParticleGenerator() = default;

bool NzParticleGenerator::Initialize()
{
	if (!NzParticleGeneratorLibrary::Initialize())
	{
		NazaraError("Failed to initialise library");
		return false;
	}

	return true;
}

void NzParticleGenerator::Uninitialize()
{
	NzParticleGeneratorLibrary::Uninitialize();
}

NzParticleGeneratorLibrary::LibraryMap NzParticleGenerator::s_library;
