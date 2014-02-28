// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics/Physics.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Physics/Config.hpp>
#include <Newton/Newton.h>
#include <Nazara/Physics/Debug.hpp>

unsigned int NzPhysics::GetMemoryUsed()
{
	return NewtonGetMemoryUsed();
}

bool NzPhysics::Initialize()
{
	if (s_moduleReferenceCounter > 0)
	{
		s_moduleReferenceCounter++;
		return true; // Déjà initialisé
	}

	// Initialisation des dépendances
	if (!NzCore::Initialize())
	{
		NazaraError("Failed to initialize core module");
		return false;
	}

	s_moduleReferenceCounter++;

	// Initialisation du module

	NazaraNotice("Initialized: Physics module");
	return true;
}

bool NzPhysics::IsInitialized()
{
	return s_moduleReferenceCounter != 0;
}

void NzPhysics::Uninitialize()
{
	if (s_moduleReferenceCounter != 1)
	{
		// Le module est soit encore utilisé, soit pas initialisé
		if (s_moduleReferenceCounter > 1)
			s_moduleReferenceCounter--;

		return;
	}

	// Libération du module
	s_moduleReferenceCounter = 0;

	NazaraNotice("Uninitialized: Physics module");

	// Libération des dépendances
	NzCore::Uninitialize();
}

unsigned int NzPhysics::s_moduleReferenceCounter = 0;
