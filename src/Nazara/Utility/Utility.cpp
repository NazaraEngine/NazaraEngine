// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Utility.hpp>
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
#include <Nazara/Utility/Formats/GIFLoader.hpp>
#include <Nazara/Utility/Formats/MD2Loader.hpp>
#include <Nazara/Utility/Formats/MD5AnimLoader.hpp>
#include <Nazara/Utility/Formats/MD5MeshLoader.hpp>
#include <Nazara/Utility/Formats/OBJLoader.hpp>
#include <Nazara/Utility/Formats/OBJSaver.hpp>
#include <Nazara/Utility/Formats/PCXLoader.hpp>
#include <Nazara/Utility/Formats/STBLoader.hpp>
#include <Nazara/Utility/Formats/STBSaver.hpp>
#include <NazaraUtils/CallOnExit.hpp>
#include <stdexcept>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup utility
	* \class Nz::Utility
	* \brief Utility class that represents the module initializer of Utility
	*/

	Utility::Utility(Config /*config*/) :
	ModuleBase("Utility", this)
	{
		if (!Font::Initialize())
			throw std::runtime_error("failed to initialize fonts");

		if (!PixelFormatInfo::Initialize())
			throw std::runtime_error("failed to initialize pixel formats");

		if (!VertexDeclaration::Initialize())
			throw std::runtime_error("failed to initialize vertex declarations");

		// On enregistre les loaders pour les extensions
		// Il s'agit ici d'une liste LIFO, le dernier loader enregistré possède la priorité

		/// Loaders génériques
		// Font
		if (Loaders::InitializeFreeType())
			m_fontLoader.RegisterLoader(Loaders::GetFontLoader_FreeType());

		// Image
		m_imageLoader.RegisterLoader(Loaders::GetImageLoader_STB()); // Generic loader (STB)
		m_imageSaver.RegisterSaver(Loaders::GetImageSaver_STB()); // Generic saver (STB)

		// ImageStream
		m_imageStreamLoader.RegisterLoader(Loaders::GetImageStreamLoader_GIF()); // GIF loader

		/// Loaders spécialisés
		// Animation
		m_animationLoader.RegisterLoader(Loaders::GetAnimationLoader_MD5Anim()); // Loader de fichiers .md5anim (v10)

		// Mesh (text)
		m_meshLoader.RegisterLoader(Loaders::GetMeshLoader_OBJ());
		m_meshSaver.RegisterSaver(Loaders::GetMeshSaver_OBJ());

		// Mesh
		m_meshLoader.RegisterLoader(Loaders::GetMeshLoader_MD2()); // .md2 (v8)
		m_meshLoader.RegisterLoader(Loaders::GetMeshLoader_MD5Mesh()); // .md5mesh (v10)
		m_meshLoader.RegisterLoader(Loaders::GetMeshLoader_OBJ()); // .obj

		// Image
		m_imageLoader.RegisterLoader(Loaders::GetImageLoader_DDS()); // DDS Loader (DirectX format)
		m_imageLoader.RegisterLoader(Loaders::GetImageLoader_PCX()); // .pcx loader (1, 4, 8, 24 bits)
	}

	Utility::~Utility()
	{
		Loaders::UninitializeFreeType();

		VertexDeclaration::Uninitialize();
		PixelFormatInfo::Uninitialize();
		Font::Uninitialize();
	}

	AnimationLoader& Utility::GetAnimationLoader()
	{
		return m_animationLoader;
	}

	const AnimationLoader& Utility::GetAnimationLoader() const
	{
		return m_animationLoader;
	}

	FontLoader& Utility::GetFontLoader()
	{
		return m_fontLoader;
	}

	const FontLoader& Utility::GetFontLoader() const
	{
		return m_fontLoader;
	}

	ImageLoader& Utility::GetImageLoader()
	{
		return m_imageLoader;
	}

	const ImageLoader& Utility::GetImageLoader() const
	{
		return m_imageLoader;
	}

	ImageStreamLoader& Utility::GetImageStreamLoader()
	{
		return m_imageStreamLoader;
	}

	const ImageStreamLoader& Utility::GetImageStreamLoader() const
	{
		return m_imageStreamLoader;
	}

	ImageSaver& Utility::GetImageSaver()
	{
		return m_imageSaver;
	}

	const ImageSaver& Utility::GetImageSaver() const
	{
		return m_imageSaver;
	}

	MeshLoader& Utility::GetMeshLoader()
	{
		return m_meshLoader;
	}

	const MeshLoader& Utility::GetMeshLoader() const
	{
		return m_meshLoader;
	}

	MeshSaver& Utility::GetMeshSaver()
	{
		return m_meshSaver;
	}

	const MeshSaver& Utility::GetMeshSaver() const
	{
		return m_meshSaver;
	}

	Utility* Utility::s_instance = nullptr;
}
