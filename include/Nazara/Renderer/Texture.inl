// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	inline TextureInfo Texture::ApplyView(TextureInfo textureInfo, const TextureViewInfo& viewInfo)
	{
		textureInfo.type        = viewInfo.viewType;
		textureInfo.pixelFormat = (viewInfo.reinterpretFormat != PixelFormat::Undefined) ? viewInfo.reinterpretFormat : textureInfo.pixelFormat;
		textureInfo.width       = GetLevelSize(textureInfo.width,  viewInfo.baseMipLevel);
		textureInfo.height      = GetLevelSize(textureInfo.height, viewInfo.baseMipLevel);
		textureInfo.depth       = GetLevelSize(textureInfo.depth,  viewInfo.baseMipLevel);
		textureInfo.levelCount  = viewInfo.levelCount;
		textureInfo.layerCount  = viewInfo.layerCount;

		return textureInfo;
	}

	inline unsigned int Texture::GetLevelSize(unsigned int size, unsigned int level)
	{
		if (size == 0) // Possible dans le cas d'une image invalide
			return 0;

		return std::max(size >> level, 1U);
	}
}

#include <Nazara/Renderer/DebugOff.hpp>
