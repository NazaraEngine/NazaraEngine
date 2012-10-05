// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERTARGETPARAMETERS_HPP
#define NAZARA_RENDERTARGETPARAMETERS_HPP

#include <Nazara/Prerequesites.hpp>

struct NzRenderTargetParameters
{
	NzRenderTargetParameters(nzUInt8 antialiasing = 0, nzUInt8 depth = 24, nzUInt8 stencil = 0) :
	antialiasingLevel(antialiasing),
	depthBits(depth),
	stencilBits(stencil)
	{
	}

	nzUInt8 antialiasingLevel;
	nzUInt8 depthBits;
	nzUInt8 stencilBits;
};

#endif // NAZARA_RENDERTARGETPARAMETERS_HPP
