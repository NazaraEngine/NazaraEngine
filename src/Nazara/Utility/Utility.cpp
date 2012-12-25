// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Utility.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Utility/Buffer.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Loaders/MD2.hpp>
#include <Nazara/Utility/Loaders/MD5Anim.hpp>
#include <Nazara/Utility/Loaders/MD5Mesh.hpp>
#include <Nazara/Utility/Loaders/PCX.hpp>
#include <Nazara/Utility/Loaders/STB.hpp>
#include <Nazara/Utility/PixelFormat.hpp>
#include <Nazara/Utility/Window.hpp>
#include <Nazara/Utility/Debug.hpp>

bool NzUtility::Initialize()
{
	if (s_moduleReferenceCounter++ != 0)
		return true; // Déjà initialisé

	// Initialisation des dépendances
	if (!NzCore::Initialize())
	{
		NazaraError("Failed to initialize core module");
		Uninitialize();

		return false;
	}

	// Initialisation du module
	if (!NzBuffer::Initialize())
	{
		NazaraError("Failed to initialize buffers");
		Uninitialize();

		return false;
	}

	if (!NzPixelFormat::Initialize())
	{
		NazaraError("Failed to initialize pixel formats");
		Uninitialize();

		return false;
	}

	if (!NzWindow::Initialize())
	{
		NazaraError("Failed to initialize window's system");
		Uninitialize();

		return false;
	}

	// On enregistre les loaders pour les extensions
	// Il s'agit ici d'une liste LIFO, le dernier loader enregistré possède la priorité

	/// Loaders génériques
	// Image
	NzLoaders_STB_Register(); // Loader générique (STB)

	/// Loaders spécialisés
	// Mesh
	NzLoaders_MD2_Register(); // Loader de fichiers .md2 (v8)
	NzLoaders_MD5Anim_Register(); // Loader de fichiers .md5anim (v10)
	NzLoaders_MD5Mesh_Register(); // Loader de fichiers .md5mesh (v10)

	// Image
	NzLoaders_PCX_Register(); // Loader de fichiers .pcx (1, 4, 8, 24 bits)

	NazaraNotice("Initialized: Utility module");

	return true;
}

bool NzUtility::IsInitialized()
{
	return s_moduleReferenceCounter != 0;
}

void NzUtility::Uninitialize()
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

	NzLoaders_MD2_Unregister();
	NzLoaders_MD5Mesh_Unregister();
	NzLoaders_PCX_Unregister();
	NzLoaders_STB_Unregister();

	NzWindow::Uninitialize();
	NzPixelFormat::Uninitialize();
	NzBuffer::Uninitialize();

	NazaraNotice("Uninitialized: Utility module");

	// Libération des dépendances
	NzCore::Uninitialize();
}

unsigned int NzUtility::s_moduleReferenceCounter = 0;

