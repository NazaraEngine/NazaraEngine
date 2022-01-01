// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/AbstractImage.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	inline bool AbstractImage::Update(const void* pixels, unsigned int srcWidth, unsigned int srcHeight, UInt8 level)
	{
		return Update(pixels, GetSize(level), srcWidth, srcHeight, level);
	}

	inline bool AbstractImage::Update(const void* pixels, const Rectui& rect, unsigned int z, unsigned int srcWidth, unsigned int srcHeight, UInt8 level)
	{
		return Update(pixels, Boxui(rect.x, rect.y, z, rect.width, rect.height, 1), srcWidth, srcHeight, level);
	}
}

#include <Nazara/Utility/DebugOff.hpp>
