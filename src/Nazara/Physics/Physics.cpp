// Copyright (C) 2012 Jérôme Leclercq / Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics/Physics.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Physics/Config.hpp>
#include <Nazara/Physics/Debug.hpp>

NzPhysics::NzPhysics()
{
}

NzPhysics::~NzPhysics()
{
	if (s_initialized)
		Uninitialize();
}

bool NzPhysics::Initialize()
{
	#if NAZARA_PHYSICS_SAFE
	if (s_initialized)
	{
		NazaraError("Physics already initialized");
		return true;
	}
	#endif

	// Initialisation du module

	s_initialized = true;

	return true;
}

void NzPhysics::Uninitialize()
{
	#if NAZARA_PHYSICS_SAFE
	if (!s_initialized)
	{
		NazaraError("Physics not initialized");
		return;
	}
	#endif

	// Libération du module

	s_initialized = false;
}

bool NzPhysics::IsInitialized()
{
	return s_initialized;
}

bool NzPhysics::s_initialized = false;
