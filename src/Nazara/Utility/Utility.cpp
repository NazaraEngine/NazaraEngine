// Copyright (C) 2015 Jérôme Leclercq
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
#include <Nazara/Utility/Animation.hpp>
#include <Nazara/Utility/Buffer.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Font.hpp>
#include <Nazara/Utility/Image.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Utility/PixelFormat.hpp>
#include <Nazara/Utility/Skeleton.hpp>
#include <Nazara/Utility/VertexDeclaration.hpp>
#include <Nazara/Utility/Window.hpp>
#include <Nazara/Utility/Formats/DDSLoader.hpp>
#include <Nazara/Utility/Formats/FreeTypeLoader.hpp>
#include <Nazara/Utility/Formats/MD2Loader.hpp>
#include <Nazara/Utility/Formats/MD5AnimLoader.hpp>
#include <Nazara/Utility/Formats/MD5MeshLoader.hpp>
#include <Nazara/Utility/Formats/OBJLoader.hpp>
#include <Nazara/Utility/Formats/OBJSaver.hpp>
#include <Nazara/Utility/Formats/PCXLoader.hpp>
#include <Nazara/Utility/Formats/STBLoader.hpp>
#include <Nazara/Utility/Formats/STBSaver.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	bool Utility::Initialize()
	{
		if (s_moduleReferenceCounter > 0)
		{
			s_moduleReferenceCounter++;
			return true; // Déjà initialisé
		}

		// Initialisation des dépendances
		if (!Core::Initialize())
		{
			NazaraError("Failed to initialize core module");
			return false;
		}

		s_moduleReferenceCounter++;

		// Initialisation du module
		CallOnExit onExit(Utility::Uninitialize);

		if (!Animation::Initialize())
		{
			NazaraError("Failed to initialize animations");
			return false;
		}

		if (!Buffer::Initialize())
		{
			NazaraError("Failed to initialize buffers");
			return false;
		}

		if (!Font::Initialize())
		{
			NazaraError("Failed to initialize fonts");
			return false;
		}

		if (!Image::Initialize())
		{
			NazaraError("Failed to initialize images");
			return false;
		}

		if (!Mesh::Initialize())
		{
			NazaraError("Failed to initialize meshes");
			return false;
		}

		if (!PixelFormat::Initialize())
		{
			NazaraError("Failed to initialize pixel formats");
			return false;
		}

		if (!Skeleton::Initialize())
		{
			NazaraError("Failed to initialize skeletons");
			return false;
		}

		if (!VertexDeclaration::Initialize())
		{
			NazaraError("Failed to initialize vertex declarations");
			return false;
		}

		if (!Window::Initialize())
		{
			NazaraError("Failed to initialize window's system");
			return false;
		}

		// On enregistre les loaders pour les extensions
		// Il s'agit ici d'une liste LIFO, le dernier loader enregistré possède la priorité

		/// Loaders génériques
		// Font
		Loaders::RegisterFreeType();

		// Image
		Loaders::RegisterDDSLoader(); // DDS Loader (DirectX format)
		Loaders::RegisterSTBLoader(); // Generic loader (STB)
		Loaders::RegisterSTBSaver();  // Generic saver (STB)

		/// Loaders spécialisés
		// Animation
		Loaders::RegisterMD5Anim(); // Loader de fichiers .md5anim (v10)

		// Mesh (text)
		Loaders::RegisterOBJLoader();
		Loaders::RegisterOBJSaver();

		// Mesh
		Loaders::RegisterMD2(); // Loader de fichiers .md2 (v8)
		Loaders::RegisterMD5Mesh(); // Loader de fichiers .md5mesh (v10)
		Loaders::RegisterOBJLoader(); // Loader de fichiers .md5mesh (v10)

		// Image
		Loaders::RegisterPCX(); // Loader de fichiers .pcx (1, 4, 8, 24 bits)

		onExit.Reset();

		NazaraNotice("Initialized: Utility module");
		return true;
	}

	bool Utility::IsInitialized()
	{
		return s_moduleReferenceCounter != 0;
	}

	void Utility::Uninitialize()
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

		Loaders::UnregisterFreeType();
		Loaders::UnregisterMD2();
		Loaders::UnregisterMD5Anim();
		Loaders::UnregisterMD5Mesh();
		Loaders::UnregisterOBJLoader();
		Loaders::UnregisterOBJSaver();
		Loaders::UnregisterPCX();
		Loaders::UnregisterSTBLoader();
		Loaders::UnregisterSTBSaver();

		Window::Uninitialize();
		VertexDeclaration::Uninitialize();
		Skeleton::Uninitialize();
		PixelFormat::Uninitialize();
		Mesh::Uninitialize();
		Image::Uninitialize();
		Font::Uninitialize();
		Buffer::Uninitialize();
		Animation::Uninitialize();

		NazaraNotice("Uninitialized: Utility module");

		// Libération des dépendances
		Core::Uninitialize();
	}

	unsigned int Utility::ComponentCount[ComponentType_Max+1] =
	{
		4, // ComponentType_Color
		1, // ComponentType_Double1
		2, // ComponentType_Double2
		3, // ComponentType_Double3
		4, // ComponentType_Double4
		1, // ComponentType_Float1
		2, // ComponentType_Float2
		3, // ComponentType_Float3
		4, // ComponentType_Float4
		1, // ComponentType_Int1
		2, // ComponentType_Int2
		3, // ComponentType_Int3
		4, // ComponentType_Int4
		4  // ComponentType_Quaternion
	};

	static_assert(ComponentType_Max+1 == 14, "Component count array is incomplete");

	std::size_t Utility::ComponentStride[ComponentType_Max+1] =
	{
		4*sizeof(UInt8),  // ComponentType_Color
		1*sizeof(double),   // ComponentType_Double1
		2*sizeof(double),   // ComponentType_Double2
		3*sizeof(double),   // ComponentType_Double3
		4*sizeof(double),   // ComponentType_Double4
		1*sizeof(float),    // ComponentType_Float1
		2*sizeof(float),    // ComponentType_Float2
		3*sizeof(float),    // ComponentType_Float3
		4*sizeof(float),    // ComponentType_Float4
		1*sizeof(UInt32), // ComponentType_Int1
		2*sizeof(UInt32), // ComponentType_Int2
		3*sizeof(UInt32), // ComponentType_Int3
		4*sizeof(UInt32), // ComponentType_Int4
		4*sizeof(float)     // ComponentType_Quaternion
	};

	static_assert(ComponentType_Max+1 == 14, "Component stride array is incomplete");

	unsigned int Utility::s_moduleReferenceCounter = 0;
}
