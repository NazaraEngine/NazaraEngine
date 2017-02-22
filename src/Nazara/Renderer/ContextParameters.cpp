// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/ContextParameters.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	// Major version of OpenGL, initialised by OpenGL::Initialize()
	UInt8 ContextParameters::defaultMajorVersion;

	 // Minor version of OpenGL, initialised by OpenGL::Initialize()
	UInt8 ContextParameters::defaultMinorVersion;

	// Context of default sharing, initialised by OpenGL::Initialize()
	const Context* ContextParameters::defaultShareContext = nullptr;

	// If it's possible, keep the compatibility with deprecated functionalities 
	bool ContextParameters::defaultCompatibilityProfile = false;

	// Default debug mode of OpenGL
	#if NAZARA_RENDERER_OPENGL_DEBUG || defined(NAZARA_DEBUG)
	bool ContextParameters::defaultDebugMode = true;
	#else
	bool ContextParameters::defaultDebugMode = false;
	#endif

	// Enables double-buffering on contexts Active le double-buffering sur les contextes
	bool ContextParameters::defaultDoubleBuffered = false;

	// Enables ressource sharing on contexts (via defaultShareContext)
	bool ContextParameters::defaultShared = true;
}
