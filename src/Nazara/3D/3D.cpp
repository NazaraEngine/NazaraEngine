// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - 3D Module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/3D/3D.hpp>
#include <Nazara/3D/Config.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/3D/Debug.hpp>

bool Nz3D::Initialize()
{
	if (s_moduleReferenceCounter++ != 0)
		return true; // Déjà initialisé

	// Initialisation des dépendances
	if (!NzRenderer::Initialize())
	{
		NazaraError("Failed to initialize renderer module");
		return false;
	}

	// Initialisation du module

	NazaraNotice("Initialized: 3D module");

	return true;
}

bool Nz3D::IsInitialized()
{
	return s_moduleReferenceCounter != 0;
}

void Nz3D::Uninitialize()
{
	if (--s_moduleReferenceCounter != 0)
		return; // Encore utilisé

	// Libération du module

	NazaraNotice("Uninitialized: 3D module");

	// Libération des dépendances
	NzRenderer::Uninitialize();
}

unsigned int Nz3D::s_moduleReferenceCounter = 0;
