// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/PixelFormat.hpp>
#include <Nazara/Core/Error.hpp>
#include <algorithm>
#include <array>
#include <cstring>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	inline PixelFormatInfo::PixelFormatInfo() :
	content(PixelFormatContent_Undefined),
	bitsPerPixel(0)
	{
	}

	inline PixelFormatInfo::PixelFormatInfo(PixelFormatContent formatContent, UInt8 bpp, PixelFormatSubType subType) :
	content(formatContent),
	redType(subType),
	greenType(subType),
	blueType(subType),
	alphaType(subType),
	bitsPerPixel(bpp)
	{
	}

	inline PixelFormatInfo::PixelFormatInfo(const String& formatName, PixelFormatContent formatContent, UInt8 bpp, PixelFormatSubType subType) :
	content(formatContent),
	redType(subType),
	greenType(subType),
	blueType(subType),
	alphaType(subType),
	name(formatName),
	bitsPerPixel(bpp)
	{
	}

	inline PixelFormatInfo::PixelFormatInfo(const String& formatName, PixelFormatContent formatContent, Bitset<> rMask, Bitset<> gMask, Bitset<> bMask, Bitset<> aMask, PixelFormatSubType subType) :
	PixelFormatInfo(formatName, formatContent, subType, rMask, subType, gMask, subType, bMask, subType, aMask)
	{
	}

	inline PixelFormatInfo::PixelFormatInfo(const String& formatName, PixelFormatContent formatContent, PixelFormatSubType rType, Bitset<> rMask, PixelFormatSubType gType, Bitset<> gMask, PixelFormatSubType bType, Bitset<> bMask, PixelFormatSubType aType, Bitset<> aMask, UInt8 bpp) :
	redMask(rMask),
	greenMask(gMask),
	blueMask(bMask),
	alphaMask(aMask),
	content(formatContent),
	redType(rType),
	greenType(gType),
	blueType(bType),
	alphaType(aType),
	name(formatName)
	{
		redMask.Reverse();
		greenMask.Reverse();
		blueMask.Reverse();
		alphaMask.Reverse();

		if (bpp == 0)
			RecomputeBitsPerPixel();
	}

	inline void PixelFormatInfo::Clear()
	{
		bitsPerPixel = 0;
		alphaMask.Clear();
		blueMask.Clear();
		greenMask.Clear();
		redMask.Clear();
		name.Clear();
	}

	inline bool PixelFormatInfo::IsCompressed() const
	{
		return redType   == PixelFormatSubType_Compressed ||
		       greenType == PixelFormatSubType_Compressed ||
		       blueType  == PixelFormatSubType_Compressed ||
		       alphaType == PixelFormatSubType_Compressed;
	}

	inline bool PixelFormatInfo::IsValid() const
	{
		return bitsPerPixel != 0;
	}

	inline void PixelFormatInfo::RecomputeBitsPerPixel()
	{
		Bitset<> counter;
		counter |= redMask;
		counter |= greenMask;
		counter |= blueMask;
		counter |= alphaMask;

		bitsPerPixel = static_cast<UInt8>(counter.Count());
	}

	inline bool PixelFormatInfo::Validate() const
	{
		if (!IsValid())
			return false;

		if (content <= PixelFormatContent_Undefined || content > PixelFormatContent_Max)
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
				case PixelFormatSubType_Half:
					if (usedBits != 16)
						return false;

					break;

				case PixelFormatSubType_Float:
					if (usedBits != 32)
						return false;

					break;

				default:
					break;
			}
		}

		return true;
	}



	inline std::size_t PixelFormat::ComputeSize(PixelFormatType format, unsigned int width, unsigned int height, unsigned int depth)
	{
		if (IsCompressed(format))
		{
			switch (format)
			{
				case PixelFormatType_DXT1:
				case PixelFormatType_DXT3:
				case PixelFormatType_DXT5:
					return (((width + 3) / 4) * ((height + 3) / 4) * ((format == PixelFormatType_DXT1) ? 8 : 16)) * depth;

				default:
					NazaraError("Unsupported format");
					return 0;
			}
		}
		else
			return width * height * depth * GetBytesPerPixel(format);
	}

	inline bool PixelFormat::Convert(PixelFormatType srcFormat, PixelFormatType dstFormat, const void* src, void* dst)
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

		ConvertFunction func = s_convertFunctions[srcFormat][dstFormat];
		if (!func)
		{
			NazaraError("Pixel format conversion from " + GetName(srcFormat) + " to " + GetName(dstFormat) + " is not supported");
			return false;
		}

		if (!func(reinterpret_cast<const UInt8*>(src), reinterpret_cast<const UInt8*>(src) + GetBytesPerPixel(srcFormat), reinterpret_cast<UInt8*>(dst)))
		{
			NazaraError("Pixel format conversion from " + GetName(srcFormat) + " to " + GetName(dstFormat) + " failed");
			return false;
		}

		return true;
	}

	inline bool PixelFormat::Convert(PixelFormatType srcFormat, PixelFormatType dstFormat, const void* start, const void* end, void* dst)
	{
		if (srcFormat == dstFormat)
		{
			std::memcpy(dst, start, reinterpret_cast<const UInt8*>(end)-reinterpret_cast<const UInt8*>(start));
			return true;
		}

		ConvertFunction func = s_convertFunctions[srcFormat][dstFormat];
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

	inline UInt8 PixelFormat::GetBitsPerPixel(PixelFormatType format)
	{
		return s_pixelFormatInfos[format].bitsPerPixel;
	}

	inline UInt8 PixelFormat::GetBytesPerPixel(PixelFormatType format)
	{
		return GetBitsPerPixel(format)/8;
	}

	inline PixelFormatContent PixelFormat::GetContent(PixelFormatType format)
	{
		return s_pixelFormatInfos[format].content;
	}

	inline const PixelFormatInfo& PixelFormat::GetInfo(PixelFormatType format)
	{
		return s_pixelFormatInfos[format];
	}

	inline const String& PixelFormat::GetName(PixelFormatType format)
	{
		return s_pixelFormatInfos[format].name;
	}

	inline bool PixelFormat::HasAlpha(PixelFormatType format)
	{
		return s_pixelFormatInfos[format].alphaMask.TestAny();
	}

	inline bool PixelFormat::IsCompressed(PixelFormatType format)
	{
		return s_pixelFormatInfos[format].IsCompressed();
	}

	inline bool PixelFormat::IsConversionSupported(PixelFormatType srcFormat, PixelFormatType dstFormat)
	{
		if (srcFormat == dstFormat)
			return true;

		return s_convertFunctions[srcFormat][dstFormat] != nullptr;
	}

	inline bool PixelFormat::IsValid(PixelFormatType format)
	{
		return format != PixelFormatType_Undefined;
	}

	inline void PixelFormat::SetConvertFunction(PixelFormatType srcFormat, PixelFormatType dstFormat, ConvertFunction func)
	{
		s_convertFunctions[srcFormat][dstFormat] = func;
	}

	inline void PixelFormat::SetFlipFunction(PixelFlipping flipping, PixelFormatType format, FlipFunction func)
	{
		s_flipFunctions[flipping][format] = func;
	}
}

#include <Nazara/Utility/DebugOff.hpp>
