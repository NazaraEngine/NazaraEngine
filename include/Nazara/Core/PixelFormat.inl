// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Error.hpp>
#include <NazaraUtils/Algorithm.hpp>
#include <array>
#include <cstring>

namespace Nz
{
	inline PixelFormatDescription::PixelFormatDescription() :
	content(PixelFormatContent::Undefined),
	bitsPerPixel(0)
	{
	}

	inline void PixelFormatDescription::Clear()
	{
		bitsPerPixel = 0;
		alphaMask.Clear();
		blueMask.Clear();
		greenMask.Clear();
		redMask.Clear();
		name = "";
	}

	inline bool PixelFormatDescription::IsBlockCompressed() const
	{
		return dataType == PixelFormatDataType::BlockCompressed;
	}

	inline bool PixelFormatDescription::IsCompressed() const
	{
		return dataType == PixelFormatDataType::BlockCompressed;
	}

	inline bool PixelFormatDescription::IsUncompressed() const
	{
		return !IsCompressed();
	}

	inline bool PixelFormatDescription::IsValid() const
	{
		return bitsPerPixel != 0;
	}

	inline void PixelFormatDescription::RecomputeBitsPerPixel()
	{
		Bitmask counter;
		counter |= redMask;
		counter |= greenMask;
		counter |= blueMask;
		counter |= alphaMask;

		bitsPerPixel = SafeCast<UInt8>(counter.Count());
	}

	inline bool PixelFormatDescription::Validate() const
	{
		if (!IsValid())
			return false;

		if (content <= PixelFormatContent::Undefined || content > PixelFormatContent::Max)
			return false;

		if (IsUncompressed())
		{
			std::array<const Bitmask*, 4> masks = { {&redMask, &greenMask, &blueMask, &alphaMask} };

			for (UInt32 i = 0; i < 4; ++i)
			{
				UInt8 usedBits = SafeCast<UInt8>(masks[i]->Count());
				if (usedBits == 0)
					continue;

				if (usedBits > bitsPerPixel)
					return false;

				if (usedBits > 64) //< Currently, formats with over 64 bits per component are not supported
					return false;
			}
		}

		return true;
	}

	inline PixelFormatDescription PixelFormatDescription::BlockCompressed(std::string_view formatName, PixelFormatContent formatContent, UInt8 bytesPerBlock, UInt8 blockSize)
	{
		PixelFormatDescription desc;
		desc.name = formatName;
		desc.content = formatContent;
		desc.dataType = PixelFormatDataType::BlockCompressed;
		desc.blockSize = blockSize;
		desc.bytesPerBlock = bytesPerBlock;

		return desc;
	}

	inline PixelFormatDescription PixelFormatDescription::Regular(std::string_view formatName, PixelFormatContent formatContent, Bitmask rMask, Bitmask gMask, Bitmask bMask, Bitmask aMask, PixelFormatDataType dataType)
	{
		PixelFormatDescription desc;
		desc.name = formatName;
		desc.redMask = rMask;
		desc.greenMask = gMask;
		desc.blueMask = bMask;
		desc.alphaMask = aMask;
		desc.content = formatContent;
		desc.dataType = dataType;

		// FIXME: Is this still relevant?
		desc.redMask.Reverse();
		desc.greenMask.Reverse();
		desc.blueMask.Reverse();
		desc.alphaMask.Reverse();

		desc.RecomputeBitsPerPixel();

		return desc;
	}

	inline std::size_t PixelFormatInfo::ComputeSize(PixelFormat format, UInt32 width, UInt32 height, UInt32 depth)
	{
		if (IsBlockCompressed(format))
		{
			UInt32 blockSize = GetBlockSize(format);
			return AlignPow2(width, blockSize) / blockSize * AlignPow2(height, blockSize) / blockSize * depth * GetBytesPerBlock(format);
		}
		else
			return width * height * depth * GetBytesPerPixel(format);
	}

	inline bool PixelFormatInfo::Convert(PixelFormat srcFormat, PixelFormat dstFormat, const void* src, void* dst)
	{
		if (srcFormat == dstFormat)
		{
			std::memcpy(dst, src, GetBytesPerPixel(srcFormat));
			return true;
		}

		NazaraAssertMsg(!IsCompressed(srcFormat), "cannot convert single pixel from compressed format");
		NazaraAssertMsg(!IsCompressed(dstFormat), "cannot convert single pixel to compressed format");

		return Convert(srcFormat, dstFormat, src, static_cast<const UInt8*>(src) + GetBytesPerPixel(srcFormat), dst);
	}

	inline bool PixelFormatInfo::Convert(PixelFormat srcFormat, PixelFormat dstFormat, const void* start, const void* end, void* dst)
	{
		if (srcFormat == dstFormat)
		{
			std::memcpy(dst, start, reinterpret_cast<const UInt8*>(end) - reinterpret_cast<const UInt8*>(start));
			return true;
		}

		ConvertFunction func = s_convertFunctions[srcFormat][dstFormat];
		if (!func)
		{
			NazaraError("pixel format conversion from {0} to {1} is not supported", GetName(srcFormat), GetName(dstFormat));
			return false;
		}

		if (!func(reinterpret_cast<const UInt8*>(start), reinterpret_cast<const UInt8*>(end), reinterpret_cast<UInt8*>(dst)))
		{
			NazaraError("pixel format conversion from {0} to {1} failed", GetName(srcFormat), GetName(dstFormat));
			return false;
		}

		return true;
	}

	inline UInt8 PixelFormatInfo::GetBitsPerPixel(PixelFormat format)
	{
		NazaraAssertMsg(IsUncompressed(format), "format is compressed and has no meaningful bpp");
		return s_pixelFormatInfos[format].bitsPerPixel;
	}

	inline UInt8 PixelFormatInfo::GetBlockSize(PixelFormat format)
	{
		NazaraAssertMsg(IsBlockCompressed(format), "format is not block compressed");
		return s_pixelFormatInfos[format].blockSize;
	}

	inline UInt8 PixelFormatInfo::GetBytesPerBlock(PixelFormat format)
	{
		NazaraAssertMsg(IsBlockCompressed(format), "format is not block compressed");
		return s_pixelFormatInfos[format].bytesPerBlock;
	}

	inline UInt8 PixelFormatInfo::GetBytesPerPixel(PixelFormat format)
	{
		return GetBitsPerPixel(format)/8;
	}

	inline PixelFormatContent PixelFormatInfo::GetContent(PixelFormat format)
	{
		return s_pixelFormatInfos[format].content;
	}

	inline const PixelFormatDescription& PixelFormatInfo::GetInfo(PixelFormat format)
	{
		return s_pixelFormatInfos[format];
	}

	inline std::string_view PixelFormatInfo::GetName(PixelFormat format)
	{
		return s_pixelFormatInfos[format].name;
	}

	inline bool PixelFormatInfo::HasAlpha(PixelFormat format)
	{
		return s_pixelFormatInfos[format].alphaMask.TestAny();
	}

	inline PixelFormat PixelFormatInfo::IdentifyFormat(const PixelFormatDescription& info)
	{
		for (auto&& [format, formatDesc] : s_pixelFormatInfos.iter_kv())
		{
			if (info.bitsPerPixel == formatDesc.bitsPerPixel && info.content == formatDesc.content
			 && info.redMask == formatDesc.redMask && info.greenMask == formatDesc.greenMask && info.blueMask == formatDesc.blueMask && info.alphaMask == formatDesc.alphaMask
			 && info.dataType == formatDesc.dataType)
			{
				return format;
			}
		}

		return PixelFormat::Undefined;
	}

	inline PixelFormat PixelFormatInfo::IdentifyFormat(std::string_view formatName)
	{
		for (auto&& [format, formatDesc] : s_pixelFormatInfos.iter_kv())
		{
			if (formatDesc.name == formatName)
				return format;
		}

		return PixelFormat::Undefined;
	}

	inline bool PixelFormatInfo::IsBlockCompressed(PixelFormat format)
	{
		return s_pixelFormatInfos[format].IsBlockCompressed();
	}

	inline bool PixelFormatInfo::IsCompressed(PixelFormat format)
	{
		return s_pixelFormatInfos[format].IsCompressed();
	}

	inline bool PixelFormatInfo::IsConversionSupported(PixelFormat srcFormat, PixelFormat dstFormat)
	{
		if (srcFormat == dstFormat)
			return true;

		return s_convertFunctions[srcFormat][dstFormat] != nullptr;
	}

	inline bool PixelFormatInfo::IsUncompressed(PixelFormat format)
	{
		return s_pixelFormatInfos[format].IsUncompressed();
	}

	inline bool PixelFormatInfo::IsValid(PixelFormat format)
	{
		return format != PixelFormat::Undefined;
	}

	inline void PixelFormatInfo::SetConvertFunction(PixelFormat srcFormat, PixelFormat dstFormat, ConvertFunction func)
	{
		s_convertFunctions[srcFormat][dstFormat] = func;
	}

	inline void PixelFormatInfo::SetFlipFunction(PixelFlipping flipping, PixelFormat format, FlipFunction func)
	{
		s_flipFunctions[format][flipping] = func;
	}

	inline std::optional<PixelFormat> PixelFormatInfo::ToSRGB(PixelFormat format)
	{
		switch (format)
		{
			case PixelFormat::BGR8:  return PixelFormat::BGR8_SRGB;
			case PixelFormat::BGRA8: return PixelFormat::BGRA8_SRGB;
			case PixelFormat::RGB8:  return PixelFormat::RGB8_SRGB;
			case PixelFormat::RGBA8: return PixelFormat::RGBA8_SRGB;
			default:                 return {};
		}
	}
}
