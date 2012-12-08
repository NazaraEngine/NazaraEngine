// Copyright (C) 2012 AUTHORS
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Core/Debug.hpp>

bool NzCore::Initialize()
{
	if (s_moduleReferenceCounter++ != 0)
		return true; // Déjà initialisé

	// Initialisation du module
	// Le noyau de Nazara n'a pour l'instant aucun besoin d'initialisation, mais dans le futur il est très probable que ce soit le cas.
	// Donc en prévision, tous les modules initialisent le noyau

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

	NazaraNotice("Uninitialized: Core");
}

unsigned int NzCore::s_moduleReferenceCounter = 0;
