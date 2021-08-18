// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/AbstractImage.hpp>
#include <Nazara/Utility/PixelFormat.hpp>
#include <Nazara/Utility/Debug.hpp>

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
