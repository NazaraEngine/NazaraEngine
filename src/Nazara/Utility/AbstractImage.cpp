// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/AbstractImage.hpp>
#include <Nazara/Utility/PixelFormat.hpp>
#include <Nazara/Utility/Debug.hpp>

NzAbstractImage::~NzAbstractImage() = default;

nzUInt8 NzAbstractImage::GetBytesPerPixel() const
{
	return NzPixelFormat::GetBytesPerPixel(GetFormat());
}

bool NzAbstractImage::IsCompressed() const
{
	return NzPixelFormat::IsCompressed(GetFormat());
}

bool NzAbstractImage::IsCubemap() const
{
	return GetType() == nzImageType_Cubemap;
}
