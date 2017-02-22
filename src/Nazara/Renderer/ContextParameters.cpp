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

	// Contexte de partage par défaut, initialisé par OpenGL::Initialize()
	const Context* ContextParameters::defaultShareContext = nullptr;

	// Si possible, garder la compatibilité avec les fonctionnalités dépréciées
	bool ContextParameters::defaultCompatibilityProfile = false;

	// Mode debug d'OpenGL par défaut
	#if NAZARA_RENDERER_OPENGL_DEBUG || defined(NAZARA_DEBUG)
	bool ContextParameters::defaultDebugMode = true;
	#else
	bool ContextParameters::defaultDebugMode = false;
	#endif

	// Active le double-buffering sur les contextes
	bool ContextParameters::defaultDoubleBuffered = false;

	// Active le partage des ressources entre contextes (Via le defaultShareContext)
	bool ContextParameters::defaultShared = true;
}
