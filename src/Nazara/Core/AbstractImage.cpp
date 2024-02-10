// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/AbstractImage.hpp>
#include <Nazara/Core/PixelFormat.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	AbstractImage::~AbstractImage() = default;

	UInt8 AbstractImage::GetBytesPerPixel() const
	{
		return PixelFormatInfo::GetBytesPerPixel(GetFormat());
	}

	bool AbstractImage::IsCompressed() const
	{
		return PixelFormatInfo::IsCompressed(GetFormat());
	}

	bool AbstractImage::IsCubemap() const
	{
		return GetType() == ImageType::Cubemap;
	}
}
