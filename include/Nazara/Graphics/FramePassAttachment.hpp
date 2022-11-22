// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_FRAMEPASSATTACHMENT_HPP
#define NAZARA_GRAPHICS_FRAMEPASSATTACHMENT_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Utility/PixelFormat.hpp>
#include <string>

namespace Nz
{
	enum class FramePassAttachmentSize
	{
		Fixed,
		SwapchainFactor
	};

	struct FramePassAttachment
	{
		std::string name;
		PixelFormat format;
		FramePassAttachmentSize size = FramePassAttachmentSize::SwapchainFactor;
		unsigned int width = 100'000;
		unsigned int height = 100'000;
	};
}

#include <Nazara/Graphics/FramePassAttachment.inl>

#endif // NAZARA_GRAPHICS_FRAMEPASSATTACHMENT_HPP
