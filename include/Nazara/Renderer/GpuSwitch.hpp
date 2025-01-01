// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <NazaraUtils/Prerequisites.hpp>

#pragma once

#ifndef NAZARA_RENDERER_GPUSWITCH_HPP
#define NAZARA_RENDERER_GPUSWITCH_HPP

#ifdef NAZARA_PLATFORM_WINDOWS

#define NAZARA_REQUEST_DEDICATED_GPU() \
extern "C" \
{ \
	NAZARA_EXPORT unsigned long NvOptimusEnablement = 1; \
	NAZARA_EXPORT unsigned long AmdPowerXpressRequestHighPerformance = 1; \
}

#else

#define NAZARA_REQUEST_DEDICATED_GPU()

#endif

#endif // NAZARA_RENDERER_GPUSWITCH_HPP
