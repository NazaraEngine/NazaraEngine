// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/PixelFormat.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <array>
#include <cstring>
#include <Nazara/Utility/Debug.hpp>

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

	inline PixelFormatDescription::PixelFormatDescription(const std::string& formatName, PixelFormatContent formatContent, UInt8 bpp, PixelFormatSubType subType) :
	name(formatName),
	content(formatContent),
	redType(subType),
	greenType(subType),
	blueType(subType),
	alphaType(subType),
	bitsPerPixel(bpp)
	{
	}

	inline PixelFormatDescription::PixelFormatDescription(const std::string& formatName, PixelFormatContent formatContent, Bitset<> rMask, Bitset<> gMask, Bitset<> bMask, Bitset<> aMask, PixelFormatSubType subType) :
	PixelFormatDescription(formatName, formatContent, subType, rMask, subType, gMask, subType, bMask, subType, aMask)
	{
	}

	inline PixelFormatDescription::PixelFormatDescription(const std::string& formatName, PixelFormatContent formatContent, PixelFormatSubType rType, Bitset<> rMask, PixelFormatSubType gType, Bitset<> gMask, PixelFormatSubType bType, Bitset<> bMask, PixelFormatSubType aType, Bitset<> aMask, UInt8 bpp) :
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
		name.clear();
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

		bitsPerPixel = static_cast<UInt8>(counter.Count());
	}

	inline bool PixelFormatDescription::Validate() const
	{
		if (!IsValid())
			return false;

		if (content <= PixelFormatContent::Undefined || content > PixelFormatContent::Max)
			return false;

		std::array<const Nz::Bitset<>*, 4> masks = { {&redMask, &greenMask, &blueMask, &alphaMask} };
		std::array<PixelFormatSubType, 4> types = { {redType, greenType, blueType, alphaType} };

		for (unsigned int i = 0; i < 4; ++i)
		{
			UInt8 usedBits = static_cast<UInt8>(masks[i]->Count());
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



	inline std::size_t PixelFormatInfo::ComputeSize(PixelFormat format, unsigned int width, unsigned int height, unsigned int depth)
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
					NazaraError("Unsupported format");
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

		#if NAZARA_UTILITY_SAFE
		if (IsCompressed(srcFormat))
		{
			NazaraError("Cannot convert single pixel from compressed format");
			return false;
		}

		if (IsCompressed(dstFormat))
		{
			NazaraError("Cannot convert single pixel to compressed format");
			return false;
		}
		#endif

		return Convert(srcFormat, dstFormat, src, static_cast<const UInt8*>(src) + GetBytesPerPixel(srcFormat), dst);
	}

	inline bool PixelFormatInfo::Convert(PixelFormat srcFormat, PixelFormat dstFormat, const void* start, const void* end, void* dst)
	{
		if (srcFormat == dstFormat)
		{
			std::memcpy(dst, start, reinterpret_cast<const UInt8*>(end) - reinterpret_cast<const UInt8*>(start));
			return true;
		}

		ConvertFunction func = s_convertFunctions[UnderlyingCast(srcFormat)][UnderlyingCast(dstFormat)];
		if (!func)
		{
			NazaraError("Pixel format conversion from " + GetName(srcFormat) + " to " + GetName(dstFormat) + " is not supported");
			return false;
		}

		if (!func(reinterpret_cast<const UInt8*>(start), reinterpret_cast<const UInt8*>(end), reinterpret_cast<UInt8*>(dst)))
		{
			NazaraError("Pixel format conversion from " + GetName(srcFormat) + " to " + GetName(dstFormat) + " failed");
			return false;
		}

		return true;
	}

	inline UInt8 PixelFormatInfo::GetBitsPerPixel(PixelFormat format)
	{
		return s_pixelFormatInfos[UnderlyingCast(format)].bitsPerPixel;
	}

	inline UInt8 PixelFormatInfo::GetBytesPerPixel(PixelFormat format)
	{
		return GetBitsPerPixel(format)/8;
	}

	inline PixelFormatContent PixelFormatInfo::GetContent(PixelFormat format)
	{
		return s_pixelFormatInfos[UnderlyingCast(format)].content;
	}

	inline const PixelFormatDescription& PixelFormatInfo::GetInfo(PixelFormat format)
	{
		return s_pixelFormatInfos[UnderlyingCast(format)];
	}

	inline const std::string& PixelFormatInfo::GetName(PixelFormat format)
	{
		return s_pixelFormatInfos[UnderlyingCast(format)].name;
	}

	inline bool PixelFormatInfo::HasAlpha(PixelFormat format)
	{
		return s_pixelFormatInfos[UnderlyingCast(format)].alphaMask.TestAny();
	}

	inline bool PixelFormatInfo::IsCompressed(PixelFormat format)
	{
		return s_pixelFormatInfos[UnderlyingCast(format)].IsCompressed();
	}

	inline bool PixelFormatInfo::IsConversionSupported(PixelFormat srcFormat, PixelFormat dstFormat)
	{
		if (srcFormat == dstFormat)
			return true;

		return s_convertFunctions[UnderlyingCast(srcFormat)][UnderlyingCast(dstFormat)] != nullptr;
	}

	inline bool PixelFormatInfo::IsValid(PixelFormat format)
	{
		return format != PixelFormat::Undefined;
	}

	inline void PixelFormatInfo::SetConvertFunction(PixelFormat srcFormat, PixelFormat dstFormat, ConvertFunction func)
	{
		s_convertFunctions[UnderlyingCast(srcFormat)][UnderlyingCast(dstFormat)] = func;
	}

	inline void PixelFormatInfo::SetFlipFunction(PixelFlipping flipping, PixelFormat format, FlipFunction func)
	{
		s_flipFunctions[UnderlyingCast(flipping)][UnderlyingCast(format)] = func;
	}
}

#include <Nazara/Utility/DebugOff.hpp>
