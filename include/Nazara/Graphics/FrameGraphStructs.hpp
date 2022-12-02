// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_FRAMEGRAPHSTRUCTS_HPP
#define NAZARA_GRAPHICS_FRAMEGRAPHSTRUCTS_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/FramePassAttachment.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <optional>
#include <string>

namespace Nz
{
	struct FrameGraphTextureData
	{
		struct ViewData
		{
			std::size_t parentTextureId;
			std::size_t arrayLayer;
		};

		std::optional<ViewData> viewData;
		std::string name;
		ImageType type;
		PixelFormat format;
		FramePassAttachmentSize size;
		TextureUsageFlags usage;
		unsigned int width;
		unsigned int height;
	};
}

#endif // NAZARA_GRAPHICS_FRAMEGRAPHSTRUCTS_HPP
