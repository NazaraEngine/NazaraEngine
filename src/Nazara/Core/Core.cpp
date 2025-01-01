// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Animation.hpp>
#include <Nazara/Core/Buffer.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Export.hpp>
#include <Nazara/Core/HardwareInfo.hpp>
#include <Nazara/Core/Image.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Core/Mesh.hpp>
#include <Nazara/Core/PixelFormat.hpp>
#include <Nazara/Core/PluginLoader.hpp>
#include <Nazara/Core/Skeleton.hpp>
#include <Nazara/Core/TaskScheduler.hpp>
#include <Nazara/Core/VertexDeclaration.hpp>
#include <Nazara/Core/Formats/DDSLoader.hpp>
#include <Nazara/Core/Formats/GIFLoader.hpp>
#include <Nazara/Core/Formats/MD2Loader.hpp>
#include <Nazara/Core/Formats/MD5AnimLoader.hpp>
#include <Nazara/Core/Formats/MD5MeshLoader.hpp>
#include <Nazara/Core/Formats/OBJLoader.hpp>
#include <Nazara/Core/Formats/OBJSaver.hpp>
#include <Nazara/Core/Formats/PCXLoader.hpp>
#include <Nazara/Core/Formats/STBLoader.hpp>
#include <Nazara/Core/Formats/STBSaver.hpp>

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::Core
	* \brief Core class that represents the Core module
	*/

	Core::Core(Config /*config*/) :
	ModuleBase("Core", this, ModuleBase::NoLog{})
	{
		Log::Initialize();

		LogInit();

		m_hardwareInfo.emplace();

		if (!PixelFormatInfo::Initialize())
			throw std::runtime_error("failed to initialize pixel formats");

		if (!VertexDeclaration::Initialize())
			throw std::runtime_error("failed to initialize vertex declarations");

		// Image
		m_imageLoader.RegisterLoader(Loaders::GetImageLoader_STB()); // Generic loader (STB)
		m_imageSaver.RegisterSaver(Loaders::GetImageSaver_STB()); // Generic saver (STB)

		// ImageStream
		m_imageStreamLoader.RegisterLoader(Loaders::GetImageStreamLoader_GIF()); // GIF loader

		/// Specialized loaders
		// Animation
		m_animationLoader.RegisterLoader(Loaders::GetAnimationLoader_MD5Anim()); // Loader de fichiers .md5anim (v10)

		// Mesh
		m_meshLoader.RegisterLoader(Loaders::GetMeshLoader_OBJ());
		m_meshLoader.RegisterLoader(Loaders::GetMeshLoader_MD2()); // .md2 (v8)
		m_meshLoader.RegisterLoader(Loaders::GetMeshLoader_MD5Mesh()); // .md5mesh (v10)
		m_meshLoader.RegisterLoader(Loaders::GetMeshLoader_OBJ()); // .obj
		m_meshSaver.RegisterSaver(Loaders::GetMeshSaver_OBJ());

		// Image
		m_imageLoader.RegisterLoader(Loaders::GetImageLoader_DDS()); // DDS Loader (DirectX format)
		m_imageLoader.RegisterLoader(Loaders::GetImageLoader_PCX()); // .pcx loader (1, 4, 8, 24 bits)
	}

	Core::~Core()
	{
		m_hardwareInfo.reset();

		LogUninit();
		Log::Uninitialize();
	}

	AnimationLoader& Core::GetAnimationLoader()
	{
		return m_animationLoader;
	}

	const AnimationLoader& Core::GetAnimationLoader() const
	{
		return m_animationLoader;
	}

	ImageLoader& Core::GetImageLoader()
	{
		return m_imageLoader;
	}

	const ImageLoader& Core::GetImageLoader() const
	{
		return m_imageLoader;
	}

	ImageStreamLoader& Core::GetImageStreamLoader()
	{
		return m_imageStreamLoader;
	}

	const ImageStreamLoader& Core::GetImageStreamLoader() const
	{
		return m_imageStreamLoader;
	}

	ImageSaver& Core::GetImageSaver()
	{
		return m_imageSaver;
	}

	const ImageSaver& Core::GetImageSaver() const
	{
		return m_imageSaver;
	}

	MeshLoader& Core::GetMeshLoader()
	{
		return m_meshLoader;
	}

	const MeshLoader& Core::GetMeshLoader() const
	{
		return m_meshLoader;
	}

	MeshSaver& Core::GetMeshSaver()
	{
		return m_meshSaver;
	}

	const MeshSaver& Core::GetMeshSaver() const
	{
		return m_meshSaver;
	}

	Core* Core::s_instance = nullptr;
}
