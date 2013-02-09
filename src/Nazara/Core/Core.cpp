// Copyright (C) 2012 AUTHORS
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/HardwareInfo.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Core/TaskScheduler.hpp>
#include <Nazara/Core/Debug.hpp>

bool NzCore::Initialize(bool initializeHardwareInfo, bool initializeTaskScheduler)
{
	s_moduleReferenceCounter++;

	// Initialisation du module
	if (initializeHardwareInfo && !NzHardwareInfo::Initialize())
		NazaraWarning("Failed to initialize hardware info"); // Non-critique

	if (initializeTaskScheduler && !NzTaskScheduler::Initialize())
	{
		NazaraError("Failed to initialize task scheduler");
		Uninitialize();

		return false;
	}

	// Vérification après l'initialisation des sous-modules
	if (s_moduleReferenceCounter != 1)
		return true; // Déjà initialisé

	NazaraNotice("Initialized: Core");

	return true;
}

bool NzCore::IsInitialized()
{
	return s_moduleReferenceCounter != 0;
}

void NzCore::Uninitialize()
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

	NzHardwareInfo::Uninitialize();
	NzTaskScheduler::Uninitialize();

	NazaraNotice("Uninitialized: Core");
}

unsigned int NzCore::s_moduleReferenceCounter = 0;
