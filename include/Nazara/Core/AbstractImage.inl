// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	inline bool AbstractImage::Update(const void* pixels, unsigned int srcWidth, unsigned int srcHeight, UInt8 level)
	{
		return Update(pixels, Boxui(Vector3ui::Zero(), GetSize(level)), srcWidth, srcHeight, level);
	}

	inline bool AbstractImage::Update(const void* pixels, const Rectui& rect, unsigned int z, unsigned int srcWidth, unsigned int srcHeight, UInt8 level)
	{
		return Update(pixels, Boxui(rect.x, rect.y, z, rect.width, rect.height, 1), srcWidth, srcHeight, level);
	}
}

#include <Nazara/Core/DebugOff.hpp>
