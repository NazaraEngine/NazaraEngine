// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <cstring>
#include <Nazara/Utility/Debug.hpp>

inline bool NzPixelFormat::Convert(nzPixelFormat srcFormat, nzPixelFormat dstFormat, const void* src, void* dst)
{
	if (srcFormat == dstFormat)
	{
		std::memcpy(dst, src, GetBPP(srcFormat));
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
		NazaraError("Pixel format conversion from " + ToString(srcFormat) + " to " + ToString(dstFormat) + " is not supported");
		return false;
	}

	if (!func(reinterpret_cast<const nzUInt8*>(src), reinterpret_cast<const nzUInt8*>(src) + GetBPP(srcFormat), reinterpret_cast<nzUInt8*>(dst)))
	{
		NazaraError("Pixel format conversion from " + ToString(srcFormat) + " to " + ToString(dstFormat) + " failed");
		return false;
	}

	return true;
}

inline bool NzPixelFormat::Convert(nzPixelFormat srcFormat, nzPixelFormat dstFormat, const void* start, const void* end, void* dst)
{
	if (srcFormat == dstFormat)
	{
		std::memcpy(dst, start, reinterpret_cast<const nzUInt8*>(end)-reinterpret_cast<const nzUInt8*>(start));
		return true;
	}

	ConvertFunction func = s_convertFunctions[srcFormat][dstFormat];
	if (!func)
	{
		NazaraError("Pixel format conversion from " + ToString(srcFormat) + " to " + ToString(dstFormat) + " is not supported");
		return false;
	}

	if (!func(reinterpret_cast<const nzUInt8*>(start), reinterpret_cast<const nzUInt8*>(end), reinterpret_cast<nzUInt8*>(dst)))
	{
		NazaraError("Pixel format conversion from " + ToString(srcFormat) + " to " + ToString(dstFormat) + " failed");
		return false;
	}

	return true;
}

inline bool NzPixelFormat::Flip(nzPixelFlipping flipping, nzPixelFormat format, unsigned int width, unsigned int height, unsigned int depth, const void* src, void* dst)
{
	#if NAZARA_UTILITY_SAFE
	if (!IsValid(format))
	{
		NazaraError("Invalid pixel format");
		return false;
	}
	#endif

	auto it = s_flipFunctions[flipping].find(format);
	if (it != s_flipFunctions[flipping].end())
		it->second(width, height, depth, reinterpret_cast<const nzUInt8*>(src), reinterpret_cast<nzUInt8*>(dst));
	else
	{
		// Flipping générique

		#if NAZARA_UTILITY_SAFE
		if (IsCompressed(format))
		{
			NazaraError("No function to flip compressed format");
			return false;
		}
		#endif

		nzUInt8 bpp = GetBPP(format);
		unsigned int lineStride = width*bpp;
		switch (flipping)
		{
			case nzPixelFlipping_Horizontally:
			{
				if (src == dst)
				{
					for (unsigned int z = 0; z < depth; ++z)
					{
						nzUInt8* ptr = reinterpret_cast<nzUInt8*>(dst) + width*height*z;
						for (unsigned int y = 0; y < height; ++y)
						{
							for (unsigned int x = 0; x < width/2; ++x)
								std::swap_ranges(&ptr[x*bpp], &ptr[(x+1)*bpp], &ptr[(width-x)*bpp]);

							ptr += lineStride;
						}
					}
				}
				else
				{
					for (unsigned int z = 0; z < depth; ++z)
					{
						nzUInt8* ptr = reinterpret_cast<nzUInt8*>(dst) + width*height*z;
						for (unsigned int y = 0; y < height; ++y)
						{
							for (unsigned int x = 0; x < width; ++x)
								std::memcpy(&ptr[x*bpp], &ptr[(width-x)*bpp], bpp);

							ptr += lineStride;
						}
					}
				}
				break;
			}

			case nzPixelFlipping_Vertically:
			{
				if (src == dst)
				{
					for (unsigned int z = 0; z < depth; ++z)
					{
						nzUInt8* ptr = reinterpret_cast<nzUInt8*>(dst) + width*height*z;
						for (unsigned int y = 0; y < height/2; ++y)
							std::swap_ranges(&ptr[y*lineStride], &ptr[(y+1)*lineStride-1], &ptr[(height-y-1)*lineStride]);
					}
				}
				else
				{
					for (unsigned int z = 0; z < depth; ++z)
					{
						const nzUInt8* srcPtr = reinterpret_cast<const nzUInt8*>(src);
						nzUInt8* dstPtr = reinterpret_cast<nzUInt8*>(dst) + (width-1)*height*depth*bpp;
						for (unsigned int y = 0; y < height; ++y)
						{
							std::memcpy(dstPtr, srcPtr, lineStride);

							srcPtr += lineStride;
							dstPtr -= lineStride;
						}
					}
				}
				break;
			}
		}
	}

	return true;
}

inline nzUInt8 NzPixelFormat::GetBPP(nzPixelFormat format)
{
	switch (format)
	{
		case nzPixelFormat_BGR8:
			return 3;

		case nzPixelFormat_BGRA8:
			return 4;

		case nzPixelFormat_DXT1:
			return 1;

		case nzPixelFormat_DXT3:
			return 2;

		case nzPixelFormat_DXT5:
			return 2;

		case nzPixelFormat_L8:
			return 1;

		case nzPixelFormat_LA8:
			return 2;
/*
		case nzPixelFormat_RGB16F:
			return 6;

		case nzPixelFormat_RGB16I:
			return 6;

		case nzPixelFormat_RGB32F:
			return 12;

		case nzPixelFormat_RGB32I:
			return 12;

		case nzPixelFormat_RGBA16F:
			return 8;

		case nzPixelFormat_RGBA16I:
			return 8;

		case nzPixelFormat_RGBA32F:
			return 16;

		case nzPixelFormat_RGBA32I:
			return 16;
*/
		case nzPixelFormat_RGBA4:
			return 2;

		case nzPixelFormat_RGB5A1:
			return 2;

		case nzPixelFormat_RGB8:
			return 3;

		case nzPixelFormat_RGBA8:
			return 4;

		case nzPixelFormat_Undefined:
			NazaraError("Invalid pixel format");
			return 0;
	}

	NazaraInternalError("Invalid pixel format");

	return 0;
}

inline bool NzPixelFormat::HasAlpha(nzPixelFormat format)
{
	switch (format)
	{
		case nzPixelFormat_BGRA8:
		case nzPixelFormat_DXT3:
		case nzPixelFormat_DXT5:
		case nzPixelFormat_LA8:
		case nzPixelFormat_RGB5A1:
		case nzPixelFormat_RGBA4:
		case nzPixelFormat_RGBA8:
			return true;

		case nzPixelFormat_BGR8:
		case nzPixelFormat_DXT1:
		case nzPixelFormat_L8:
		case nzPixelFormat_RGB8:
			return false;

		case nzPixelFormat_Undefined:
			break;
	}

	NazaraError("Invalid pixel format");
	return false;
}

inline bool NzPixelFormat::IsCompressed(nzPixelFormat format)
{
	switch (format)
	{
		case nzPixelFormat_DXT1:
		case nzPixelFormat_DXT3:
		case nzPixelFormat_DXT5:
			return true;

		default:
			return false;
	}
}

inline bool NzPixelFormat::IsConversionSupported(nzPixelFormat srcFormat, nzPixelFormat dstFormat)
{
	if (srcFormat == dstFormat)
		return true;

	return s_convertFunctions[srcFormat][dstFormat] != nullptr;
}

inline bool NzPixelFormat::IsValid(nzPixelFormat format)
{
	return format != nzPixelFormat_Undefined;
}

inline void NzPixelFormat::SetConvertFunction(nzPixelFormat srcFormat, nzPixelFormat dstFormat, ConvertFunction func)
{
	s_convertFunctions[srcFormat][dstFormat] = func;
}

inline void NzPixelFormat::SetFlipFunction(nzPixelFlipping flipping, nzPixelFormat format, FlipFunction func)
{
	s_flipFunctions[flipping][format] = func;
}

inline NzString NzPixelFormat::ToString(nzPixelFormat format)
{
	switch (format)
	{
		case nzPixelFormat_BGR8:
			return "BGR8";

		case nzPixelFormat_BGRA8:
			return "BGRA8";

		case nzPixelFormat_DXT1:
			return "DXT1";

		case nzPixelFormat_DXT3:
			return "DXT3";

		case nzPixelFormat_DXT5:
			return "DXT5";

		case nzPixelFormat_L8:
			return "L8";

		case nzPixelFormat_LA8:
			return "LA8";
/*
		case nzPixelFormat_RGB16F:
			return "RGB16F";

		case nzPixelFormat_RGB16I:
			return "RGB16I";

		case nzPixelFormat_RGB32F:
			return "RGB32F";

		case nzPixelFormat_RGB32I:
			return "RGB32I";

		case nzPixelFormat_RGBA16F:
			return "RGBA16F";

		case nzPixelFormat_RGBA16I:
			return "RGBA16I";

		case nzPixelFormat_RGBA32F:
			return "RGBA32F";

		case nzPixelFormat_RGBA32I:
			return "RGBA32I";
*/
		case nzPixelFormat_RGBA4:
			return "RGBA4";

		case nzPixelFormat_RGB5A1:
			return "RGB5A1";

		case nzPixelFormat_RGB8:
			return "RGB8";

		case nzPixelFormat_RGBA8:
			return "RGBA8";

		case nzPixelFormat_Undefined:
			return "Undefined";
	}

	NazaraError("Invalid pixel format");
	return "Invalid format";
}

#include <Nazara/Utility/DebugOff.hpp>
