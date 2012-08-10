// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/PixelFormat.hpp>
#include <Nazara/Core/Endianness.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace
{
	inline nzUInt8 c4to5(nzUInt8 c)
	{
		return c * (31.f/15.f);
	}

	inline nzUInt8 c4to8(nzUInt8 c)
	{
		return c * (255/15);
	}

	inline nzUInt8 c5to4(nzUInt8 c)
	{
		return c * (15.f/31.f);
	}

	inline nzUInt8 c5to8(nzUInt8 c)
	{
		return c * (255.f/31.f);
	}

	inline nzUInt8 c8to4(nzUInt8 c)
	{
		return c * (15.f/255.f);
	}

	inline nzUInt8 c8to5(nzUInt8 c)
	{
		return c * (31.f/255.f);
	}

	template<nzPixelFormat from, nzPixelFormat to>
	nzUInt8* ConvertPixels(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		NazaraUnused(start);
		NazaraUnused(dst);
		NazaraUnused(end);

		NazaraInternalError("Conversion from " + NzPixelFormat::ToString(from) + " to " + NzPixelFormat::ToString(to) + " is not supported");
		return nullptr;
	}

	/**********************************BGR8***********************************/
	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_BGR8, nzPixelFormat_BGRA8>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		while (start < end)
		{
			*dst++ = start[0];
			*dst++ = start[1];
			*dst++ = start[2];
			*dst++ = 0xFF;

			start += 3;
		}

		return dst;
	}

	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_BGR8, nzPixelFormat_L8>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		while (start < end)
		{
			*dst++ = static_cast<nzUInt8>(start[2] * 0.3 + start[1] * 0.59 + start[0] * 0.11);

			start += 3;
		}

		return dst;
	}

	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_BGR8, nzPixelFormat_LA8>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		while (start < end)
		{
			*dst++ = static_cast<nzUInt8>(start[2] * 0.3 + start[1] * 0.59 + start[0] * 0.11);
			*dst++ = 0xFF;

			start += 3;
		}

		return dst;
	}

	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_BGR8, nzPixelFormat_RGBA4>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		nzUInt16* ptr = reinterpret_cast<nzUInt16*>(dst);
		while (start < end)
		{
			*ptr = (static_cast<nzUInt16>(c8to4(start[2])) << 12) |
			       (static_cast<nzUInt16>(c8to4(start[1])) << 8)  |
			       (static_cast<nzUInt16>(c8to4(start[0])) << 4)  |
			       0x0F;

			#ifdef NAZARA_BIG_ENDIAN
			NzByteSwap(ptr, sizeof(nzUInt16));
			#endif

			ptr++;
			start += 3;
		}

		return dst;
	}

	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_BGR8, nzPixelFormat_RGB5A1>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		nzUInt16* ptr = reinterpret_cast<nzUInt16*>(dst);
		while (start < end)
		{
			*ptr = (static_cast<nzUInt16>(c8to5(start[2])) << 11) |
			       (static_cast<nzUInt16>(c8to5(start[1])) << 6)  |
			       (static_cast<nzUInt16>(c8to5(start[0])) << 1)  |
			       0x1;

			#ifdef NAZARA_BIG_ENDIAN
			NzByteSwap(ptr, sizeof(nzUInt16));
			#endif

			ptr++;
			start += 3;
		}

		return reinterpret_cast<nzUInt8*>(ptr);
	}

	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_BGR8, nzPixelFormat_RGB8>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		while (start < end)
		{
			*dst++ = start[2];
			*dst++ = start[1];
			*dst++ = start[0];

			start += 3;
		}

		return dst;
	}

	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_BGR8, nzPixelFormat_RGBA8>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		while (start < end)
		{
			*dst++ = start[2];
			*dst++ = start[1];
			*dst++ = start[0];
			*dst++ = 0xFF;

			start += 3;
		}

		return dst;
	}

	/**********************************BGRA8**********************************/
	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_BGRA8, nzPixelFormat_BGR8>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		while (start < end)
		{
			*dst++ = start[0];
			*dst++ = start[1];
			*dst++ = start[2];

			start += 4;
		}

		return dst;
	}

	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_BGRA8, nzPixelFormat_L8>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		while (start < end)
		{
			*dst++ = static_cast<nzUInt8>(start[2] * 0.3 + start[1] * 0.59 + start[0] * 0.11);

			start += 4;
		}

		return dst;
	}

	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_BGRA8, nzPixelFormat_LA8>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		while (start < end)
		{
			*dst++ = static_cast<nzUInt8>(start[2] * 0.3 + start[1] * 0.59 + start[0] * 0.11);
			*dst++ = start[3];

			start += 4;
		}

		return dst;
	}

	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_BGRA8, nzPixelFormat_RGBA4>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		nzUInt16* ptr = reinterpret_cast<nzUInt16*>(dst);
		while (start < end)
		{
			*ptr = (static_cast<nzUInt16>(c8to4(start[2])) << 12) |
				   (static_cast<nzUInt16>(c8to4(start[1])) << 8)  |
			       (static_cast<nzUInt16>(c8to4(start[0])) << 4)  |
			       (static_cast<nzUInt16>(c8to4(start[3])) << 0);

			#ifdef NAZARA_BIG_ENDIAN
			NzByteSwap(ptr, sizeof(nzUInt16));
			#endif

			ptr++;
			start += 4;
		}

		return dst;
	}

	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_BGRA8, nzPixelFormat_RGB5A1>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		nzUInt16* ptr = reinterpret_cast<nzUInt16*>(dst);
		while (start < end)
		{
			*ptr = (static_cast<nzUInt16>(c8to5(start[2])) << 11) |
				   (static_cast<nzUInt16>(c8to5(start[1])) << 6)  |
			       (static_cast<nzUInt16>(c8to5(start[0])) << 1)  |
			       ((start[3] == 0xFF) ? 1 : 0);

			#ifdef NAZARA_BIG_ENDIAN
			NzByteSwap(ptr, sizeof(nzUInt16));
			#endif

			ptr++;
			start += 4;
		}

		return reinterpret_cast<nzUInt8*>(ptr);
	}

	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_BGRA8, nzPixelFormat_RGB8>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		while (start < end)
		{
			*dst++ = start[2];
			*dst++ = start[1];
			*dst++ = start[0];

			start += 4;
		}

		return dst;
	}

	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_BGRA8, nzPixelFormat_RGBA8>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		while (start < end)
		{
			*dst++ = start[2];
			*dst++ = start[1];
			*dst++ = start[0];
			*dst++ = start[3];

			start += 4;
		}

		return dst;
	}

	/***********************************L8************************************/
	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_L8, nzPixelFormat_BGR8>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		while (start < end)
		{
			*dst++ = start[0];
			*dst++ = start[0];
			*dst++ = start[0];

			start += 1;
		}

		return dst;
	}

	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_L8, nzPixelFormat_BGRA8>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		while (start < end)
		{
			*dst++ = start[0];
			*dst++ = start[0];
			*dst++ = start[0];
			*dst++ = 0xFF;

			start += 1;
		}

		return dst;
	}

	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_L8, nzPixelFormat_LA8>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		while (start < end)
		{
			*dst++ = start[0];
			*dst++ = 0xFF;

			start += 1;
		}

		return dst;
	}

	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_L8, nzPixelFormat_RGBA4>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		nzUInt16* ptr = reinterpret_cast<nzUInt16*>(dst);
		while (start < end)
		{
			nzUInt16 l = static_cast<nzUInt16>(c8to4(start[0]));

			*ptr = (l << 12) |
				   (l << 8)  |
			       (l << 4)  |
			       0x0F;

			#ifdef NAZARA_BIG_ENDIAN
			NzByteSwap(ptr, sizeof(nzUInt16));
			#endif

			ptr++;
			start += 1;
		}

		return dst;
	}

	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_L8, nzPixelFormat_RGB5A1>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		nzUInt16* ptr = reinterpret_cast<nzUInt16*>(dst);
		while (start < end)
		{
			nzUInt16 l = static_cast<nzUInt16>(c8to5(start[0]));

			*ptr = (l << 11) |
				   (l << 6)  |
			       (l << 1)  |
			       1;

			#ifdef NAZARA_BIG_ENDIAN
			NzByteSwap(ptr, sizeof(nzUInt16));
			#endif

			ptr++;
			start += 1;
		}

		return reinterpret_cast<nzUInt8*>(ptr);
	}

	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_L8, nzPixelFormat_RGB8>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		while (start < end)
		{
			*dst++ = start[0];
			*dst++ = start[0];
			*dst++ = start[0];

			start += 1;
		}

		return dst;
	}

	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_L8, nzPixelFormat_RGBA8>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		while (start < end)
		{
			*dst++ = start[0];
			*dst++ = start[0];
			*dst++ = start[0];
			*dst++ = 0xFF;

			start += 1;
		}

		return dst;
	}

	/***********************************LA8***********************************/
	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_LA8, nzPixelFormat_BGR8>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		while (start < end)
		{
			*dst++ = start[0];
			*dst++ = start[0];
			*dst++ = start[0];

			start += 2;
		}

		return dst;
	}

	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_LA8, nzPixelFormat_BGRA8>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		while (start < end)
		{
			*dst++ = start[0];
			*dst++ = start[0];
			*dst++ = start[0];
			*dst++ = start[1];

			start += 2;
		}

		return dst;
	}

	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_LA8, nzPixelFormat_L8>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		while (start < end)
		{
			*dst++ = start[0];

			start += 2;
		}

		return dst;
	}

	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_LA8, nzPixelFormat_RGBA4>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		nzUInt16* ptr = reinterpret_cast<nzUInt16*>(dst);
		while (start < end)
		{
			nzUInt16 l = static_cast<nzUInt16>(c8to4(start[0]));

			*ptr = (l << 12) | (l << 8) | (l << 4) | c8to4(start[1]);

			#ifdef NAZARA_BIG_ENDIAN
			NzByteSwap(ptr, sizeof(nzUInt16));
			#endif

			ptr++;
			start += 2;
		}

		return dst;
	}

	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_LA8, nzPixelFormat_RGB5A1>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		nzUInt16* ptr = reinterpret_cast<nzUInt16*>(dst);
		while (start < end)
		{
			nzUInt16 l = static_cast<nzUInt16>(c8to5(start[0]));

			*ptr = (l << 11) | (l << 6) | (l << 1) | ((start[1] == 0xFF) ? 1 : 0);

			#ifdef NAZARA_BIG_ENDIAN
			NzByteSwap(ptr, sizeof(nzUInt16));
			#endif

			ptr++;
			start += 2;
		}

		return reinterpret_cast<nzUInt8*>(ptr);
	}

	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_LA8, nzPixelFormat_RGB8>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		while (start < end)
		{
			*dst++ = start[0];
			*dst++ = start[0];
			*dst++ = start[0];

			start += 2;
		}

		return dst;
	}

	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_LA8, nzPixelFormat_RGBA8>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		while (start < end)
		{
			*dst++ = start[0];
			*dst++ = start[0];
			*dst++ = start[0];
			*dst++ = start[1];

			start += 2;
		}

		return dst;
	}

	/*********************************RGBA4***********************************/
	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_RGBA4, nzPixelFormat_BGR8>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		while (start < end)
		{
			nzUInt16 pixel = *reinterpret_cast<const nzUInt16*>(start);

			#ifdef NAZARA_BIG_ENDIAN
			NzByteSwap(&pixel, sizeof(nzUInt16));
			#endif

			*dst++ = c4to8((pixel & 0x00F0) >> 4);
			*dst++ = c4to8((pixel & 0x0F00) >> 8);
			*dst++ = c4to8((pixel & 0xF000) >> 12);

			start += 2;
		}

		return dst;
	}

	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_RGBA4, nzPixelFormat_BGRA8>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		while (start < end)
		{
			nzUInt16 pixel = *reinterpret_cast<const nzUInt16*>(start);

			#ifdef NAZARA_BIG_ENDIAN
			NzByteSwap(&pixel, sizeof(nzUInt16));
			#endif

			*dst++ = c4to8((pixel & 0x00F0) >> 4);
			*dst++ = c4to8((pixel & 0x0F00) >> 8);
			*dst++ = c4to8((pixel & 0xF000) >> 12);
			*dst++ = c4to8((pixel & 0x000F) >> 0);

			start += 2;
		}

		return dst;
	}

	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_RGBA4, nzPixelFormat_L8>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		while (start < end)
		{
			nzUInt16 pixel = *reinterpret_cast<const nzUInt16*>(start);

			#ifdef NAZARA_BIG_ENDIAN
			NzByteSwap(&pixel, sizeof(nzUInt16));
			#endif

			nzUInt16 r = c4to8((pixel & 0xF000) >> 12);
			nzUInt16 g = c4to8((pixel & 0x0F00) >> 8);
			nzUInt16 b = c4to8((pixel & 0x00F0) >> 4);

			*dst++ = static_cast<nzUInt8>(r * 0.3 + g * 0.59 + b * 0.11);

			start += 2;
		}

		return dst;
	}

	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_RGBA4, nzPixelFormat_LA8>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		while (start < end)
		{
			nzUInt16 pixel = *reinterpret_cast<const nzUInt16*>(start);

			#ifdef NAZARA_BIG_ENDIAN
			NzByteSwap(&pixel, sizeof(nzUInt16));
			#endif

			nzUInt16 r = c4to8((pixel & 0xF000) >> 12);
			nzUInt16 g = c4to8((pixel & 0x0F00) >> 8);
			nzUInt16 b = c4to8((pixel & 0x00F0) >> 4);

			*dst++ = static_cast<nzUInt8>(r * 0.3 + g * 0.59 + b * 0.11);
			*dst++ = c4to8(pixel & 0x000F);

			start += 2;
		}

		return dst;
	}

	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_RGBA4, nzPixelFormat_RGB5A1>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		nzUInt16* ptr = reinterpret_cast<nzUInt16*>(dst);
		while (start < end)
		{
			nzUInt16 pixel = *reinterpret_cast<const nzUInt16*>(start);

			#ifdef NAZARA_BIG_ENDIAN
			NzByteSwap(&pixel, sizeof(nzUInt16));
			#endif

			nzUInt16 r = c4to5((pixel & 0xF000) >> 12);
			nzUInt16 g = c4to5((pixel & 0x0F00) >> 8);
			nzUInt16 b = c4to5((pixel & 0x00F0) >> 4);
			nzUInt16 a = c4to5((pixel & 0x000F) >> 0);

			*ptr = (r << 11) | (g << 6) | (b << 1) | ((a == 0xFF) ? 1 : 0);

			#ifdef NAZARA_BIG_ENDIAN
			NzByteSwap(ptr, sizeof(nzUInt16));
			#endif

			ptr++;
			start += 2;
		}

		return reinterpret_cast<nzUInt8*>(ptr);
	}

	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_RGBA4, nzPixelFormat_RGB8>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		while (start < end)
		{
			nzUInt16 pixel = *reinterpret_cast<const nzUInt16*>(start);

			#ifdef NAZARA_BIG_ENDIAN
			NzByteSwap(&pixel, sizeof(nzUInt16));
			#endif

			*dst++ = c4to8((pixel & 0xF000) >> 12);
			*dst++ = c4to8((pixel & 0x0F00) >> 8);
			*dst++ = c4to8((pixel & 0x00F0) >> 4);

			start += 2;
		}

		return dst;
	}

	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_RGBA4, nzPixelFormat_RGBA8>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		while (start < end)
		{
			nzUInt16 pixel = *reinterpret_cast<const nzUInt16*>(start);

			#ifdef NAZARA_BIG_ENDIAN
			NzByteSwap(&pixel, sizeof(nzUInt16));
			#endif

			*dst++ = c4to8((pixel & 0xF000) >> 12);
			*dst++ = c4to8((pixel & 0x0F00) >> 8);
			*dst++ = c4to8((pixel & 0x00F0) >> 4);
			*dst++ = c4to8((pixel & 0x000F) >> 0);

			start += 2;
		}

		return dst;
	}

	/*********************************RGB5A1**********************************/
	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_RGB5A1, nzPixelFormat_BGR8>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		while (start < end)
		{
			nzUInt16 pixel = *reinterpret_cast<const nzUInt16*>(start);

			#ifdef NAZARA_BIG_ENDIAN
			NzByteSwap(&pixel, sizeof(nzUInt16));
			#endif

			*dst++ = c5to8((pixel & 0x003E) >> 1);
			*dst++ = c5to8((pixel & 0x07C0) >> 6);
			*dst++ = c5to8((pixel & 0xF800) >> 11);

			start += 2;
		}

		return dst;
	}

	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_RGB5A1, nzPixelFormat_BGRA8>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		while (start < end)
		{
			nzUInt16 pixel = *reinterpret_cast<const nzUInt16*>(start);

			#ifdef NAZARA_BIG_ENDIAN
			NzByteSwap(&pixel, sizeof(nzUInt16));
			#endif

			*dst++ = c5to8((pixel & 0x003E) >> 1);
			*dst++ = c5to8((pixel & 0x07C0) >> 6);
			*dst++ = c5to8((pixel & 0xF800) >> 11);
			*dst++ = static_cast<nzUInt8>((pixel & 0x1)*0xFF);

			start += 2;
		}

		return dst;
	}

	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_RGB5A1, nzPixelFormat_L8>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		while (start < end)
		{
			nzUInt16 pixel = *reinterpret_cast<const nzUInt16*>(start);

			#ifdef NAZARA_BIG_ENDIAN
			NzByteSwap(&pixel, sizeof(nzUInt16));
			#endif

			nzUInt8 r = c5to8((pixel & 0xF800) >> 11);
			nzUInt8 g = c5to8((pixel & 0x07C0) >> 6);
			nzUInt8 b = c5to8((pixel & 0x003E) >> 1);

			*dst++ = static_cast<nzUInt8>(r * 0.3 + g * 0.59 + b * 0.11);

			start += 2;
		}

		return dst;
	}

	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_RGB5A1, nzPixelFormat_LA8>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		while (start < end)
		{
			nzUInt16 pixel = *reinterpret_cast<const nzUInt16*>(start);

			#ifdef NAZARA_BIG_ENDIAN
			NzByteSwap(&pixel, sizeof(nzUInt16));
			#endif

			nzUInt8 r = c5to8((pixel & 0xF800) >> 11);
			nzUInt8 g = c5to8((pixel & 0x07C0) >> 6);
			nzUInt8 b = c5to8((pixel & 0x003E) >> 1);

			*dst++ = static_cast<nzUInt8>(r * 0.3 + g * 0.59 + b * 0.11);
			*dst++ = static_cast<nzUInt8>((pixel & 0x1)*0xFF);

			start += 2;
		}

		return dst;
	}

	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_RGB5A1, nzPixelFormat_RGBA4>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		nzUInt16* ptr = reinterpret_cast<nzUInt16*>(dst);
		while (start < end)
		{
			nzUInt16 pixel = *reinterpret_cast<const nzUInt16*>(start);

			#ifdef NAZARA_BIG_ENDIAN
			NzByteSwap(&pixel, sizeof(nzUInt16));
			#endif

			nzUInt8 r = c5to4((pixel & 0xF800) >> 11);
			nzUInt8 g = c5to4((pixel & 0x07C0) >> 6);
			nzUInt8 b = c5to4((pixel & 0x003E) >> 1);

			*ptr = (r << 12) | (g << 8) | (b << 4) | ((pixel & 0x1)*0x0F);

			#ifdef NAZARA_BIG_ENDIAN
			NzByteSwap(ptr, sizeof(nzUInt16));
			#endif

			ptr++;
			start += 2;
		}

		return dst;
	}

	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_RGB5A1, nzPixelFormat_RGB8>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		while (start < end)
		{
			nzUInt16 pixel = *reinterpret_cast<const nzUInt16*>(start);

			#ifdef NAZARA_BIG_ENDIAN
			NzByteSwap(&pixel, sizeof(nzUInt16));
			#endif

			*dst++ = c5to8((pixel & 0xF800) >> 11);
			*dst++ = c5to8((pixel & 0x07C0) >> 6);
			*dst++ = c5to8((pixel & 0x003E) >> 1);

			start += 2;
		}

		return dst;
	}

	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_RGB5A1, nzPixelFormat_RGBA8>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		while (start < end)
		{
			nzUInt16 pixel = *reinterpret_cast<const nzUInt16*>(start);

			#ifdef NAZARA_BIG_ENDIAN
			NzByteSwap(&pixel, sizeof(nzUInt16));
			#endif

			*dst++ = c5to8((pixel & 0xF800) >> 11);
			*dst++ = c5to8((pixel & 0x07C0) >> 6);
			*dst++ = c5to8((pixel & 0x003E) >> 1);
			*dst++ = static_cast<nzUInt8>((pixel & 0x1)*0xFF);

			start += 2;
		}

		return dst;
	}

	/**********************************RGB8***********************************/
	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_RGB8, nzPixelFormat_BGR8>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		while (start < end)
		{
			*dst++ = start[2];
			*dst++ = start[1];
			*dst++ = start[0];

			start += 3;
		}

		return dst;
	}

	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_RGB8, nzPixelFormat_BGRA8>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		while (start < end)
		{
			*dst++ = start[2];
			*dst++ = start[1];
			*dst++ = start[0];
			*dst++ = 0xFF;

			start += 3;
		}

		return dst;
	}

	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_RGB8, nzPixelFormat_L8>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		while (start < end)
		{
			*dst++ = static_cast<nzUInt8>(start[0] * 0.3 + start[1] * 0.59 + start[2] * 0.11);

			start += 3;
		}

		return dst;
	}

	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_RGB8, nzPixelFormat_LA8>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		while (start < end)
		{
			*dst++ = static_cast<nzUInt8>(start[0] * 0.3 + start[1] * 0.59 + start[2] * 0.11);
			*dst++ = 0xFF;

			start += 3;
		}

		return dst;
	}

	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_RGB8, nzPixelFormat_RGBA4>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		nzUInt16* ptr = reinterpret_cast<nzUInt16*>(dst);
		while (start < end)
		{
			*ptr = (static_cast<nzUInt16>(c8to4(start[0])) << 12) |
				   (static_cast<nzUInt16>(c8to4(start[1])) << 8)  |
			       (static_cast<nzUInt16>(c8to4(start[2])) << 4)  |
			       0x0F;

			#ifdef NAZARA_BIG_ENDIAN
			NzByteSwap(ptr, sizeof(nzUInt16));
			#endif

			ptr++;
			start += 3;
		}

		return dst;
	}

	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_RGB8, nzPixelFormat_RGB5A1>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		nzUInt16* ptr = reinterpret_cast<nzUInt16*>(dst);
		while (start < end)
		{
			*ptr = (static_cast<nzUInt16>(c8to5(start[0])) << 11) |
				   (static_cast<nzUInt16>(c8to5(start[1])) << 6)  |
			       (static_cast<nzUInt16>(c8to5(start[2])) << 1)  |
				   0x1;

			#ifdef NAZARA_BIG_ENDIAN
			NzByteSwap(ptr, sizeof(nzUInt16));
			#endif

			ptr++;
			start += 3;
		}

		return reinterpret_cast<nzUInt8*>(ptr);
	}

	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_RGB8, nzPixelFormat_RGBA8>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		while (start < end)
		{
			*dst++ = start[0];
			*dst++ = start[1];
			*dst++ = start[2];
			*dst++ = 0xFF;

			start += 3;
		}

		return dst;
	}

	/**********************************RGBA8**********************************/
	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_RGBA8, nzPixelFormat_BGR8>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		while (start < end)
		{
			*dst++ = start[2];
			*dst++ = start[1];
			*dst++ = start[0];

			start += 4;
		}

		return dst;
	}

	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_RGBA8, nzPixelFormat_BGRA8>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		while (start < end)
		{
			*dst++ = start[2];
			*dst++ = start[1];
			*dst++ = start[0];
			*dst++ = start[3];

			start += 4;
		}

		return dst;
	}

	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_RGBA8, nzPixelFormat_L8>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		while (start < end)
		{
			*dst++ = static_cast<nzUInt8>(start[0] * 0.3 + start[1] * 0.59 + start[2] * 0.11);

			start += 4;
		}

		return dst;
	}

	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_RGBA8, nzPixelFormat_LA8>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		while (start < end)
		{
			*dst++ = static_cast<nzUInt8>(start[0] * 0.3 + start[1] * 0.59 + start[2] * 0.11);
			*dst++ = start[3];

			start += 4;
		}

		return dst;
	}

	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_RGBA8, nzPixelFormat_RGBA4>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		nzUInt16* ptr = reinterpret_cast<nzUInt16*>(dst);
		while (start < end)
		{
			*ptr = (static_cast<nzUInt16>(c8to4(start[0])) << 12) |
				   (static_cast<nzUInt16>(c8to4(start[1])) << 8)  |
			       (static_cast<nzUInt16>(c8to4(start[2])) << 4)  |
			       (static_cast<nzUInt16>(c8to4(start[3])) << 0);

			#ifdef NAZARA_BIG_ENDIAN
			NzByteSwap(ptr, sizeof(nzUInt16));
			#endif

			ptr++;
			start += 4;
		}

		return dst;
	}

	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_RGBA8, nzPixelFormat_RGB5A1>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		nzUInt16* ptr = reinterpret_cast<nzUInt16*>(dst);
		while (start < end)
		{
			*ptr = (static_cast<nzUInt16>(c8to5(start[0])) << 11) |
			       (static_cast<nzUInt16>(c8to5(start[1])) << 6)  |
			       (static_cast<nzUInt16>(c8to5(start[2])) << 1)  |
				   ((start[3] == 0xFF) ? 1 : 0);

			#ifdef NAZARA_BIG_ENDIAN
			NzByteSwap(ptr, sizeof(nzUInt16));
			#endif

			ptr++;
			start += 4;
		}

		return reinterpret_cast<nzUInt8*>(ptr);
	}

	template<>
	nzUInt8* ConvertPixels<nzPixelFormat_RGBA8, nzPixelFormat_RGB8>(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst)
	{
		while (start < end)
		{
			*dst++ = start[0];
			*dst++ = start[1];
			*dst++ = start[2];

			start += 4;
		}

		return dst;
	}

	template<nzPixelFormat format1, nzPixelFormat format2>
	void RegisterConverter()
	{
		NzPixelFormat::SetConvertFunction(format1, format2, &ConvertPixels<format1, format2>);
	}
}

bool NzPixelFormat::Initialize()
{
	/**********************************BGR8***********************************/
	RegisterConverter<nzPixelFormat_BGR8, nzPixelFormat_BGRA8>();
	RegisterConverter<nzPixelFormat_BGR8, nzPixelFormat_L8>();
	RegisterConverter<nzPixelFormat_BGR8, nzPixelFormat_LA8>();/*
	RegisterConverter<nzPixelFormat_BGR8, nzPixelFormat_RGB16F>();
	RegisterConverter<nzPixelFormat_BGR8, nzPixelFormat_RGB16I>();
	RegisterConverter<nzPixelFormat_BGR8, nzPixelFormat_RGB32F>();
	RegisterConverter<nzPixelFormat_BGR8, nzPixelFormat_RGB32I>();
	RegisterConverter<nzPixelFormat_BGR8, nzPixelFormat_RGBA16F>();
	RegisterConverter<nzPixelFormat_BGR8, nzPixelFormat_RGBA16I>();
	RegisterConverter<nzPixelFormat_BGR8, nzPixelFormat_RGBA32F>();
	RegisterConverter<nzPixelFormat_BGR8, nzPixelFormat_RGBA32I>();*/
	RegisterConverter<nzPixelFormat_BGR8, nzPixelFormat_RGBA4>();
	RegisterConverter<nzPixelFormat_BGR8, nzPixelFormat_RGB5A1>();
	RegisterConverter<nzPixelFormat_BGR8, nzPixelFormat_RGB8>();
	RegisterConverter<nzPixelFormat_BGR8, nzPixelFormat_RGBA8>();

	/**********************************BGRA8**********************************/
	RegisterConverter<nzPixelFormat_BGRA8, nzPixelFormat_BGR8>();
	RegisterConverter<nzPixelFormat_BGRA8, nzPixelFormat_L8>();
	RegisterConverter<nzPixelFormat_BGRA8, nzPixelFormat_LA8>();/*
	RegisterConverter<nzPixelFormat_BGRA8, nzPixelFormat_RGB16F>();
	RegisterConverter<nzPixelFormat_BGRA8, nzPixelFormat_RGB16I>();
	RegisterConverter<nzPixelFormat_BGRA8, nzPixelFormat_RGB32F>();
	RegisterConverter<nzPixelFormat_BGRA8, nzPixelFormat_RGB32I>();
	RegisterConverter<nzPixelFormat_BGRA8, nzPixelFormat_RGBA16F>();
	RegisterConverter<nzPixelFormat_BGRA8, nzPixelFormat_RGBA16I>();
	RegisterConverter<nzPixelFormat_BGRA8, nzPixelFormat_RGBA32F>();
	RegisterConverter<nzPixelFormat_BGRA8, nzPixelFormat_RGBA32I>();*/
	RegisterConverter<nzPixelFormat_BGRA8, nzPixelFormat_RGBA4>();
	RegisterConverter<nzPixelFormat_BGRA8, nzPixelFormat_RGB5A1>();
	RegisterConverter<nzPixelFormat_BGRA8, nzPixelFormat_RGB8>();
	RegisterConverter<nzPixelFormat_BGRA8, nzPixelFormat_RGBA8>();

	/**********************************DXT1***********************************/
	///TODO: Décompresseur DXT1
/*
	RegisterConverter<nzPixelFormat_DXT1, nzPixelFormat_BGR8>();
	RegisterConverter<nzPixelFormat_DXT1, nzPixelFormat_BGRA8>();
	RegisterConverter<nzPixelFormat_DXT1, nzPixelFormat_DXT3>();
	RegisterConverter<nzPixelFormat_DXT1, nzPixelFormat_DXT5>();
	RegisterConverter<nzPixelFormat_DXT1, nzPixelFormat_L8>();
	RegisterConverter<nzPixelFormat_DXT1, nzPixelFormat_LA8>();
	RegisterConverter<nzPixelFormat_DXT1, nzPixelFormat_RGB16F>();
	RegisterConverter<nzPixelFormat_DXT1, nzPixelFormat_RGB16I>();
	RegisterConverter<nzPixelFormat_DXT1, nzPixelFormat_RGB32F>();
	RegisterConverter<nzPixelFormat_DXT1, nzPixelFormat_RGB32I>();
	RegisterConverter<nzPixelFormat_DXT1, nzPixelFormat_RGBA16F>();
	RegisterConverter<nzPixelFormat_DXT1, nzPixelFormat_RGBA16I>();
	RegisterConverter<nzPixelFormat_DXT1, nzPixelFormat_RGBA32F>();
	RegisterConverter<nzPixelFormat_DXT1, nzPixelFormat_RGBA32I>();
	RegisterConverter<nzPixelFormat_DXT1, nzPixelFormat_RGBA4>();
	RegisterConverter<nzPixelFormat_DXT1, nzPixelFormat_RGB5A1>();
	RegisterConverter<nzPixelFormat_DXT1, nzPixelFormat_RGB8>();
	RegisterConverter<nzPixelFormat_DXT1, nzPixelFormat_RGBA8>();
*/

	/**********************************DXT3***********************************/
	///TODO: Décompresseur DXT3
/*
	RegisterConverter<nzPixelFormat_DXT3, nzPixelFormat_BGR8>();
	RegisterConverter<nzPixelFormat_DXT3, nzPixelFormat_BGRA8>();
	RegisterConverter<nzPixelFormat_DXT3, nzPixelFormat_DXT1>();
	RegisterConverter<nzPixelFormat_DXT3, nzPixelFormat_DXT5>();
	RegisterConverter<nzPixelFormat_DXT3, nzPixelFormat_L8>();
	RegisterConverter<nzPixelFormat_DXT3, nzPixelFormat_LA8>();
	RegisterConverter<nzPixelFormat_DXT3, nzPixelFormat_RGB16F>();
	RegisterConverter<nzPixelFormat_DXT3, nzPixelFormat_RGB16I>();
	RegisterConverter<nzPixelFormat_DXT3, nzPixelFormat_RGB32F>();
	RegisterConverter<nzPixelFormat_DXT3, nzPixelFormat_RGB32I>();
	RegisterConverter<nzPixelFormat_DXT3, nzPixelFormat_RGBA16F>();
	RegisterConverter<nzPixelFormat_DXT3, nzPixelFormat_RGBA16I>();
	RegisterConverter<nzPixelFormat_DXT3, nzPixelFormat_RGBA32F>();
	RegisterConverter<nzPixelFormat_DXT3, nzPixelFormat_RGBA32I>();
	RegisterConverter<nzPixelFormat_DXT3, nzPixelFormat_RGBA4>();
	RegisterConverter<nzPixelFormat_DXT3, nzPixelFormat_RGB5A1>();
	RegisterConverter<nzPixelFormat_DXT3, nzPixelFormat_RGB8>();
	RegisterConverter<nzPixelFormat_DXT3, nzPixelFormat_RGBA8>();
*/

	/**********************************DXT5***********************************/
	///TODO: Décompresseur DXT5
/*
	RegisterConverter<nzPixelFormat_DXT5, nzPixelFormat_BGR8>();
	RegisterConverter<nzPixelFormat_DXT5, nzPixelFormat_BGRA8>();
	RegisterConverter<nzPixelFormat_DXT5, nzPixelFormat_DXT1>();
	RegisterConverter<nzPixelFormat_DXT5, nzPixelFormat_DXT3>();
	RegisterConverter<nzPixelFormat_DXT5, nzPixelFormat_L8>();
	RegisterConverter<nzPixelFormat_DXT5, nzPixelFormat_LA8>();
	RegisterConverter<nzPixelFormat_DXT5, nzPixelFormat_RGB16F>();
	RegisterConverter<nzPixelFormat_DXT5, nzPixelFormat_RGB16I>();
	RegisterConverter<nzPixelFormat_DXT5, nzPixelFormat_RGB32F>();
	RegisterConverter<nzPixelFormat_DXT5, nzPixelFormat_RGB32I>();
	RegisterConverter<nzPixelFormat_DXT5, nzPixelFormat_RGBA16F>();
	RegisterConverter<nzPixelFormat_DXT5, nzPixelFormat_RGBA16I>();
	RegisterConverter<nzPixelFormat_DXT5, nzPixelFormat_RGBA32F>();
	RegisterConverter<nzPixelFormat_DXT5, nzPixelFormat_RGBA32I>();
	RegisterConverter<nzPixelFormat_DXT5, nzPixelFormat_RGBA4>();
	RegisterConverter<nzPixelFormat_DXT5, nzPixelFormat_RGB5A1>();
	RegisterConverter<nzPixelFormat_DXT5, nzPixelFormat_RGB8>();
	RegisterConverter<nzPixelFormat_DXT5, nzPixelFormat_RGBA8>();
*/

	/***********************************L8************************************/
	RegisterConverter<nzPixelFormat_L8, nzPixelFormat_BGR8>();
	RegisterConverter<nzPixelFormat_L8, nzPixelFormat_BGRA8>();
	RegisterConverter<nzPixelFormat_L8, nzPixelFormat_LA8>();/*
	RegisterConverter<nzPixelFormat_L8, nzPixelFormat_RGB16F>();
	RegisterConverter<nzPixelFormat_L8, nzPixelFormat_RGB16I>();
	RegisterConverter<nzPixelFormat_L8, nzPixelFormat_RGB32F>();
	RegisterConverter<nzPixelFormat_L8, nzPixelFormat_RGB32I>();
	RegisterConverter<nzPixelFormat_L8, nzPixelFormat_RGBA16F>();
	RegisterConverter<nzPixelFormat_L8, nzPixelFormat_RGBA16I>();
	RegisterConverter<nzPixelFormat_L8, nzPixelFormat_RGBA32F>();
	RegisterConverter<nzPixelFormat_L8, nzPixelFormat_RGBA32I>();*/
	RegisterConverter<nzPixelFormat_L8, nzPixelFormat_RGBA4>();
	RegisterConverter<nzPixelFormat_L8, nzPixelFormat_RGB5A1>();
	RegisterConverter<nzPixelFormat_L8, nzPixelFormat_RGB8>();
	RegisterConverter<nzPixelFormat_L8, nzPixelFormat_RGBA8>();

	/***********************************LA8***********************************/
	RegisterConverter<nzPixelFormat_LA8, nzPixelFormat_BGR8>();
	RegisterConverter<nzPixelFormat_LA8, nzPixelFormat_BGRA8>();
	RegisterConverter<nzPixelFormat_LA8, nzPixelFormat_L8>();/*
	RegisterConverter<nzPixelFormat_LA8, nzPixelFormat_RGB16F>();
	RegisterConverter<nzPixelFormat_LA8, nzPixelFormat_RGB16I>();
	RegisterConverter<nzPixelFormat_LA8, nzPixelFormat_RGB32F>();
	RegisterConverter<nzPixelFormat_LA8, nzPixelFormat_RGB32I>();
	RegisterConverter<nzPixelFormat_LA8, nzPixelFormat_RGBA16F>();
	RegisterConverter<nzPixelFormat_LA8, nzPixelFormat_RGBA16I>();
	RegisterConverter<nzPixelFormat_LA8, nzPixelFormat_RGBA32F>();
	RegisterConverter<nzPixelFormat_LA8, nzPixelFormat_RGBA32I>();*/
	RegisterConverter<nzPixelFormat_LA8, nzPixelFormat_RGBA4>();
	RegisterConverter<nzPixelFormat_LA8, nzPixelFormat_RGB5A1>();
	RegisterConverter<nzPixelFormat_LA8, nzPixelFormat_RGB8>();
	RegisterConverter<nzPixelFormat_LA8, nzPixelFormat_RGBA8>();

	/**********************************RGBA4**********************************/
	RegisterConverter<nzPixelFormat_RGBA4, nzPixelFormat_BGR8>();
	RegisterConverter<nzPixelFormat_RGBA4, nzPixelFormat_BGRA8>();
	RegisterConverter<nzPixelFormat_RGBA4, nzPixelFormat_L8>();
	RegisterConverter<nzPixelFormat_RGBA4, nzPixelFormat_LA8>();/*
	RegisterConverter<nzPixelFormat_RGBA4, nzPixelFormat_RGB16F>();
	RegisterConverter<nzPixelFormat_RGBA4, nzPixelFormat_RGB16I>();
	RegisterConverter<nzPixelFormat_RGBA4, nzPixelFormat_RGB32F>();
	RegisterConverter<nzPixelFormat_RGBA4, nzPixelFormat_RGB32I>();
	RegisterConverter<nzPixelFormat_RGBA4, nzPixelFormat_RGBA16F>();
	RegisterConverter<nzPixelFormat_RGBA4, nzPixelFormat_RGBA16I>();
	RegisterConverter<nzPixelFormat_RGBA4, nzPixelFormat_RGBA32F>();
	RegisterConverter<nzPixelFormat_RGBA4, nzPixelFormat_RGBA32I>();*/
	RegisterConverter<nzPixelFormat_RGBA4, nzPixelFormat_RGB5A1>();
	RegisterConverter<nzPixelFormat_RGBA4, nzPixelFormat_RGB8>();
	RegisterConverter<nzPixelFormat_RGBA4, nzPixelFormat_RGBA8>();

	/*********************************RGB5A1**********************************/
	RegisterConverter<nzPixelFormat_RGB5A1, nzPixelFormat_BGR8>();
	RegisterConverter<nzPixelFormat_RGB5A1, nzPixelFormat_BGRA8>();
	RegisterConverter<nzPixelFormat_RGB5A1, nzPixelFormat_L8>();
	RegisterConverter<nzPixelFormat_RGB5A1, nzPixelFormat_LA8>();/*
	RegisterConverter<nzPixelFormat_RGB5A1, nzPixelFormat_RGB16F>();
	RegisterConverter<nzPixelFormat_RGB5A1, nzPixelFormat_RGB16I>();
	RegisterConverter<nzPixelFormat_RGB5A1, nzPixelFormat_RGB32F>();
	RegisterConverter<nzPixelFormat_RGB5A1, nzPixelFormat_RGB32I>();
	RegisterConverter<nzPixelFormat_RGB5A1, nzPixelFormat_RGBA16F>();
	RegisterConverter<nzPixelFormat_RGB5A1, nzPixelFormat_RGBA16I>();
	RegisterConverter<nzPixelFormat_RGB5A1, nzPixelFormat_RGBA32F>();
	RegisterConverter<nzPixelFormat_RGB5A1, nzPixelFormat_RGBA32I>();*/
	RegisterConverter<nzPixelFormat_RGB5A1, nzPixelFormat_RGBA4>();
	RegisterConverter<nzPixelFormat_RGB5A1, nzPixelFormat_RGB8>();
	RegisterConverter<nzPixelFormat_RGB5A1, nzPixelFormat_RGBA8>();

	/**********************************RGB8***********************************/
	RegisterConverter<nzPixelFormat_RGB8, nzPixelFormat_BGR8>();
	RegisterConverter<nzPixelFormat_RGB8, nzPixelFormat_BGRA8>();
	RegisterConverter<nzPixelFormat_RGB8, nzPixelFormat_L8>();
	RegisterConverter<nzPixelFormat_RGB8, nzPixelFormat_LA8>();/*
	RegisterConverter<nzPixelFormat_RGB8, nzPixelFormat_RGB16F>();
	RegisterConverter<nzPixelFormat_RGB8, nzPixelFormat_RGB16I>();
	RegisterConverter<nzPixelFormat_RGB8, nzPixelFormat_RGB32F>();
	RegisterConverter<nzPixelFormat_RGB8, nzPixelFormat_RGB32I>();
	RegisterConverter<nzPixelFormat_RGB8, nzPixelFormat_RGBA16F>();
	RegisterConverter<nzPixelFormat_RGB8, nzPixelFormat_RGBA16I>();
	RegisterConverter<nzPixelFormat_RGB8, nzPixelFormat_RGBA32F>();
	RegisterConverter<nzPixelFormat_RGB8, nzPixelFormat_RGBA32I>();*/
	RegisterConverter<nzPixelFormat_RGB8, nzPixelFormat_RGBA4>();
	RegisterConverter<nzPixelFormat_RGB8, nzPixelFormat_RGB5A1>();
	RegisterConverter<nzPixelFormat_RGB8, nzPixelFormat_RGBA8>();

	/**********************************RGBA8**********************************/
	RegisterConverter<nzPixelFormat_RGBA8, nzPixelFormat_BGR8>();
	RegisterConverter<nzPixelFormat_RGBA8, nzPixelFormat_BGRA8>();
	RegisterConverter<nzPixelFormat_RGBA8, nzPixelFormat_L8>();
	RegisterConverter<nzPixelFormat_RGBA8, nzPixelFormat_LA8>();/*
	RegisterConverter<nzPixelFormat_RGBA8, nzPixelFormat_RGB16F>();
	RegisterConverter<nzPixelFormat_RGBA8, nzPixelFormat_RGB16I>();
	RegisterConverter<nzPixelFormat_RGBA8, nzPixelFormat_RGB32F>();
	RegisterConverter<nzPixelFormat_RGBA8, nzPixelFormat_RGB32I>();
	RegisterConverter<nzPixelFormat_RGBA8, nzPixelFormat_RGBA16F>();
	RegisterConverter<nzPixelFormat_RGBA8, nzPixelFormat_RGBA16I>();
	RegisterConverter<nzPixelFormat_RGBA8, nzPixelFormat_RGBA32F>();
	RegisterConverter<nzPixelFormat_RGBA8, nzPixelFormat_RGBA32I>();*/
	RegisterConverter<nzPixelFormat_RGBA8, nzPixelFormat_RGBA4>();
	RegisterConverter<nzPixelFormat_RGBA8, nzPixelFormat_RGB5A1>();
	RegisterConverter<nzPixelFormat_RGBA8, nzPixelFormat_RGB8>();

	return true;
}

void NzPixelFormat::Uninitialize()
{
	std::memset(s_convertFunctions, 0, (nzPixelFormat_Max+1)*(nzPixelFormat_Max+1)*sizeof(NzPixelFormat::ConvertFunction));

	for (unsigned int i = 0; i <= nzPixelFlipping_Max; ++i)
		s_flipFunctions[i].clear();
}

NzPixelFormat::ConvertFunction NzPixelFormat::s_convertFunctions[nzPixelFormat_Max+1][nzPixelFormat_Max+1] = {{nullptr}}; ///FIXME: Fonctionne correctement ?
std::map<nzPixelFormat, NzPixelFormat::FlipFunction> NzPixelFormat::s_flipFunctions[nzPixelFlipping_Max+1];
