// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERWINDOWPARAMETERS_HPP
#define NAZARA_RENDERWINDOWPARAMETERS_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Utility/PixelFormat.hpp>
#include <vector>

namespace Nz
{
	struct RenderWindowParameters
	{
		std::vector<PixelFormatType> depthFormats = {Nz::PixelFormatType_Depth32, Nz::PixelFormatType_Depth24}; //< By order of preference
		bool verticalSync = false;
	};
}

#endif // NAZARA_RENDERWINDOWPARAMETERS_HPP
