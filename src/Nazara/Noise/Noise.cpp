// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Noise/Noise.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Noise/Config.hpp>
#include <Nazara/Noise/Debug.hpp>

NzNoise::NzNoise()
{
}

NzNoise::~NzNoise()
{
	if (s_initialized)
		Uninitialize();
}

bool NzNoise::Initialize()
{
	#if NAZARA_NOISE_SAFE
	if (s_initialized)
	{
		NazaraError("NzNoise already initialized");
		return true;
	}
	#endif

	// Initialisation du module

	s_initialized = true;

	return true;
}

void NzNoise::Uninitialize()
{
	#if NAZARA_NOISE_SAFE
	if (!s_initialized)
	{
		NazaraError("NzNoise not initialized");
		return;
	}
	#endif

	// Libération du module

	s_initialized = false;
}

bool NzNoise::IsInitialized()
{
	return s_initialized;
}

bool NzNoise::s_initialized = false;

//#include <Nazara/Core/DebugOff.hpp> //A INCLURE ?
