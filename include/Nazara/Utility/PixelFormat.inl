// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <algorithm>
#include <cstring>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
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
			NazaraError("Pixel format conversion from " + ToString(srcFormat) + " to " + ToString(dstFormat) + " is not supported");
			return false;
		}

		if (!func(reinterpret_cast<const UInt8*>(src), reinterpret_cast<const UInt8*>(src) + GetBytesPerPixel(srcFormat), reinterpret_cast<UInt8*>(dst)))
		{
			NazaraError("Pixel format conversion from " + ToString(srcFormat) + " to " + ToString(dstFormat) + " failed");
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
			NazaraError("Pixel format conversion from " + ToString(srcFormat) + " to " + ToString(dstFormat) + " is not supported");
			return false;
		}

		if (!func(reinterpret_cast<const UInt8*>(start), reinterpret_cast<const UInt8*>(end), reinterpret_cast<UInt8*>(dst)))
		{
			NazaraError("Pixel format conversion from " + ToString(srcFormat) + " to " + ToString(dstFormat) + " failed");
			return false;
		}

		return true;
	}

	inline bool PixelFormat::Flip(PixelFlipping flipping, PixelFormatType format, unsigned int width, unsigned int height, unsigned int depth, const void* src, void* dst)
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
			it->second(width, height, depth, reinterpret_cast<const UInt8*>(src), reinterpret_cast<UInt8*>(dst));
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

			UInt8 bpp = GetBytesPerPixel(format);
			unsigned int lineStride = width*bpp;
			switch (flipping)
			{
				case PixelFlipping_Horizontally:
				{
					if (src == dst)
					{
						for (unsigned int z = 0; z < depth; ++z)
						{
							UInt8* ptr = reinterpret_cast<UInt8*>(dst) + width*height*z;
							for (unsigned int y = 0; y < height/2; ++y)
								std::swap_ranges(&ptr[y*lineStride], &ptr[(y+1)*lineStride-1], &ptr[(height-y-1)*lineStride]);
						}
					}
					else
					{
						for (unsigned int z = 0; z < depth; ++z)
						{
							const UInt8* srcPtr = reinterpret_cast<const UInt8*>(src);
							UInt8* dstPtr = reinterpret_cast<UInt8*>(dst) + (width-1)*height*depth*bpp;
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

				case PixelFlipping_Vertically:
				{
					if (src == dst)
					{
						for (unsigned int z = 0; z < depth; ++z)
						{
							UInt8* ptr = reinterpret_cast<UInt8*>(dst) + width*height*z;
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
							UInt8* ptr = reinterpret_cast<UInt8*>(dst) + width*height*z;
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
			}
		}

		return true;
	}

	inline UInt8 PixelFormat::GetBitsPerPixel(PixelFormatType format)
	{
		switch (format)
		{
			case PixelFormatType_A8:
				return 8;

			case PixelFormatType_BGR8:
				return 24;

			case PixelFormatType_BGRA8:
				return 32;

			case PixelFormatType_DXT1:
				return 8;

			case PixelFormatType_DXT3:
				return 16;

			case PixelFormatType_DXT5:
				return 16;

			case PixelFormatType_L8:
				return 8;

			case PixelFormatType_LA8:
				return 16;

			case PixelFormatType_R8:
			case PixelFormatType_R8I:
			case PixelFormatType_R8UI:
				return 8;

			case PixelFormatType_R16:
			case PixelFormatType_R16F:
			case PixelFormatType_R16I:
			case PixelFormatType_R16UI:
				return 16;

			case PixelFormatType_R32F:
			case PixelFormatType_R32I:
			case PixelFormatType_R32UI:
				return 32;

			case PixelFormatType_RG8:
			case PixelFormatType_RG8I:
			case PixelFormatType_RG8UI:
				return 16;

			case PixelFormatType_RG16:
			case PixelFormatType_RG16F:
			case PixelFormatType_RG16I:
			case PixelFormatType_RG16UI:
				return 32;

			case PixelFormatType_RG32F:
			case PixelFormatType_RG32I:
			case PixelFormatType_RG32UI:
				return 64;

			case PixelFormatType_RGB16F:
			case PixelFormatType_RGB16I:
			case PixelFormatType_RGB16UI:
				return 48;

			case PixelFormatType_RGB32F:
			case PixelFormatType_RGB32I:
			case PixelFormatType_RGB32UI:
				return 96;

			case PixelFormatType_RGBA16F:
			case PixelFormatType_RGBA16I:
			case PixelFormatType_RGBA16UI:
				return 64;

			case PixelFormatType_RGBA32F:
			case PixelFormatType_RGBA32I:
			case PixelFormatType_RGBA32UI:
				return 128;

			case PixelFormatType_RGBA4:
				return 16;

			case PixelFormatType_RGB5A1:
				return 16;

			case PixelFormatType_RGB8:
				return 24;

			case PixelFormatType_RGBA8:
				return 32;

			case PixelFormatType_Depth16:
				return 16;

			case PixelFormatType_Depth24:
				return 24;

			case PixelFormatType_Depth24Stencil8:
				return 32;

			case PixelFormatType_Depth32:
				return 32;

			case PixelFormatType_Stencil1:
				return 1;

			case PixelFormatType_Stencil4:
				return 2;

			case PixelFormatType_Stencil8:
				return 8;

			case PixelFormatType_Stencil16:
				return 16;

			case PixelFormatType_Undefined:
				break;
		}

		NazaraError("Invalid pixel format");
		return 0;
	}

	inline UInt8 PixelFormat::GetBytesPerPixel(PixelFormatType format)
	{
		return GetBitsPerPixel(format)/8;
	}

	inline PixelFormatTypeType PixelFormat::GetType(PixelFormatType format)
	{
		switch (format)
		{
			case PixelFormatType_A8:
			case PixelFormatType_BGR8:
			case PixelFormatType_BGRA8:
			case PixelFormatType_DXT1:
			case PixelFormatType_DXT3:
			case PixelFormatType_DXT5:
			case PixelFormatType_L8:
			case PixelFormatType_LA8:
			case PixelFormatType_R8:
			case PixelFormatType_R8I:
			case PixelFormatType_R8UI:
			case PixelFormatType_R16:
			case PixelFormatType_R16F:
			case PixelFormatType_R16I:
			case PixelFormatType_R16UI:
			case PixelFormatType_R32F:
			case PixelFormatType_R32I:
			case PixelFormatType_R32UI:
			case PixelFormatType_RG8:
			case PixelFormatType_RG8I:
			case PixelFormatType_RG8UI:
			case PixelFormatType_RG16:
			case PixelFormatType_RG16F:
			case PixelFormatType_RG16I:
			case PixelFormatType_RG16UI:
			case PixelFormatType_RG32F:
			case PixelFormatType_RG32I:
			case PixelFormatType_RG32UI:
			case PixelFormatType_RGB5A1:
			case PixelFormatType_RGB8:
			case PixelFormatType_RGB16F:
			case PixelFormatType_RGB16I:
			case PixelFormatType_RGB16UI:
			case PixelFormatType_RGB32F:
			case PixelFormatType_RGB32I:
			case PixelFormatType_RGB32UI:
			case PixelFormatType_RGBA4:
			case PixelFormatType_RGBA8:
			case PixelFormatType_RGBA16F:
			case PixelFormatType_RGBA16I:
			case PixelFormatType_RGBA16UI:
			case PixelFormatType_RGBA32F:
			case PixelFormatType_RGBA32I:
			case PixelFormatType_RGBA32UI:
				return PixelFormatTypeType_Color;

			case PixelFormatType_Depth16:
			case PixelFormatType_Depth24:
			case PixelFormatType_Depth32:
				return PixelFormatTypeType_Depth;

			case PixelFormatType_Depth24Stencil8:
				return PixelFormatTypeType_DepthStencil;

			case PixelFormatType_Stencil1:
			case PixelFormatType_Stencil4:
			case PixelFormatType_Stencil8:
			case PixelFormatType_Stencil16:
				return PixelFormatTypeType_Stencil;

			case PixelFormatType_Undefined:
				break;
		}

		NazaraError("Invalid pixel format");
		return PixelFormatTypeType_Undefined;
	}

	inline bool PixelFormat::HasAlpha(PixelFormatType format)
	{
		switch (format)
		{
			case PixelFormatType_A8:
			case PixelFormatType_BGRA8:
			case PixelFormatType_DXT3:
			case PixelFormatType_DXT5:
			case PixelFormatType_LA8:
			case PixelFormatType_RGB5A1:
			case PixelFormatType_RGBA16F:
			case PixelFormatType_RGBA16I:
			case PixelFormatType_RGBA16UI:
			case PixelFormatType_RGBA32F:
			case PixelFormatType_RGBA32I:
			case PixelFormatType_RGBA32UI:
			case PixelFormatType_RGBA4:
			case PixelFormatType_RGBA8:
				return true;

			case PixelFormatType_BGR8:
			case PixelFormatType_DXT1:
			case PixelFormatType_L8:
			case PixelFormatType_R8:
			case PixelFormatType_R8I:
			case PixelFormatType_R8UI:
			case PixelFormatType_R16:
			case PixelFormatType_R16F:
			case PixelFormatType_R16I:
			case PixelFormatType_R16UI:
			case PixelFormatType_R32F:
			case PixelFormatType_R32I:
			case PixelFormatType_R32UI:
			case PixelFormatType_RG8:
			case PixelFormatType_RG8I:
			case PixelFormatType_RG8UI:
			case PixelFormatType_RG16:
			case PixelFormatType_RG16F:
			case PixelFormatType_RG16I:
			case PixelFormatType_RG16UI:
			case PixelFormatType_RG32F:
			case PixelFormatType_RG32I:
			case PixelFormatType_RG32UI:
			case PixelFormatType_RGB8:
			case PixelFormatType_RGB16F:
			case PixelFormatType_RGB16I:
			case PixelFormatType_RGB16UI:
			case PixelFormatType_RGB32F:
			case PixelFormatType_RGB32I:
			case PixelFormatType_RGB32UI:
			case PixelFormatType_Depth16:
			case PixelFormatType_Depth24:
			case PixelFormatType_Depth24Stencil8:
			case PixelFormatType_Depth32:
			case PixelFormatType_Stencil1:
			case PixelFormatType_Stencil4:
			case PixelFormatType_Stencil8:
			case PixelFormatType_Stencil16:
				return false;

			case PixelFormatType_Undefined:
				break;
		}

		NazaraError("Invalid pixel format");
		return false;
	}

	inline bool PixelFormat::IsCompressed(PixelFormatType format)
	{
		switch (format)
		{
			case PixelFormatType_DXT1:
			case PixelFormatType_DXT3:
			case PixelFormatType_DXT5:
				return true;

			case PixelFormatType_A8:
			case PixelFormatType_BGR8:
			case PixelFormatType_BGRA8:
			case PixelFormatType_L8:
			case PixelFormatType_LA8:
			case PixelFormatType_R8:
			case PixelFormatType_R8I:
			case PixelFormatType_R8UI:
			case PixelFormatType_R16:
			case PixelFormatType_R16F:
			case PixelFormatType_R16I:
			case PixelFormatType_R16UI:
			case PixelFormatType_R32F:
			case PixelFormatType_R32I:
			case PixelFormatType_R32UI:
			case PixelFormatType_RG8:
			case PixelFormatType_RG8I:
			case PixelFormatType_RG8UI:
			case PixelFormatType_RG16:
			case PixelFormatType_RG16F:
			case PixelFormatType_RG16I:
			case PixelFormatType_RG16UI:
			case PixelFormatType_RG32F:
			case PixelFormatType_RG32I:
			case PixelFormatType_RG32UI:
			case PixelFormatType_RGB5A1:
			case PixelFormatType_RGB8:
			case PixelFormatType_RGB16F:
			case PixelFormatType_RGB16I:
			case PixelFormatType_RGB16UI:
			case PixelFormatType_RGB32F:
			case PixelFormatType_RGB32I:
			case PixelFormatType_RGB32UI:
			case PixelFormatType_RGBA4:
			case PixelFormatType_RGBA8:
			case PixelFormatType_RGBA16F:
			case PixelFormatType_RGBA16I:
			case PixelFormatType_RGBA16UI:
			case PixelFormatType_RGBA32F:
			case PixelFormatType_RGBA32I:
			case PixelFormatType_RGBA32UI:
			case PixelFormatType_Depth16:
			case PixelFormatType_Depth24:
			case PixelFormatType_Depth24Stencil8:
			case PixelFormatType_Depth32:
			case PixelFormatType_Stencil1:
			case PixelFormatType_Stencil4:
			case PixelFormatType_Stencil8:
			case PixelFormatType_Stencil16:
				return false;

			case PixelFormatType_Undefined:
				break;
		}

		NazaraError("Invalid pixel format");
		return false;
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

	inline String PixelFormat::ToString(PixelFormatType format)
	{
		switch (format)
		{
			case PixelFormatType_A8:
				return "A8";

			case PixelFormatType_BGR8:
				return "BGR8";

			case PixelFormatType_BGRA8:
				return "BGRA8";

			case PixelFormatType_DXT1:
				return "DXT1";

			case PixelFormatType_DXT3:
				return "DXT3";

			case PixelFormatType_DXT5:
				return "DXT5";

			case PixelFormatType_L8:
				return "L8";

			case PixelFormatType_LA8:
				return "LA8";

			case PixelFormatType_R8:
				return "R8";

			case PixelFormatType_R8I:
				return "R8I";

			case PixelFormatType_R8UI:
				return "R8UI";

			case PixelFormatType_R16:
				return "R16";

			case PixelFormatType_R16F:
				return "R16F";

			case PixelFormatType_R16I:
				return "R16I";

			case PixelFormatType_R16UI:
				return "R16UI";

			case PixelFormatType_R32F:
				return "R32F";

			case PixelFormatType_R32I:
				return "R32I";

			case PixelFormatType_R32UI:
				return "R32UI";

			case PixelFormatType_RG8:
				return "RG8";

			case PixelFormatType_RG8I:
				return "RG8I";

			case PixelFormatType_RG8UI:
				return "RG8UI";

			case PixelFormatType_RG16:
				return "RG16";

			case PixelFormatType_RG16F:
				return "RG16F";

			case PixelFormatType_RG16I:
				return "RG16I";

			case PixelFormatType_RG16UI:
				return "RG16UI";

			case PixelFormatType_RG32F:
				return "RG32F";

			case PixelFormatType_RG32I:
				return "RG32I";

			case PixelFormatType_RG32UI:
				return "RG32UI";

			case PixelFormatType_RGB5A1:
				return "RGB5A1";

			case PixelFormatType_RGB8:
				return "RGB8";

			case PixelFormatType_RGB16F:
				return "RGB16F";

			case PixelFormatType_RGB16I:
				return "RGB16I";

			case PixelFormatType_RGB16UI:
				return "RGB16UI";

			case PixelFormatType_RGB32F:
				return "RGB32F";

			case PixelFormatType_RGB32I:
				return "RGB32I";

			case PixelFormatType_RGB32UI:
				return "RGB32UI";

			case PixelFormatType_RGBA4:
				return "RGBA4";

			case PixelFormatType_RGBA8:
				return "RGBA8";

			case PixelFormatType_RGBA16F:
				return "RGBA16F";

			case PixelFormatType_RGBA16I:
				return "RGBA16I";

			case PixelFormatType_RGBA16UI:
				return "RGBA16UI";

			case PixelFormatType_RGBA32F:
				return "RGBA32F";

			case PixelFormatType_RGBA32I:
				return "RGBA32I";

			case PixelFormatType_RGBA32UI:
				return "RGBA32UI";

			case PixelFormatType_Depth16:
				return "Depth16";

			case PixelFormatType_Depth24:
				return "Depth24";

			case PixelFormatType_Depth24Stencil8:
				return "Depth24Stencil8";

			case PixelFormatType_Depth32:
				return "Depth32";

			case PixelFormatType_Stencil1:
				return "Stencil1";

			case PixelFormatType_Stencil4:
				return "Stencil4";

			case PixelFormatType_Stencil8:
				return "Stencil8";

			case PixelFormatType_Stencil16:
				return "Stencil16";

			case PixelFormatType_Undefined:
				return "Undefined";
		}

		NazaraError("Invalid pixel format");
		return "Invalid format";
	}
}

#include <Nazara/Utility/DebugOff.hpp>
