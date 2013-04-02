// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/Loaders/Mesh.hpp>
#include <Nazara/Graphics/Loaders/OBJ.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Graphics/Debug.hpp>

bool NzGraphics::Initialize()
{
	if (s_moduleReferenceCounter++ != 0)
		return true; // Déjà initialisé

	// Initialisation des dépendances
	if (!NzRenderer::Initialize())
	{
		NazaraError("Failed to initialize Renderer module");
		Uninitialize();

		return false;
	}

	// Initialisation du module

	// Loaders
	NzLoaders_OBJ_Register();

	// Loader générique
	NzLoaders_Mesh_Register();

	NazaraNotice("Initialized: Graphics module");

	return true;
}

bool NzGraphics::IsInitialized()
{
	return s_moduleReferenceCounter != 0;
}

void NzGraphics::Uninitialize()
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

	// Loaders
	NzLoaders_Mesh_Unregister();
	NzLoaders_OBJ_Unregister();

	NazaraNotice("Uninitialized: Graphics module");

	// Libération des dépendances
	NzRenderer::Uninitialize();
}

unsigned int NzGraphics::s_moduleReferenceCounter = 0;
