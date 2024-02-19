// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_RENDERER_SWAPCHAINPARAMETERS_HPP
#define NAZARA_RENDERER_SWAPCHAINPARAMETERS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/PixelFormat.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <vector>

namespace Nz
{
	struct SwapchainParameters
	{
		std::vector<PixelFormat> depthFormats = { Nz::PixelFormat::Depth24Stencil8, Nz::PixelFormat::Depth32FStencil8, Nz::PixelFormat::Depth16Stencil8, Nz::PixelFormat::Depth32F, Nz::PixelFormat::Depth24 }; //< By order of preference
		std::vector<PresentMode> presentMode = { PresentMode::Mailbox, PresentMode::Immediate, PresentMode::RelaxedVerticalSync, PresentMode::VerticalSync }; //< By order of preference
	};
}

#endif // NAZARA_RENDERER_SWAPCHAINPARAMETERS_HPP
