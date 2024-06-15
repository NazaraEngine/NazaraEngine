// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
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

	inline PixelFormatDescription::PixelFormatDescription(PixelFormatContent formatContent, UInt8 bpp, PixelFormatSubType subType) :
	content(formatContent),
	redType(subType),
	greenType(subType),
	blueType(subType),
	alphaType(subType),
	bitsPerPixel(bpp)
	{
	}

	inline PixelFormatDescription::PixelFormatDescription(std::string_view formatName, PixelFormatContent formatContent, UInt8 bpp, PixelFormatSubType subType) :
	name(formatName),
	content(formatContent),
	redType(subType),
	greenType(subType),
	blueType(subType),
	alphaType(subType),
	bitsPerPixel(bpp)
	{
	}

	inline PixelFormatDescription::PixelFormatDescription(std::string_view formatName, PixelFormatContent formatContent, Bitset<> rMask, Bitset<> gMask, Bitset<> bMask, Bitset<> aMask, PixelFormatSubType subType) :
	PixelFormatDescription(formatName, formatContent, subType, rMask, subType, gMask, subType, bMask, subType, aMask)
	{
	}

	inline PixelFormatDescription::PixelFormatDescription(std::string_view formatName, PixelFormatContent formatContent, PixelFormatSubType rType, Bitset<> rMask, PixelFormatSubType gType, Bitset<> gMask, PixelFormatSubType bType, Bitset<> bMask, PixelFormatSubType aType, Bitset<> aMask, UInt8 bpp) :
	name(formatName),
	redMask(rMask),
	greenMask(gMask),
	blueMask(bMask),
	alphaMask(aMask),
	content(formatContent),
	redType(rType),
	greenType(gType),
	blueType(bType),
	alphaType(aType)
	{
		redMask.Reverse();
		greenMask.Reverse();
		blueMask.Reverse();
		alphaMask.Reverse();

		if (bpp == 0)
			RecomputeBitsPerPixel();
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

	inline bool PixelFormatDescription::IsCompressed() const
	{
		return redType   == PixelFormatSubType::Compressed ||
		       greenType == PixelFormatSubType::Compressed ||
		       blueType  == PixelFormatSubType::Compressed ||
		       alphaType == PixelFormatSubType::Compressed;
	}

	inline bool PixelFormatDescription::IsValid() const
	{
		return bitsPerPixel != 0;
	}

	inline void PixelFormatDescription::RecomputeBitsPerPixel()
	{
		Bitset<> counter;
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

		std::array<const Nz::Bitset<>*, 4> masks = { {&redMask, &greenMask, &blueMask, &alphaMask} };
		std::array<PixelFormatSubType, 4> types = { {redType, greenType, blueType, alphaType} };

		for (UInt32 i = 0; i < 4; ++i)
		{
			UInt8 usedBits = SafeCast<UInt8>(masks[i]->Count());
			if (usedBits == 0)
				continue;

			if (usedBits > bitsPerPixel)
				return false;

			if (usedBits > 64) //< Currently, formats with over 64 bits per component are not supported
				return false;

			switch (types[i])
			{
				case PixelFormatSubType::Half:
					if (usedBits != 16)
						return false;

					break;

				case PixelFormatSubType::Float:
					if (usedBits != 32)
						return false;

					break;

				default:
					break;
			}
		}

		return true;
	}



	inline std::size_t PixelFormatInfo::ComputeSize(PixelFormat format, UInt32 width, UInt32 height, UInt32 depth)
	{
		if (IsCompressed(format))
		{
			switch (format)
			{
				case PixelFormat::DXT1:
				case PixelFormat::DXT3:
				case PixelFormat::DXT5:
					return (((width + 3) / 4) * ((height + 3) / 4) * ((format == PixelFormat::DXT1) ? 8 : 16)) * depth;

				default:
					NazaraError("unsupported format");
					return 0;
			}
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

		NazaraAssert(!IsCompressed(srcFormat), "cannot convert single pixel from compressed format");
		NazaraAssert(!IsCompressed(dstFormat), "cannot convert single pixel to compressed format");

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
			NazaraErrorFmt("pixel format conversion from {0} to {1} is not supported", GetName(srcFormat), GetName(dstFormat));
			return false;
		}

		if (!func(reinterpret_cast<const UInt8*>(start), reinterpret_cast<const UInt8*>(end), reinterpret_cast<UInt8*>(dst)))
		{
			NazaraErrorFmt("pixel format conversion from {0} to {1} failed", GetName(srcFormat), GetName(dstFormat));
			return false;
		}

		return true;
	}

	inline UInt8 PixelFormatInfo::GetBitsPerPixel(PixelFormat format)
	{
		return s_pixelFormatInfos[format].bitsPerPixel;
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
			if (info.bitsPerPixel == formatDesc.bitsPerPixel && info.content == formatDesc.content &&
				info.redMask == formatDesc.redMask && info.greenMask == formatDesc.greenMask && info.blueMask == formatDesc.blueMask && info.alphaMask == formatDesc.alphaMask &&
				info.redType == formatDesc.redType && info.greenType == formatDesc.greenType && info.blueType == formatDesc.blueType && info.alphaType == formatDesc.alphaType)
				return format;
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

