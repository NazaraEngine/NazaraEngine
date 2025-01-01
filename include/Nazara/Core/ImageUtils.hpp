// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_IMAGEUTILS_HPP
#define NAZARA_CORE_IMAGEUTILS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/Export.hpp>
#include <Nazara/Core/PixelFormat.hpp>
#include <Nazara/Math/Box.hpp>

namespace Nz::ImageUtils
{
	static constexpr UInt8 MaxLevels = 32;

	inline void ArrayToRegion(ImageType type, UInt32 baseLayer, UInt32 layerCount, Boxui32& region);
	inline void Copy(UInt8* destination, const UInt8* source, PixelFormat format, UInt32 width, UInt32 height, UInt32 depth = 1, UInt32 dstWidth = 0, UInt32 dstHeight = 0, UInt32 srcWidth = 0, UInt32 srcHeight = 0);
	template<typename F> auto ForEachLevel(ImageType type, UInt32 width, UInt32 height, UInt32 depth, F&& callback);
	template<typename F> auto ForEachLevel(std::size_t levelCount, ImageType type, UInt32 width, UInt32 height, UInt32 depth, F&& callback);
	inline UInt32 GetLevelSize(UInt32 size, UInt8 level);
	inline UInt8 GetMaxLevel(UInt32 width, UInt32 height, UInt32 depth = 1);
	inline UInt8 GetMaxLevel(ImageType type, UInt32 width, UInt32 height, UInt32 depth = 1);
	inline Boxui32 RegionToArray(ImageType type, Boxui32 region, UInt32& baseLayer, UInt32& layerCount);
}

#include <Nazara/Core/ImageUtils.inl>

#endif // NAZARA_CORE_IMAGEUTILS_HPP
