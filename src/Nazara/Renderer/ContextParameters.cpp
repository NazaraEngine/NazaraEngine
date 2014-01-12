// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/ContextParameters.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Debug.hpp>

// Version majeure d'OpenGL, initialisé par NzOpenGL::Initialize()
nzUInt8 NzContextParameters::defaultMajorVersion;

 // Version majeure d'OpenGL, initialisé par NzOpenGL::Initialize()
nzUInt8 NzContextParameters::defaultMinorVersion;

// Contexte de partage par défaut, initialisé par NzOpenGL::Initialize()
const NzContext* NzContextParameters::defaultShareContext = nullptr;

// Si possible, garder la compatibilité avec les fonctionnalités dépréciées
bool NzContextParameters::defaultCompatibilityProfile = false;

// Mode debug d'OpenGL par défaut
#if NAZARA_RENDERER_OPENGL_DEBUG || defined(NAZARA_DEBUG)
bool NzContextParameters::defaultDebugMode = true;
#else
bool NzContextParameters::defaultDebugMode = false;
#endif

// Active le double-buffering sur les contextes
bool NzContextParameters::defaultDoubleBuffered = false;

// Active le partage des ressources entre contextes (Via le defaultShareContext)
bool NzContextParameters::defaultShared = true;
