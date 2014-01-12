// Copyright (C) 2014 AUTHORS
// This file is part of the "Nazara Engine - Module name"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/ModuleName/ModuleName.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/ModuleName/Config.hpp>
#include <Nazara/ModuleName/Debug.hpp>

bool NzModuleName::Initialize()
{
	if (s_moduleReferenceCounter++ != 0)
		return true; // Déjà initialisé

	// Initialisation des dépendances
	if (!NzCore::Initialize())
	{
		NazaraError("Failed to initialize core module");
		return false;
	}

	// Initialisation du module

	NazaraNotice("Initialized: ModuleName module");

	return true;
}

bool NzModuleName::IsInitialized()
{
	return s_moduleReferenceCounter != 0;
}

void NzModuleName::Uninitialize()
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

	NazaraNotice("Uninitialized: ModuleName module");

	// Libération des dépendances
	NzCore::Uninitialize();
}

unsigned int NzModuleName::s_moduleReferenceCounter = 0;
