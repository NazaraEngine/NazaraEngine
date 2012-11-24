// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - 3D Module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/3D/ModuleName.hpp>
#include <Nazara/3D/Config.hpp>
#include <Nazara/3D/Debug.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>

bool Nz3D::Initialize()
{
	if (s_moduleReferenceCouter++ != 0)
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

bool Nz3D::IsInitialized()
{
	return s_moduleReferenceCouter != 0;
}

void Nz3D::Uninitialize()
{
	if (--s_moduleReferenceCouter != 0)
		return; // Encore utilisé

	// Libération du module

	NazaraNotice("Uninitialized: ModuleName module");

	// Libération des dépendances
	NzCore::Uninitialize();
}

unsigned int Nz3D::s_moduleReferenceCouter = 0;
