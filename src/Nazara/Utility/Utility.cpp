// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Utility.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Utility/Animation.hpp>
#include <Nazara/Utility/Buffer.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Font.hpp>
#include <Nazara/Utility/Image.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Utility/PixelFormat.hpp>
#include <Nazara/Utility/Skeleton.hpp>
#include <Nazara/Utility/VertexDeclaration.hpp>
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
#include <stdexcept>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup utility
	* \class Nz::Utility
	* \brief Utility class that represents the module initializer of Utility
	*/

	Utility::Utility() :
	Module("Utility", this)
	{
		if (!Animation::Initialize())
			throw std::runtime_error("failed to initialize animations");

		if (!Buffer::Initialize())
			throw std::runtime_error("failed to initialize buffers");

		if (!Font::Initialize())
			throw std::runtime_error("failed to initialize fonts");

		if (!Image::Initialize())
			throw std::runtime_error("failed to initialize images");

		if (!Mesh::Initialize())
			throw std::runtime_error("failed to initialize meshes");

		if (!PixelFormatInfo::Initialize())
			throw std::runtime_error("failed to initialize pixel formats");

		if (!Skeleton::Initialize())
			throw std::runtime_error("failed to initialize skeletons");

		if (!VertexDeclaration::Initialize())
			throw std::runtime_error("failed to initialize vertex declarations");

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
	}

	Utility::~Utility()
	{
		Loaders::UnregisterFreeType();
		Loaders::UnregisterMD2();
		Loaders::UnregisterMD5Anim();
		Loaders::UnregisterMD5Mesh();
		Loaders::UnregisterOBJLoader();
		Loaders::UnregisterOBJSaver();
		Loaders::UnregisterPCX();
		Loaders::UnregisterSTBLoader();
		Loaders::UnregisterSTBSaver();

		VertexDeclaration::Uninitialize();
		Skeleton::Uninitialize();
		PixelFormatInfo::Uninitialize();
		Mesh::Uninitialize();
		Image::Uninitialize();
		Font::Uninitialize();
		Buffer::Uninitialize();
		Animation::Uninitialize();
	}

	Utility* Utility::s_instance = nullptr;
}
