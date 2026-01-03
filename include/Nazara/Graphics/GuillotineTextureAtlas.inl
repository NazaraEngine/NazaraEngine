// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	inline GuillotineTextureAtlas::GuillotineTextureAtlas(RenderDevice& renderDevice, PixelFormat pixelFormat, UInt32 initialLayerSize) :
	GuillotineImageAtlas(pixelFormat, initialLayerSize),
	m_renderDevice(renderDevice),
	m_texturePixelFormat(pixelFormat)
	{
	}

	inline GuillotineTextureAtlas::GuillotineTextureAtlas(RenderDevice& renderDevice, PixelFormat texturePixelFormat, PixelFormat imagePixelFormat, UInt32 initialLayerSize) :
	GuillotineImageAtlas(imagePixelFormat, initialLayerSize),
	m_renderDevice(renderDevice),
	m_texturePixelFormat(texturePixelFormat)
	{
		NazaraCheck(PixelFormatInfo::GetBytesPerPixel(imagePixelFormat) == PixelFormatInfo::GetBytesPerPixel(texturePixelFormat), "image pixel format ({0}) and texture pixel format ({1}) must have the same BPP ({2} != {3})", PixelFormatInfo::GetName(imagePixelFormat), PixelFormatInfo::GetName(texturePixelFormat), PixelFormatInfo::GetBytesPerPixel(imagePixelFormat), PixelFormatInfo::GetBytesPerPixel(texturePixelFormat));
	}
}
