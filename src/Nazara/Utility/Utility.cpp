// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Utility.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Utility/Buffer.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Loaders/MD2.hpp>
#include <Nazara/Utility/Loaders/PCX.hpp>
#include <Nazara/Utility/Loaders/STB.hpp>
#include <Nazara/Utility/PixelFormat.hpp>
#include <Nazara/Utility/Window.hpp>
#include <Nazara/Utility/Debug.hpp>

NzUtility::NzUtility()
{
}

NzUtility::~NzUtility()
{
	if (s_initialized)
		Uninitialize();
}

bool NzUtility::Initialize()
{
	#if NAZARA_UTILITY_SAFE
	if (s_initialized)
	{
		NazaraError("Renderer already initialized");
		return true;
	}
	#endif

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

	s_initialized = true;

	return true;
}

void NzUtility::Uninitialize()
{
	#if NAZARA_UTILITY_SAFE
	if (!s_initialized)
	{
		NazaraError("Utility not initialized");
		return;
	}
	#endif

	NzLoaders_MD2_Unregister();
	NzLoaders_PCX_Unregister();
	NzLoaders_STB_Unregister();

	NzWindow::Uninitialize();
	NzPixelFormat::Uninitialize();
	NzBuffer::Uninitialize();

	s_initialized = false;
}

bool NzUtility::IsInitialized()
{
	return s_initialized;
}

bool NzUtility::s_initialized = false;
