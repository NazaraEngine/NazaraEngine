// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_FRAMEPASSATTACHMENT_HPP
#define NAZARA_GRAPHICS_FRAMEPASSATTACHMENT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/PixelFormat.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <string>

namespace Nz
{
	enum class FramePassAttachmentSize
	{
		Fixed,
		ViewerTargetFactor,
	};

	struct FramePassAttachment
	{
		std::string name;
		PixelFormat format;
		TextureUsageFlags additionalUsages;
		FramePassAttachmentSize size = FramePassAttachmentSize::ViewerTargetFactor;
		unsigned int width = 100'000;
		unsigned int height = 100'000;
		unsigned int viewerIndex = 0;
	};
}

#include <Nazara/Graphics/FramePassAttachment.inl>

#endif // NAZARA_GRAPHICS_FRAMEPASSATTACHMENT_HPP
