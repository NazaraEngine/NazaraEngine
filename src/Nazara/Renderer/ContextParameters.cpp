// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/ContextParameters.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Debug.hpp>

nzUInt8 NzContextParameters::defaultMajorVersion; // Initialisé par NzOpenGL
nzUInt8 NzContextParameters::defaultMinorVersion; // Initialisé par NzOpenGL
const NzContext* NzContextParameters::defaultShareContext = nullptr;
NzWindowHandle NzContextParameters::defaultWindow = 0;
bool NzContextParameters::defaultCompatibilityProfile = false;
#if NAZARA_RENDERER_OPENGL_DEBUG || defined(NAZARA_DEBUG)
bool NzContextParameters::defaultDebugMode = true;
#else
bool NzContextParameters::defaultDebugMode = false;
#endif
bool NzContextParameters::defaultDoubleBuffered = false;
bool NzContextParameters::defaultShared = true;
