// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERWINDOWPARAMETERS_HPP
#define NAZARA_RENDERWINDOWPARAMETERS_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Utility/PixelFormat.hpp>
#include <vector>

namespace Nz
{
	struct RenderWindowParameters
	{
		std::vector<PixelFormat> depthFormats = {Nz::PixelFormat::Depth24Stencil8, Nz::PixelFormat::Depth32FStencil8, Nz::PixelFormat::Depth16Stencil8, Nz::PixelFormat::Depth32F, Nz::PixelFormat::Depth24}; //< By order of preference
		bool verticalSync = false;
	};
}

#endif // NAZARA_RENDERWINDOWPARAMETERS_HPP
