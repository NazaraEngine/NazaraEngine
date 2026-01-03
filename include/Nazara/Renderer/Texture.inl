// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	inline TextureInfo Texture::ApplyView(TextureInfo textureInfo, const TextureViewInfo& viewInfo)
	{
		textureInfo.type        = viewInfo.viewType;
		textureInfo.pixelFormat = (viewInfo.reinterpretFormat != PixelFormat::Undefined) ? viewInfo.reinterpretFormat : textureInfo.pixelFormat;
		textureInfo.width       = ImageUtils::GetLevelSize(textureInfo.width,  viewInfo.baseMipLevel);
		textureInfo.height      = ImageUtils::GetLevelSize(textureInfo.height, viewInfo.baseMipLevel);
		textureInfo.depth       = ImageUtils::GetLevelSize(textureInfo.depth,  viewInfo.baseMipLevel);
		textureInfo.levelCount  = viewInfo.levelCount;
		textureInfo.layerCount  = viewInfo.layerCount;

		return textureInfo;
	}

	inline TextureInfo Texture::BuildTextureInfo(const Image& image)
	{
		TextureInfo texParams;
		texParams.pixelFormat = image.GetFormat();
		texParams.type = image.GetType();
		texParams.width = image.GetWidth();

		switch (image.GetType())
		{
			case ImageType::E1D:
				texParams.height = 1;
				break;

			case ImageType::E2D:
				texParams.height = image.GetHeight();
				break;

			case ImageType::E3D:
				texParams.height = image.GetHeight();
				texParams.depth = image.GetDepth();
				break;

			case ImageType::E1D_Array:
				texParams.height = 1;
				texParams.layerCount = image.GetHeight();
				break;

			case ImageType::E2D_Array:
				texParams.height = image.GetHeight();
				texParams.layerCount = image.GetDepth();
				break;

			case ImageType::Cubemap:
				texParams.height = image.GetHeight();
				texParams.layerCount = 6;
				break;
		}

		return texParams;
	}
}
