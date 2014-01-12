// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <cstring>
#include <Nazara/Utility/Debug.hpp>

inline bool NzPixelFormat::Convert(nzPixelFormat srcFormat, nzPixelFormat dstFormat, const void* src, void* dst)
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

	if (!func(reinterpret_cast<const nzUInt8*>(src), reinterpret_cast<const nzUInt8*>(src) + GetBytesPerPixel(srcFormat), reinterpret_cast<nzUInt8*>(dst)))
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

		nzUInt8 bpp = GetBytesPerPixel(format);
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

			case nzPixelFlipping_Vertically:
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
		}
	}

	return true;
}

inline nzUInt8 NzPixelFormat::GetBitsPerPixel(nzPixelFormat format)
{
	switch (format)
	{
		case nzPixelFormat_BGR8:
			return 24;

		case nzPixelFormat_BGRA8:
			return 32;

		case nzPixelFormat_DXT1:
			return 8;

		case nzPixelFormat_DXT3:
			return 16;

		case nzPixelFormat_DXT5:
			return 16;

		case nzPixelFormat_L8:
			return 8;

		case nzPixelFormat_LA8:
			return 16;

		case nzPixelFormat_R8:
		case nzPixelFormat_R8I:
		case nzPixelFormat_R8UI:
			return 8;

		case nzPixelFormat_R16:
		case nzPixelFormat_R16F:
		case nzPixelFormat_R16I:
		case nzPixelFormat_R16UI:
			return 16;

		case nzPixelFormat_R32F:
		case nzPixelFormat_R32I:
		case nzPixelFormat_R32UI:
			return 32;

		case nzPixelFormat_RG8:
		case nzPixelFormat_RG8I:
		case nzPixelFormat_RG8UI:
			return 16;

		case nzPixelFormat_RG16:
		case nzPixelFormat_RG16F:
		case nzPixelFormat_RG16I:
		case nzPixelFormat_RG16UI:
			return 32;

		case nzPixelFormat_RG32F:
		case nzPixelFormat_RG32I:
		case nzPixelFormat_RG32UI:
			return 64;

		case nzPixelFormat_RGB16F:
		case nzPixelFormat_RGB16I:
		case nzPixelFormat_RGB16UI:
			return 48;

		case nzPixelFormat_RGB32F:
		case nzPixelFormat_RGB32I:
		case nzPixelFormat_RGB32UI:
			return 96;

		case nzPixelFormat_RGBA16F:
		case nzPixelFormat_RGBA16I:
		case nzPixelFormat_RGBA16UI:
			return 64;

		case nzPixelFormat_RGBA32F:
		case nzPixelFormat_RGBA32I:
		case nzPixelFormat_RGBA32UI:
			return 128;

		case nzPixelFormat_RGBA4:
			return 16;

		case nzPixelFormat_RGB5A1:
			return 16;

		case nzPixelFormat_RGB8:
			return 24;

		case nzPixelFormat_RGBA8:
			return 32;

		case nzPixelFormat_Depth16:
			return 16;

		case nzPixelFormat_Depth24:
			return 24;

		case nzPixelFormat_Depth24Stencil8:
			return 32;

		case nzPixelFormat_Depth32:
			return 32;

		case nzPixelFormat_Stencil1:
			return 1;

		case nzPixelFormat_Stencil4:
			return 2;

		case nzPixelFormat_Stencil8:
			return 8;

		case nzPixelFormat_Stencil16:
			return 16;

		case nzPixelFormat_Undefined:
			break;
	}

	NazaraError("Invalid pixel format");
	return 0;
}

inline nzUInt8 NzPixelFormat::GetBytesPerPixel(nzPixelFormat format)
{
	nzUInt8 bytesPerPixel = GetBitsPerPixel(format)/8;

	#if NAZARA_UTILITY_SAFE
	if (bytesPerPixel == 0)
		NazaraWarning("This format is either invalid or using less than one byte per pixel");
	#endif

	return bytesPerPixel;
}

inline nzPixelFormatType NzPixelFormat::GetType(nzPixelFormat format)
{
	switch (format)
	{
		case nzPixelFormat_BGR8:
		case nzPixelFormat_BGRA8:
		case nzPixelFormat_DXT1:
		case nzPixelFormat_DXT3:
		case nzPixelFormat_DXT5:
		case nzPixelFormat_L8:
		case nzPixelFormat_LA8:
		case nzPixelFormat_R8:
		case nzPixelFormat_R8I:
		case nzPixelFormat_R8UI:
		case nzPixelFormat_R16:
		case nzPixelFormat_R16F:
		case nzPixelFormat_R16I:
		case nzPixelFormat_R16UI:
		case nzPixelFormat_R32F:
		case nzPixelFormat_R32I:
		case nzPixelFormat_R32UI:
		case nzPixelFormat_RG8:
		case nzPixelFormat_RG8I:
		case nzPixelFormat_RG8UI:
		case nzPixelFormat_RG16:
		case nzPixelFormat_RG16F:
		case nzPixelFormat_RG16I:
		case nzPixelFormat_RG16UI:
		case nzPixelFormat_RG32F:
		case nzPixelFormat_RG32I:
		case nzPixelFormat_RG32UI:
		case nzPixelFormat_RGB5A1:
		case nzPixelFormat_RGB8:
		case nzPixelFormat_RGB16F:
		case nzPixelFormat_RGB16I:
		case nzPixelFormat_RGB16UI:
		case nzPixelFormat_RGB32F:
		case nzPixelFormat_RGB32I:
		case nzPixelFormat_RGB32UI:
		case nzPixelFormat_RGBA4:
		case nzPixelFormat_RGBA8:
		case nzPixelFormat_RGBA16F:
		case nzPixelFormat_RGBA16I:
		case nzPixelFormat_RGBA16UI:
		case nzPixelFormat_RGBA32F:
		case nzPixelFormat_RGBA32I:
		case nzPixelFormat_RGBA32UI:
			return nzPixelFormatType_Color;

		case nzPixelFormat_Depth16:
		case nzPixelFormat_Depth24:
		case nzPixelFormat_Depth32:
			return nzPixelFormatType_Depth;

		case nzPixelFormat_Depth24Stencil8:
			return nzPixelFormatType_DepthStencil;

		case nzPixelFormat_Stencil1:
		case nzPixelFormat_Stencil4:
		case nzPixelFormat_Stencil8:
		case nzPixelFormat_Stencil16:
			return nzPixelFormatType_Stencil;

		case nzPixelFormat_Undefined:
			break;
	}

	NazaraError("Invalid pixel format");
	return nzPixelFormatType_Undefined;
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
		case nzPixelFormat_RGBA16F:
		case nzPixelFormat_RGBA16I:
		case nzPixelFormat_RGBA16UI:
		case nzPixelFormat_RGBA32F:
		case nzPixelFormat_RGBA32I:
		case nzPixelFormat_RGBA32UI:
		case nzPixelFormat_RGBA4:
		case nzPixelFormat_RGBA8:
			return true;

		case nzPixelFormat_BGR8:
		case nzPixelFormat_DXT1:
		case nzPixelFormat_L8:
		case nzPixelFormat_R8:
		case nzPixelFormat_R8I:
		case nzPixelFormat_R8UI:
		case nzPixelFormat_R16:
		case nzPixelFormat_R16F:
		case nzPixelFormat_R16I:
		case nzPixelFormat_R16UI:
		case nzPixelFormat_R32F:
		case nzPixelFormat_R32I:
		case nzPixelFormat_R32UI:
		case nzPixelFormat_RG8:
		case nzPixelFormat_RG8I:
		case nzPixelFormat_RG8UI:
		case nzPixelFormat_RG16:
		case nzPixelFormat_RG16F:
		case nzPixelFormat_RG16I:
		case nzPixelFormat_RG16UI:
		case nzPixelFormat_RG32F:
		case nzPixelFormat_RG32I:
		case nzPixelFormat_RG32UI:
		case nzPixelFormat_RGB8:
		case nzPixelFormat_RGB16F:
		case nzPixelFormat_RGB16I:
		case nzPixelFormat_RGB16UI:
		case nzPixelFormat_RGB32F:
		case nzPixelFormat_RGB32I:
		case nzPixelFormat_RGB32UI:
		case nzPixelFormat_Depth16:
		case nzPixelFormat_Depth24:
		case nzPixelFormat_Depth24Stencil8:
		case nzPixelFormat_Depth32:
		case nzPixelFormat_Stencil1:
		case nzPixelFormat_Stencil4:
		case nzPixelFormat_Stencil8:
		case nzPixelFormat_Stencil16:
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

		case nzPixelFormat_BGR8:
		case nzPixelFormat_BGRA8:
		case nzPixelFormat_L8:
		case nzPixelFormat_LA8:
		case nzPixelFormat_R8:
		case nzPixelFormat_R8I:
		case nzPixelFormat_R8UI:
		case nzPixelFormat_R16:
		case nzPixelFormat_R16F:
		case nzPixelFormat_R16I:
		case nzPixelFormat_R16UI:
		case nzPixelFormat_R32F:
		case nzPixelFormat_R32I:
		case nzPixelFormat_R32UI:
		case nzPixelFormat_RG8:
		case nzPixelFormat_RG8I:
		case nzPixelFormat_RG8UI:
		case nzPixelFormat_RG16:
		case nzPixelFormat_RG16F:
		case nzPixelFormat_RG16I:
		case nzPixelFormat_RG16UI:
		case nzPixelFormat_RG32F:
		case nzPixelFormat_RG32I:
		case nzPixelFormat_RG32UI:
		case nzPixelFormat_RGB5A1:
		case nzPixelFormat_RGB8:
		case nzPixelFormat_RGB16F:
		case nzPixelFormat_RGB16I:
		case nzPixelFormat_RGB16UI:
		case nzPixelFormat_RGB32F:
		case nzPixelFormat_RGB32I:
		case nzPixelFormat_RGB32UI:
		case nzPixelFormat_RGBA4:
		case nzPixelFormat_RGBA8:
		case nzPixelFormat_RGBA16F:
		case nzPixelFormat_RGBA16I:
		case nzPixelFormat_RGBA16UI:
		case nzPixelFormat_RGBA32F:
		case nzPixelFormat_RGBA32I:
		case nzPixelFormat_RGBA32UI:
		case nzPixelFormat_Depth16:
		case nzPixelFormat_Depth24:
		case nzPixelFormat_Depth24Stencil8:
		case nzPixelFormat_Depth32:
		case nzPixelFormat_Stencil1:
		case nzPixelFormat_Stencil4:
		case nzPixelFormat_Stencil8:
		case nzPixelFormat_Stencil16:
			return false;

		case nzPixelFormat_Undefined:
			break;
	}

	NazaraError("Invalid pixel format");
	return false;
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

		case nzPixelFormat_R8:
			return "R8";

		case nzPixelFormat_R8I:
			return "R8I";

		case nzPixelFormat_R8UI:
			return "R8UI";

		case nzPixelFormat_R16:
			return "R16";

		case nzPixelFormat_R16F:
			return "R16F";

		case nzPixelFormat_R16I:
			return "R16I";

		case nzPixelFormat_R16UI:
			return "R16UI";

		case nzPixelFormat_R32F:
			return "R32F";

		case nzPixelFormat_R32I:
			return "R32I";

		case nzPixelFormat_R32UI:
			return "R32UI";

		case nzPixelFormat_RG8:
			return "RG8";

		case nzPixelFormat_RG8I:
			return "RG8I";

		case nzPixelFormat_RG8UI:
			return "RG8UI";

		case nzPixelFormat_RG16:
			return "RG16";

		case nzPixelFormat_RG16F:
			return "RG16F";

		case nzPixelFormat_RG16I:
			return "RG16I";

		case nzPixelFormat_RG16UI:
			return "RG16UI";

		case nzPixelFormat_RG32F:
			return "RG32F";

		case nzPixelFormat_RG32I:
			return "RG32I";

		case nzPixelFormat_RG32UI:
			return "RG32UI";

		case nzPixelFormat_RGB5A1:
			return "RGB5A1";

		case nzPixelFormat_RGB8:
			return "RGB8";

		case nzPixelFormat_RGB16F:
			return "RGB16F";

		case nzPixelFormat_RGB16I:
			return "RGB16I";

		case nzPixelFormat_RGB16UI:
			return "RGB16UI";

		case nzPixelFormat_RGB32F:
			return "RGB32F";

		case nzPixelFormat_RGB32I:
			return "RGB32I";

		case nzPixelFormat_RGB32UI:
			return "RGB32UI";

		case nzPixelFormat_RGBA4:
			return "RGBA4";

		case nzPixelFormat_RGBA8:
			return "RGBA8";

		case nzPixelFormat_RGBA16F:
			return "RGBA16F";

		case nzPixelFormat_RGBA16I:
			return "RGBA16I";

		case nzPixelFormat_RGBA16UI:
			return "RGBA16UI";

		case nzPixelFormat_RGBA32F:
			return "RGBA32F";

		case nzPixelFormat_RGBA32I:
			return "RGBA32I";

		case nzPixelFormat_RGBA32UI:
			return "RGBA32UI";

		case nzPixelFormat_Depth16:
			return "Depth16";

		case nzPixelFormat_Depth24:
			return "Depth24";

		case nzPixelFormat_Depth24Stencil8:
			return "Depth24Stencil8";

		case nzPixelFormat_Depth32:
			return "Depth32";

		case nzPixelFormat_Stencil1:
			return "Stencil1";

		case nzPixelFormat_Stencil4:
			return "Stencil4";

		case nzPixelFormat_Stencil8:
			return "Stencil8";

		case nzPixelFormat_Stencil16:
			return "Stencil16";

		case nzPixelFormat_Undefined:
			return "Undefined";
	}

	NazaraError("Invalid pixel format");
	return "Invalid format";
}

#include <Nazara/Utility/DebugOff.hpp>
