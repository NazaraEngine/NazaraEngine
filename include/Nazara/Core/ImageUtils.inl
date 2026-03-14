// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
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
				NazaraAssertMsg(baseLayer == 0, "out of bounds");
				NazaraAssertMsg(layerCount <= 1, "out of bounds");
				[[fallthrough]];
			case ImageType::E2D:
				NazaraAssertMsg(baseLayer == 0, "out of bounds");
				NazaraAssertMsg(layerCount <= 1, "out of bounds");
				[[fallthrough]];
			case ImageType::E3D:
				region.z = 0;
				region.depth = 1;
				break;
		}
	}

	inline void Copy(void* destination, const void* source, PixelFormat format, UInt32 width, UInt32 height, UInt32 depth, UInt32 dstRowStride, UInt32 dstDepthStride, UInt32 srcRowStride, UInt32 srcDepthStride)
	{
		NazaraAssertMsg(width > 0, "width must be greater than zero");
		NazaraAssertMsg(height > 0, "height must be greater than zero");
		NazaraAssertMsg(depth > 0, "depth must be greater than zero");

		UInt8* dstPtr = static_cast<UInt8*>(destination);
		const UInt8* srcPtr = static_cast<const UInt8*>(source);

		UInt8 bpp = PixelFormatInfo::GetBytesPerPixel(format);

		if (dstRowStride == 0)
			dstRowStride = width * bpp;

		if (dstDepthStride == 0)
			dstDepthStride = width * height * bpp;

		if (srcRowStride == 0)
			srcRowStride = width * bpp;

		if (srcDepthStride == 0)
			srcDepthStride = width * height * bpp;

		if ((height == 1 || dstRowStride == srcRowStride) && (depth == 1 || dstDepthStride == srcDepthStride))
			std::memcpy(dstPtr, source, PixelFormatInfo::ComputeSize(format, width, height, depth));
		else
		{
			std::size_t lineSize = static_cast<std::size_t>(width) * bpp;

			for (UInt32 i = 0; i < depth; ++i)
			{
				UInt8* dstFacePtr = dstPtr;
				const UInt8* srcFacePtr = srcPtr;
				for (UInt32 y = 0; y < height; ++y)
				{
					std::memcpy(dstFacePtr, srcFacePtr, lineSize);

					dstFacePtr += dstRowStride;
					srcFacePtr += srcRowStride;
				}

				dstPtr += dstDepthStride;
				srcPtr += srcDepthStride;
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
				if (!callback(SafeCast<UInt8>(i), width, height, depth))
					return false;
			}
			else if constexpr (std::is_void_v<Ret>)
				callback(SafeCast<UInt8>(i), width, height, depth);
			else
				static_assert(AlwaysFalse<Ret>(), "callback must either return a bool or nothing");

			switch (type)
			{
				case ImageType::E3D:
					depth = std::max(depth >> 1u, 1u);
					[[fallthrough]];
				case ImageType::E2D:
				case ImageType::E2D_Array:
				case ImageType::Cubemap:
					height = std::max(height >> 1u, 1u);
					[[fallthrough]];
				case ImageType::E1D:
				case ImageType::E1D_Array:
					width = std::max(width >> 1u, 1u);
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
				NazaraAssertMsg(region.y == 0, "out of bounds");
				NazaraAssertMsg(region.height <= 1, "out of bounds");
				[[fallthrough]];
			case ImageType::E2D:
				NazaraAssertMsg(region.z == 0, "out of bounds");
				NazaraAssertMsg(region.depth <= 1, "out of bounds");
				[[fallthrough]];
			case ImageType::E3D:
				baseLayer = 0;
				layerCount = 1;
				break;
		}

		return region;
	}
}
