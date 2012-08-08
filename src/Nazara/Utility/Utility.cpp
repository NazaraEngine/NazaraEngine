// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Utility.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Utility/Buffer.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Loaders/MD2.hpp>
#include <Nazara/Utility/Loaders/PCX.hpp>
#include <Nazara/Utility/Loaders/STB.hpp>
#include <Nazara/Utility/PixelFormat.hpp>
#include <Nazara/Utility/Window.hpp>
#include <Nazara/Utility/Debug.hpp>

bool NzUtility::Initialize()
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
	if (!NzBuffer::Initialize())
	{
		NazaraError("Failed to initialize buffers");
		return false;
	}

	if (!NzPixelFormat::Initialize())
	{
		NazaraError("Failed to initialize pixel formats");
		return false;
	}

	if (!NzWindow::Initialize())
	{
		NazaraError("Failed to initialize window's system");
		NzPixelFormat::Uninitialize();

		return false;
	}

	/// Loaders spécialisés
	// Mesh
	NzLoaders_MD2_Register(); // Loader de fichiers .MD2 (v8)

	// Image
	NzLoaders_PCX_Register(); // Loader de fichiers .PCX (1, 4, 8, 24)

	/// Loaders génériques (En dernier pour donner la priorité aux loaders spécialisés)
	// Image
	NzLoaders_STB_Register(); // Loader générique (STB)

	NazaraNotice("Initialized: Utility module");

	return true;
}

bool NzUtility::IsInitialized()
{
	return s_moduleReferenceCouter != 0;
}

void NzUtility::Uninitialize()
{
	if (--s_moduleReferenceCouter != 0)
		return; // Encore utilisé

	// Libération du module
	NzLoaders_MD2_Unregister();
	NzLoaders_PCX_Unregister();
	NzLoaders_STB_Unregister();

	NzWindow::Uninitialize();
	NzPixelFormat::Uninitialize();
	NzBuffer::Uninitialize();

	NazaraNotice("Uninitialized: Utility module");

	// Libération des dépendances
	NzCore::Uninitialize();
}

unsigned int NzUtility::s_moduleReferenceCouter = 0;

