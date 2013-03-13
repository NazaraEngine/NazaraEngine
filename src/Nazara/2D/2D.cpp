// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - 2D Module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/2D/2D.hpp>
#include <Nazara/2D/Config.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/2D/Debug.hpp>

bool Nz2D::Initialize()
{
	if (s_moduleReferenceCounter++ != 0)
		return true; // Déjà initialisé

	// Initialisation des dépendances
	if (!NzRenderer::Initialize())
	{
		NazaraError("Failed to initialize renderer module");
		Uninitialize();

		return false;
	}

	// Initialisation du module

	NazaraNotice("Initialized: 2D module");

	return true;
}

bool Nz2D::IsInitialized()
{
	return s_moduleReferenceCounter != 0;
}

void Nz2D::Uninitialize()
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

	NazaraNotice("Uninitialized: 2D module");

	// Libération des dépendances
	NzRenderer::Uninitialize();
}

unsigned int Nz2D::s_moduleReferenceCounter = 0;
