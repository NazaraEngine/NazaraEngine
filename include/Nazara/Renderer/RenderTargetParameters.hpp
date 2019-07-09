// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERTARGETPARAMETERS_HPP
#define NAZARA_RENDERTARGETPARAMETERS_HPP

#include <Nazara/Prerequisites.hpp>

namespace Nz
{
	struct RenderTargetParameters
	{
		RenderTargetParameters(UInt8 antialiasing = 0, UInt8 depth = 24, UInt8 stencil = 0) :
		antialiasingLevel(antialiasing),
		depthBits(depth),
		stencilBits(stencil)
		{
		}

		UInt8 antialiasingLevel;
		UInt8 depthBits;
		UInt8 stencilBits;
	};
}

#endif // NAZARA_RENDERTARGETPARAMETERS_HPP
