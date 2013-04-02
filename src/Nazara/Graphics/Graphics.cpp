// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/Loaders/Mesh.hpp>
#include <Nazara/Graphics/Loaders/OBJ.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Graphics/Debug.hpp>

bool Nz3D::Initialize()
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

bool Nz3D::IsInitialized()
{
	return s_moduleReferenceCounter != 0;
}

void Nz3D::Uninitialize()
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
	Nz2D::Uninitialize();
}

unsigned int Nz3D::s_moduleReferenceCounter = 0;
