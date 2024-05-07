// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_FRAMEGRAPHSTRUCTS_HPP
#define NAZARA_GRAPHICS_FRAMEGRAPHSTRUCTS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Graphics/FramePassAttachment.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <optional>
#include <string>

namespace Nz
{
	class Texture;

	struct FrameGraphTextureData
	{
		struct ViewData
		{
			std::size_t parentTextureId;
			std::size_t arrayLayer;
		};

		std::optional<ViewData> viewData;
		std::shared_ptr<Texture> externalTexture;
		std::string name;
		ImageType type;
		PixelFormat format;
		FramePassAttachmentSize size;
		TextureUsageFlags usage;
		UInt32 width;
		UInt32 height;
		UInt32 layerCount;
		bool canReuse;
		unsigned int viewerIndex;
	};
}

#endif // NAZARA_GRAPHICS_FRAMEGRAPHSTRUCTS_HPP
