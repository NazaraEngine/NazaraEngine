// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Error.hpp>
#include <NazaraUtils/Algorithm.hpp>
#include <NazaraUtils/TypeTraits.hpp>

namespace Nz::ImageUtils
{
	inline void ArrayToRegion(ImageType type, UInt32 baseLayer, UInt32 layerCount, Boxui32& region)
	{
		switch (type)
		{
			case ImageType::E1D_Array:
				region.y = baseLayer;
				region.height = layerCount;
				break;

			case ImageType::Cubemap:
			case ImageType::E2D_Array:
				region.z = baseLayer;
				region.depth = layerCount;
				break;

			case ImageType::E1D:
				NazaraAssert(baseLayer == 0, "out of bounds");
				NazaraAssert(layerCount <= 1, "out of bounds");
				[[fallthrough]];
			case ImageType::E2D:
				NazaraAssert(baseLayer == 0, "out of bounds");
				NazaraAssert(layerCount <= 1, "out of bounds");
				[[fallthrough]];
			case ImageType::E3D:
				region.z = 0;
				region.depth = 1;
				break;
		}
	}

	inline void Copy(UInt8* destination, const UInt8* source, PixelFormat format, UInt32 width, UInt32 height, UInt32 depth, UInt32 dstWidth, UInt32 dstHeight, UInt32 srcWidth, UInt32 srcHeight)
	{
		NazaraAssert(width > 0, "width must be greater than zero");
		NazaraAssert(height > 0, "height must be greater than zero");
		NazaraAssert(depth > 0, "depth must be greater than zero");

		if (dstWidth == 0)
			dstWidth = width;

		if (dstHeight == 0)
			dstHeight = height;

		if (srcWidth == 0)
			srcWidth = width;

		if (srcHeight == 0)
			srcHeight = height;

		if ((height == 1 || (dstWidth == width && srcWidth == width)) && (depth == 1 || (dstHeight == height && srcHeight == height)))
			std::memcpy(destination, source, PixelFormatInfo::ComputeSize(format, width, height, depth));
		else
		{
			UInt8 bpp = PixelFormatInfo::GetBytesPerPixel(format);
			std::size_t lineStride = static_cast<std::size_t>(width) * bpp;
			std::size_t dstLineStride = static_cast<std::size_t>(dstWidth) * bpp;
			std::size_t dstFaceStride = static_cast<std::size_t>(dstLineStride) * dstHeight;
			std::size_t srcLineStride = static_cast<std::size_t>(srcWidth) * bpp;
			std::size_t srcFaceStride = static_cast<std::size_t>(srcLineStride) * srcHeight;

			for (UInt32 i = 0; i < depth; ++i)
			{
				UInt8* dstFacePtr = destination;
				const UInt8* srcFacePtr = source;
				for (UInt32 y = 0; y < height; ++y)
				{
					std::memcpy(dstFacePtr, srcFacePtr, lineStride);

					dstFacePtr += dstLineStride;
					srcFacePtr += srcLineStride;
				}

				destination += dstFaceStride;
				source += srcFaceStride;
			}
		}
	}

	template<typename F>
	auto ForEachLevel(ImageType type, UInt32 width, UInt32 height, UInt32 depth, F&& callback)
	{
		UInt8 levelCount = GetMaxLevel(type, width, height, depth);
		return ForEachLevel(levelCount, type, width, height, depth, std::forward<F>(callback));
	}

	template<typename F>
	auto ForEachLevel(std::size_t levelCount, ImageType type, UInt32 width, UInt32 height, UInt32 depth, F&& callback)
	{
		using Ret = std::invoke_result_t<F, UInt8, UInt32, UInt32, UInt32>;

		for (std::size_t i = 0; i < levelCount; ++i)
		{
			if constexpr (std::is_same_v<Ret, bool>)
			{
				if (!callback(i, width, height, depth))
					return false;
			}
			else if constexpr (std::is_void_v<Ret>)
				callback(i, width, height, depth);
			else
				static_assert(AlwaysFalse<Ret>(), "callback must either return a bool or nothing");

			switch (type)
			{
				case ImageType::E3D:
					depth = std::max<UInt8>(depth >> 1u, 1u);
					[[fallthrough]];
				case ImageType::E2D:
				case ImageType::E2D_Array:
				case ImageType::Cubemap:
					height = std::max<UInt8>(height >> 1u, 1u);
					[[fallthrough]];
				case ImageType::E1D:
				case ImageType::E1D_Array:
					width = std::max<UInt8>(width >> 1u, 1u);
					break;
			}
		}

		if constexpr (std::is_same_v<Ret, bool>)
			return true;
	}

	inline UInt32 GetLevelSize(UInt32 size, UInt8 level)
	{
		return (size != 0) ? std::max<UInt32>(size >> level, 1u) : 0;
	}

	inline UInt8 GetMaxLevel(UInt32 width, UInt32 height, UInt32 depth)
	{
		// Maximum level is the one required for the greater size
		return SafeCast<UInt8>(std::max(IntegralLog2(std::max({ width, height, depth })), 1U));
	}

	inline UInt8 GetMaxLevel(ImageType type, UInt32 width, UInt32 height, UInt32 depth)
	{
		// We need image type to avoid counting layers in the level count
		switch (type)
		{
			case ImageType::E1D:
			case ImageType::E1D_Array:
				return GetMaxLevel(width, 1U, 1U);

			case ImageType::E2D:
			case ImageType::E2D_Array:
			case ImageType::Cubemap:
				return GetMaxLevel(width, height, 1U);

			case ImageType::E3D:
				return GetMaxLevel(width, height, depth);
		}

		NazaraError("Image type not handled ({0:#x})", UnderlyingCast(type));
		return 0;
	}

	inline Boxui32 RegionToArray(ImageType type, Boxui32 region, UInt32& baseLayer, UInt32& layerCount)
	{
		switch (type)
		{
			case ImageType::E1D_Array:
				baseLayer = region.y;
				layerCount = region.height;

				region.y = 0;
				region.height = 1;
				break;

			case ImageType::Cubemap:
			case ImageType::E2D_Array:
				baseLayer = region.z;
				layerCount = region.depth;

				region.z = 0;
				region.depth = 1;
				break;

			case ImageType::E1D:
				NazaraAssert(region.y == 0, "out of bounds");
				NazaraAssert(region.height <= 1, "out of bounds");
				[[fallthrough]];
			case ImageType::E2D:
				NazaraAssert(region.z == 0, "out of bounds");
				NazaraAssert(region.depth <= 1, "out of bounds");
				[[fallthrough]];
			case ImageType::E3D:
				baseLayer = 0;
				layerCount = 1;
				break;
		}

		return region;
	}
}
