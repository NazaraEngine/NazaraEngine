// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Utility.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/HardwareInfo.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Core/TaskScheduler.hpp>
#include <Nazara/Core/Thread.hpp>
#include <Nazara/Utility/Buffer.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Loaders/MD2.hpp>
#include <Nazara/Utility/Loaders/MD5Anim.hpp>
#include <Nazara/Utility/Loaders/MD5Mesh.hpp>
#include <Nazara/Utility/Loaders/PCX.hpp>
#include <Nazara/Utility/Loaders/STB.hpp>
#include <Nazara/Utility/PixelFormat.hpp>
#include <Nazara/Utility/VertexDeclaration.hpp>
#include <Nazara/Utility/Window.hpp>
#include <Nazara/Utility/Debug.hpp>

bool NzUtility::Initialize()
{
	if (s_moduleReferenceCounter > 0)
	{
		s_moduleReferenceCounter++;
		return true; // Déjà initialisé
	}

	// Initialisation des dépendances
	if (!NzCore::Initialize())
	{
		NazaraError("Failed to initialize core module");
		return false;
	}

	s_moduleReferenceCounter++;

	// Initialisation du module
	NzCallOnExit onExit(NzUtility::Uninitialize);

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

	if (!NzVertexDeclaration::Initialize())
	{
		NazaraError("Failed to initialize vertex declarations");
		return false;
	}

	if (!NzWindow::Initialize())
	{
		NazaraError("Failed to initialize window's system");
		return false;
	}

	// On enregistre les loaders pour les extensions
	// Il s'agit ici d'une liste LIFO, le dernier loader enregistré possède la priorité

	/// Loaders génériques
	// Image
	NzLoaders_STB_Register(); // Loader générique (STB)

	/// Loaders spécialisés
	// Animation
	NzLoaders_MD5Anim_Register(); // Loader de fichiers .md5anim (v10)

	// Mesh
	NzLoaders_MD2_Register(); // Loader de fichiers .md2 (v8)
	NzLoaders_MD5Mesh_Register(); // Loader de fichiers .md5mesh (v10)

	// Image
	NzLoaders_PCX_Register(); // Loader de fichiers .pcx (1, 4, 8, 24 bits)

	onExit.Reset();

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
	NzLoaders_MD5Anim_Unregister();
	NzLoaders_MD5Mesh_Unregister();
	NzLoaders_PCX_Unregister();
	NzLoaders_STB_Unregister();

	NzWindow::Uninitialize();
	NzVertexDeclaration::Uninitialize();
	NzPixelFormat::Uninitialize();
	NzBuffer::Uninitialize();

	NazaraNotice("Uninitialized: Utility module");

	// Libération des dépendances
	NzCore::Uninitialize();
}

unsigned int NzUtility::ComponentCount[nzComponentType_Max+1] =
{
	4, // nzComponentType_Color
	1, // nzComponentType_Double1
	2, // nzComponentType_Double2
	3, // nzComponentType_Double3
	4, // nzComponentType_Double4
	1, // nzComponentType_Float1
	2, // nzComponentType_Float2
	3, // nzComponentType_Float3
	4, // nzComponentType_Float4
	1, // nzComponentType_Int1
	2, // nzComponentType_Int2
	3, // nzComponentType_Int3
	4, // nzComponentType_Int4
	4  // nzComponentType_Quaternion
};

static_assert(nzComponentType_Max+1 == 14, "Component count array is incomplete");

unsigned int NzUtility::ComponentStride[nzComponentType_Max+1] =
{
	4*sizeof(nzUInt8),  // nzComponentType_Color
	1*sizeof(double),   // nzComponentType_Double1
	2*sizeof(double),   // nzComponentType_Double2
	3*sizeof(double),   // nzComponentType_Double3
	4*sizeof(double),   // nzComponentType_Double4
	1*sizeof(float),    // nzComponentType_Float1
	2*sizeof(float),    // nzComponentType_Float2
	3*sizeof(float),    // nzComponentType_Float3
	4*sizeof(float),    // nzComponentType_Float4
	1*sizeof(nzUInt32), // nzComponentType_Int1
	2*sizeof(nzUInt32), // nzComponentType_Int2
	3*sizeof(nzUInt32), // nzComponentType_Int3
	4*sizeof(nzUInt32), // nzComponentType_Int4
	4*sizeof(float)     // nzComponentType_Quaternion
};

static_assert(nzComponentType_Max+1 == 14, "Component stride array is incomplete");

unsigned int NzUtility::s_moduleReferenceCounter = 0;
