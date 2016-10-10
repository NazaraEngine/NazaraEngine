// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/PixelFormat.hpp>
#include <Nazara/Core/Endianness.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	namespace
	{
		inline UInt8 c4to5(UInt8 c)
		{
			return static_cast<UInt8>(c * (31.f/15.f));
		}

		inline UInt8 c4to8(UInt8 c)
		{
			return c << 4;
		}

		inline UInt8 c5to4(UInt8 c)
		{
			return static_cast<UInt8>(c * (15.f/31.f));
		}

		inline UInt8 c5to8(UInt8 c)
		{
			return static_cast<UInt8>(c * (255.f/31.f));
		}

		inline UInt8 c8to4(UInt8 c)
		{
			return c >> 4;
		}

		inline UInt8 c8to5(UInt8 c)
		{
			return static_cast<UInt8>(c * (31.f/255.f));
		}

		template<PixelFormatType from, PixelFormatType to>
		UInt8* ConvertPixels(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			NazaraUnused(start);
			NazaraUnused(dst);
			NazaraUnused(end);

			NazaraInternalError("Conversion from " + PixelFormat::GetName(from) + " to " + PixelFormat::GetName(to) + " is not supported");
			return nullptr;
		}

		/**********************************A8***********************************/
		template<>
		UInt8* ConvertPixels<PixelFormatType_A8, PixelFormatType_BGRA8>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			while (start < end)
			{
				*dst++ = 0xFF;
				*dst++ = 0xFF;
				*dst++ = 0xFF;
				*dst++ = *start;

				start += 1;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormatType_A8, PixelFormatType_LA8>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			while (start < end)
			{
				*dst++ = 0xFF;
				*dst++ = *start;

				start += 1;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormatType_A8, PixelFormatType_RGB5A1>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			UInt16* ptr = reinterpret_cast<UInt16*>(dst);
			while (start < end)
			{
				*ptr = (static_cast<UInt16>(0x1F) << 11) |
					   (static_cast<UInt16>(0x1F) << 6)  |
					   (static_cast<UInt16>(0x1F) << 1)  |
					   ((*start > 0xF) ? 1 : 0); // > 128

				#ifdef NAZARA_BIG_ENDIAN
				SwapBytes(ptr, sizeof(UInt16));
				#endif

				ptr++;
				start += 1;
			}

			return reinterpret_cast<UInt8*>(ptr);
		}

		template<>
		UInt8* ConvertPixels<PixelFormatType_A8, PixelFormatType_RGBA4>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			UInt16* ptr = reinterpret_cast<UInt16*>(dst);
			while (start < end)
			{
				*ptr = 0xFFF0 | c8to4(*start);

				#ifdef NAZARA_BIG_ENDIAN
				SwapBytes(ptr, sizeof(UInt16));
				#endif

				ptr++;
				start += 1;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormatType_A8, PixelFormatType_RGBA8>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			while (start < end)
			{
				*dst++ = 0xFF;
				*dst++ = 0xFF;
				*dst++ = 0xFF;
				*dst++ = *start;

				start += 1;
			}

			return dst;
		}

		/**********************************BGR8***********************************/
		template<>
		UInt8* ConvertPixels<PixelFormatType_BGR8, PixelFormatType_BGRA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormatType_BGR8, PixelFormatType_L8>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			while (start < end)
			{
				*dst++ = static_cast<UInt8>(start[2] * 0.3f + start[1] * 0.59f + start[0] * 0.11f);

				start += 3;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormatType_BGR8, PixelFormatType_LA8>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			while (start < end)
			{
				*dst++ = static_cast<UInt8>(start[2] * 0.3f + start[1] * 0.59f + start[0] * 0.11f);
				*dst++ = 0xFF;

				start += 3;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormatType_BGR8, PixelFormatType_RGB5A1>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			UInt16* ptr = reinterpret_cast<UInt16*>(dst);
			while (start < end)
			{
				*ptr = (static_cast<UInt16>(c8to5(start[2])) << 11) |
					   (static_cast<UInt16>(c8to5(start[1])) << 6)  |
					   (static_cast<UInt16>(c8to5(start[0])) << 1)  |
					   0x1;

				#ifdef NAZARA_BIG_ENDIAN
				SwapBytes(ptr, sizeof(UInt16));
				#endif

				ptr++;
				start += 3;
			}

			return reinterpret_cast<UInt8*>(ptr);
		}

		template<>
		UInt8* ConvertPixels<PixelFormatType_BGR8, PixelFormatType_RGB8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormatType_BGR8, PixelFormatType_RGBA4>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			UInt16* ptr = reinterpret_cast<UInt16*>(dst);
			while (start < end)
			{
				*ptr = (static_cast<UInt16>(c8to4(start[2])) << 12) |
					   (static_cast<UInt16>(c8to4(start[1])) << 8)  |
					   (static_cast<UInt16>(c8to4(start[0])) << 4)  |
					   0x0F;

				#ifdef NAZARA_BIG_ENDIAN
				SwapBytes(ptr, sizeof(UInt16));
				#endif

				ptr++;
				start += 3;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormatType_BGR8, PixelFormatType_RGBA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormatType_BGRA8, PixelFormatType_A8>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			while (start < end)
			{
				*dst++ = start[3];

				start += 4;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormatType_BGRA8, PixelFormatType_BGR8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormatType_BGRA8, PixelFormatType_L8>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			while (start < end)
			{
				*dst++ = static_cast<UInt8>(start[2] * 0.3f + start[1] * 0.59f + start[0] * 0.11f);

				start += 4;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormatType_BGRA8, PixelFormatType_LA8>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			while (start < end)
			{
				*dst++ = static_cast<UInt8>(start[2] * 0.3f + start[1] * 0.59f + start[0] * 0.11f);
				*dst++ = start[3];

				start += 4;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormatType_BGRA8, PixelFormatType_RGBA4>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			UInt16* ptr = reinterpret_cast<UInt16*>(dst);
			while (start < end)
			{
				*ptr = (static_cast<UInt16>(c8to4(start[2])) << 12) |
					   (static_cast<UInt16>(c8to4(start[1])) << 8)  |
					   (static_cast<UInt16>(c8to4(start[0])) << 4)  |
					   (static_cast<UInt16>(c8to4(start[3])) << 0);

				#ifdef NAZARA_BIG_ENDIAN
				SwapBytes(ptr, sizeof(UInt16));
				#endif

				ptr++;
				start += 4;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormatType_BGRA8, PixelFormatType_RGB5A1>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			UInt16* ptr = reinterpret_cast<UInt16*>(dst);
			while (start < end)
			{
				*ptr = (static_cast<UInt16>(c8to5(start[2])) << 11) |
					   (static_cast<UInt16>(c8to5(start[1])) << 6)  |
					   (static_cast<UInt16>(c8to5(start[0])) << 1)  |
					   ((start[3] > 0xF) ? 1 : 0); // > 128

				#ifdef NAZARA_BIG_ENDIAN
				SwapBytes(ptr, sizeof(UInt16));
				#endif

				ptr++;
				start += 4;
			}

			return reinterpret_cast<UInt8*>(ptr);
		}

		template<>
		UInt8* ConvertPixels<PixelFormatType_BGRA8, PixelFormatType_RGB8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormatType_BGRA8, PixelFormatType_RGBA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormatType_L8, PixelFormatType_BGR8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormatType_L8, PixelFormatType_BGRA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormatType_L8, PixelFormatType_LA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormatType_L8, PixelFormatType_RGB5A1>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			UInt16* ptr = reinterpret_cast<UInt16*>(dst);
			while (start < end)
			{
				UInt16 l = static_cast<UInt16>(c8to5(start[0]));

				*ptr = (l << 11) |
					   (l << 6)  |
					   (l << 1)  |
					   1;

				#ifdef NAZARA_BIG_ENDIAN
				SwapBytes(ptr, sizeof(UInt16));
				#endif

				ptr++;
				start += 1;
			}

			return reinterpret_cast<UInt8*>(ptr);
		}

		template<>
		UInt8* ConvertPixels<PixelFormatType_L8, PixelFormatType_RGB8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormatType_L8, PixelFormatType_RGBA4>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			UInt16* ptr = reinterpret_cast<UInt16*>(dst);
			while (start < end)
			{
				UInt16 l = static_cast<UInt16>(c8to4(start[0]));

				*ptr = (l << 12) |
					   (l << 8)  |
					   (l << 4)  |
					   0x0F;

				#ifdef NAZARA_BIG_ENDIAN
				SwapBytes(ptr, sizeof(UInt16));
				#endif

				ptr++;
				start += 1;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormatType_L8, PixelFormatType_RGBA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormatType_LA8, PixelFormatType_A8>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			while (start < end)
			{
				*dst++ = start[1];

				start += 2;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormatType_LA8, PixelFormatType_BGR8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormatType_LA8, PixelFormatType_BGRA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormatType_LA8, PixelFormatType_L8>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			while (start < end)
			{
				*dst++ = start[0];

				start += 2;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormatType_LA8, PixelFormatType_RGB5A1>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			UInt16* ptr = reinterpret_cast<UInt16*>(dst);
			while (start < end)
			{
				UInt16 l = static_cast<UInt16>(c8to5(start[0]));

				*ptr = (l << 11) | (l << 6) | (l << 1) | ((start[1] > 0xF) ? 1 : 0);

				#ifdef NAZARA_BIG_ENDIAN
				SwapBytes(ptr, sizeof(UInt16));
				#endif

				ptr++;
				start += 2;
			}

			return reinterpret_cast<UInt8*>(ptr);
		}

		template<>
		UInt8* ConvertPixels<PixelFormatType_LA8, PixelFormatType_RGB8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormatType_LA8, PixelFormatType_RGBA4>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			UInt16* ptr = reinterpret_cast<UInt16*>(dst);
			while (start < end)
			{
				UInt16 l = static_cast<UInt16>(c8to4(start[0]));

				*ptr = (l << 12) | (l << 8) | (l << 4) | c8to4(start[1]);

				#ifdef NAZARA_BIG_ENDIAN
				SwapBytes(ptr, sizeof(UInt16));
				#endif

				ptr++;
				start += 2;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormatType_LA8, PixelFormatType_RGBA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormatType_RGBA4, PixelFormatType_A8>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			while (start < end)
			{
				UInt16 pixel = *reinterpret_cast<const UInt16*>(start);

				#ifdef NAZARA_BIG_ENDIAN
				SwapBytes(&pixel, sizeof(UInt16));
				#endif

				*dst++ = c4to8(pixel & 0x000F);

				start += 2;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormatType_RGBA4, PixelFormatType_BGR8>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			while (start < end)
			{
				UInt16 pixel = *reinterpret_cast<const UInt16*>(start);

				#ifdef NAZARA_BIG_ENDIAN
				SwapBytes(&pixel, sizeof(UInt16));
				#endif

				*dst++ = c4to8((pixel & 0x00F0) >> 4);
				*dst++ = c4to8((pixel & 0x0F00) >> 8);
				*dst++ = c4to8((pixel & 0xF000) >> 12);

				start += 2;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormatType_RGBA4, PixelFormatType_BGRA8>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			while (start < end)
			{
				UInt16 pixel = *reinterpret_cast<const UInt16*>(start);

				#ifdef NAZARA_BIG_ENDIAN
				SwapBytes(&pixel, sizeof(UInt16));
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
		UInt8* ConvertPixels<PixelFormatType_RGBA4, PixelFormatType_L8>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			while (start < end)
			{
				UInt16 pixel = *reinterpret_cast<const UInt16*>(start);

				#ifdef NAZARA_BIG_ENDIAN
				SwapBytes(&pixel, sizeof(UInt16));
				#endif

				UInt16 r = c4to8((pixel & 0xF000) >> 12);
				UInt16 g = c4to8((pixel & 0x0F00) >> 8);
				UInt16 b = c4to8((pixel & 0x00F0) >> 4);

				*dst++ = static_cast<UInt8>(r * 0.3f + g * 0.59f + b * 0.11f);

				start += 2;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormatType_RGBA4, PixelFormatType_LA8>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			while (start < end)
			{
				UInt16 pixel = *reinterpret_cast<const UInt16*>(start);

				#ifdef NAZARA_BIG_ENDIAN
				SwapBytes(&pixel, sizeof(UInt16));
				#endif

				UInt16 r = c4to8((pixel & 0xF000) >> 12);
				UInt16 g = c4to8((pixel & 0x0F00) >> 8);
				UInt16 b = c4to8((pixel & 0x00F0) >> 4);

				*dst++ = static_cast<UInt8>(r * 0.3f + g * 0.59f + b * 0.11f);
				*dst++ = c4to8(pixel & 0x000F);

				start += 2;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormatType_RGBA4, PixelFormatType_RGB5A1>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			UInt16* ptr = reinterpret_cast<UInt16*>(dst);
			while (start < end)
			{
				UInt16 pixel = *reinterpret_cast<const UInt16*>(start);

				#ifdef NAZARA_BIG_ENDIAN
				SwapBytes(&pixel, sizeof(UInt16));
				#endif

				UInt16 r = c4to5((pixel & 0xF000) >> 12);
				UInt16 g = c4to5((pixel & 0x0F00) >> 8);
				UInt16 b = c4to5((pixel & 0x00F0) >> 4);
				UInt16 a = c4to5((pixel & 0x000F) >> 0);

				*ptr = (r << 11) | (g << 6) | (b << 1) | ((a > 0x3) ? 1 : 0);

				#ifdef NAZARA_BIG_ENDIAN
				SwapBytes(ptr, sizeof(UInt16));
				#endif

				ptr++;
				start += 2;
			}

			return reinterpret_cast<UInt8*>(ptr);
		}

		template<>
		UInt8* ConvertPixels<PixelFormatType_RGBA4, PixelFormatType_RGB8>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			while (start < end)
			{
				UInt16 pixel = *reinterpret_cast<const UInt16*>(start);

				#ifdef NAZARA_BIG_ENDIAN
				SwapBytes(&pixel, sizeof(UInt16));
				#endif

				*dst++ = c4to8((pixel & 0xF000) >> 12);
				*dst++ = c4to8((pixel & 0x0F00) >> 8);
				*dst++ = c4to8((pixel & 0x00F0) >> 4);

				start += 2;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormatType_RGBA4, PixelFormatType_RGBA8>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			while (start < end)
			{
				UInt16 pixel = *reinterpret_cast<const UInt16*>(start);

				#ifdef NAZARA_BIG_ENDIAN
				SwapBytes(&pixel, sizeof(UInt16));
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
		UInt8* ConvertPixels<PixelFormatType_RGB5A1, PixelFormatType_A8>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			while (start < end)
			{
				UInt16 pixel = *reinterpret_cast<const UInt16*>(start);

				#ifdef NAZARA_BIG_ENDIAN
				SwapBytes(&pixel, sizeof(UInt16));
				#endif

				*dst++ = static_cast<UInt8>((pixel & 0x1)*0xFF);

				start += 2;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormatType_RGB5A1, PixelFormatType_BGR8>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			while (start < end)
			{
				UInt16 pixel = *reinterpret_cast<const UInt16*>(start);

				#ifdef NAZARA_BIG_ENDIAN
				SwapBytes(&pixel, sizeof(UInt16));
				#endif

				*dst++ = c5to8((pixel & 0x003E) >> 1);
				*dst++ = c5to8((pixel & 0x07C0) >> 6);
				*dst++ = c5to8((pixel & 0xF800) >> 11);

				start += 2;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormatType_RGB5A1, PixelFormatType_BGRA8>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			while (start < end)
			{
				UInt16 pixel = *reinterpret_cast<const UInt16*>(start);

				#ifdef NAZARA_BIG_ENDIAN
				SwapBytes(&pixel, sizeof(UInt16));
				#endif

				*dst++ = c5to8((pixel & 0x003E) >> 1);
				*dst++ = c5to8((pixel & 0x07C0) >> 6);
				*dst++ = c5to8((pixel & 0xF800) >> 11);
				*dst++ = static_cast<UInt8>((pixel & 0x1)*0xFF);

				start += 2;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormatType_RGB5A1, PixelFormatType_L8>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			while (start < end)
			{
				UInt16 pixel = *reinterpret_cast<const UInt16*>(start);

				#ifdef NAZARA_BIG_ENDIAN
				SwapBytes(&pixel, sizeof(UInt16));
				#endif

				UInt8 r = c5to8((pixel & 0xF800) >> 11);
				UInt8 g = c5to8((pixel & 0x07C0) >> 6);
				UInt8 b = c5to8((pixel & 0x003E) >> 1);

				*dst++ = static_cast<UInt8>(r * 0.3f + g * 0.59f + b * 0.11f);

				start += 2;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormatType_RGB5A1, PixelFormatType_LA8>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			while (start < end)
			{
				UInt16 pixel = *reinterpret_cast<const UInt16*>(start);

				#ifdef NAZARA_BIG_ENDIAN
				SwapBytes(&pixel, sizeof(UInt16));
				#endif

				UInt8 r = c5to8((pixel & 0xF800) >> 11);
				UInt8 g = c5to8((pixel & 0x07C0) >> 6);
				UInt8 b = c5to8((pixel & 0x003E) >> 1);

				*dst++ = static_cast<UInt8>(r * 0.3f + g * 0.59f + b * 0.11f);
				*dst++ = static_cast<UInt8>((pixel & 0x1)*0xFF);

				start += 2;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormatType_RGB5A1, PixelFormatType_RGB8>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			while (start < end)
			{
				UInt16 pixel = *reinterpret_cast<const UInt16*>(start);

				#ifdef NAZARA_BIG_ENDIAN
				SwapBytes(&pixel, sizeof(UInt16));
				#endif

				*dst++ = c5to8((pixel & 0xF800) >> 11);
				*dst++ = c5to8((pixel & 0x07C0) >> 6);
				*dst++ = c5to8((pixel & 0x003E) >> 1);

				start += 2;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormatType_RGB5A1, PixelFormatType_RGBA4>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			UInt16* ptr = reinterpret_cast<UInt16*>(dst);
			while (start < end)
			{
				UInt16 pixel = *reinterpret_cast<const UInt16*>(start);

				#ifdef NAZARA_BIG_ENDIAN
				SwapBytes(&pixel, sizeof(UInt16));
				#endif

				UInt8 r = c5to4((pixel & 0xF800) >> 11);
				UInt8 g = c5to4((pixel & 0x07C0) >> 6);
				UInt8 b = c5to4((pixel & 0x003E) >> 1);

				*ptr = (r << 12) | (g << 8) | (b << 4) | ((pixel & 0x1)*0x0F);

				#ifdef NAZARA_BIG_ENDIAN
				SwapBytes(ptr, sizeof(UInt16));
				#endif

				ptr++;
				start += 2;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormatType_RGB5A1, PixelFormatType_RGBA8>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			while (start < end)
			{
				UInt16 pixel = *reinterpret_cast<const UInt16*>(start);

				#ifdef NAZARA_BIG_ENDIAN
				SwapBytes(&pixel, sizeof(UInt16));
				#endif

				*dst++ = c5to8((pixel & 0xF800) >> 11);
				*dst++ = c5to8((pixel & 0x07C0) >> 6);
				*dst++ = c5to8((pixel & 0x003E) >> 1);
				*dst++ = static_cast<UInt8>((pixel & 0x1)*0xFF);

				start += 2;
			}

			return dst;
		}

		/**********************************RGB8***********************************/
		template<>
		UInt8* ConvertPixels<PixelFormatType_RGB8, PixelFormatType_BGR8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormatType_RGB8, PixelFormatType_BGRA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormatType_RGB8, PixelFormatType_L8>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			while (start < end)
			{
				*dst++ = static_cast<UInt8>(start[0] * 0.3f + start[1] * 0.59f + start[2] * 0.11f);

				start += 3;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormatType_RGB8, PixelFormatType_LA8>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			while (start < end)
			{
				*dst++ = static_cast<UInt8>(start[0] * 0.3f + start[1] * 0.59f + start[2] * 0.11f);
				*dst++ = 0xFF;

				start += 3;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormatType_RGB8, PixelFormatType_RGB5A1>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			UInt16* ptr = reinterpret_cast<UInt16*>(dst);
			while (start < end)
			{
				*ptr = (static_cast<UInt16>(c8to5(start[0])) << 11) |
					   (static_cast<UInt16>(c8to5(start[1])) << 6)  |
					   (static_cast<UInt16>(c8to5(start[2])) << 1)  |
					   0x1;

				#ifdef NAZARA_BIG_ENDIAN
				SwapBytes(ptr, sizeof(UInt16));
				#endif

				ptr++;
				start += 3;
			}

			return reinterpret_cast<UInt8*>(ptr);
		}

		template<>
		UInt8* ConvertPixels<PixelFormatType_RGB8, PixelFormatType_RGBA4>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			UInt16* ptr = reinterpret_cast<UInt16*>(dst);
			while (start < end)
			{
				*ptr = (static_cast<UInt16>(c8to4(start[0])) << 12) |
					   (static_cast<UInt16>(c8to4(start[1])) << 8)  |
					   (static_cast<UInt16>(c8to4(start[2])) << 4)  |
					   0x0F;

				#ifdef NAZARA_BIG_ENDIAN
				SwapBytes(ptr, sizeof(UInt16));
				#endif

				ptr++;
				start += 3;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormatType_RGB8, PixelFormatType_RGBA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormatType_RGBA8, PixelFormatType_A8>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			while (start < end)
			{
				*dst++ = start[3];

				start += 4;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormatType_RGBA8, PixelFormatType_BGR8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormatType_RGBA8, PixelFormatType_BGRA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormatType_RGBA8, PixelFormatType_L8>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			while (start < end)
			{
				*dst++ = static_cast<UInt8>(start[0] * 0.3f + start[1] * 0.59f + start[2] * 0.11f);

				start += 4;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormatType_RGBA8, PixelFormatType_LA8>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			while (start < end)
			{
				*dst++ = static_cast<UInt8>(start[0] * 0.3f + start[1] * 0.59f + start[2] * 0.11f);
				*dst++ = start[3];

				start += 4;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormatType_RGBA8, PixelFormatType_RGB5A1>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			UInt16* ptr = reinterpret_cast<UInt16*>(dst);
			while (start < end)
			{
				*ptr = (static_cast<UInt16>(c8to5(start[0])) << 11) |
					   (static_cast<UInt16>(c8to5(start[1])) << 6)  |
					   (static_cast<UInt16>(c8to5(start[2])) << 1)  |
					   ((start[3] > 0xF) ? 1 : 0); // > 128

				#ifdef NAZARA_BIG_ENDIAN
				SwapBytes(ptr, sizeof(UInt16));
				#endif

				ptr++;
				start += 4;
			}

			return reinterpret_cast<UInt8*>(ptr);
		}

		template<>
		UInt8* ConvertPixels<PixelFormatType_RGBA8, PixelFormatType_RGB8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormatType_RGBA8, PixelFormatType_RGBA4>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			UInt16* ptr = reinterpret_cast<UInt16*>(dst);
			while (start < end)
			{
				*ptr = (static_cast<UInt16>(c8to4(start[0])) << 12) |
					   (static_cast<UInt16>(c8to4(start[1])) << 8)  |
					   (static_cast<UInt16>(c8to4(start[2])) << 4)  |
					   (static_cast<UInt16>(c8to4(start[3])) << 0);

				#ifdef NAZARA_BIG_ENDIAN
				SwapBytes(ptr, sizeof(UInt16));
				#endif

				ptr++;
				start += 4;
			}

			return dst;
		}

		template<PixelFormatType format1, PixelFormatType format2>
		void RegisterConverter()
		{
			PixelFormat::SetConvertFunction(format1, format2, &ConvertPixels<format1, format2>);
		}
	}

	PixelFormatType PixelFormat::IdentifyFormat(const PixelFormatInfo& info)
	{
		for (unsigned int i = 0; i <= PixelFormatType_Max; ++i)
		{
			PixelFormatInfo& info2 = s_pixelFormatInfos[i];
			if (info.bitsPerPixel == info2.bitsPerPixel && info.content == info2.content &&
			    info.redMask == info2.redMask && info.greenMask == info2.greenMask && info.blueMask == info2.blueMask && info.alphaMask == info2.alphaMask &&
			    info.redType == info2.redType && info.greenType == info2.greenType && info.blueType == info2.blueType && info.alphaType == info2.alphaType)
				return static_cast<PixelFormatType>(i);
		}

		return PixelFormatType_Undefined;
	}

	bool PixelFormat::Initialize()
	{
		Bitset<> b32(0xFFFFFFFF);
		b32.Resize(128);

		// Setup informations about every pixel format
		s_pixelFormatInfos[PixelFormatType_A8]              = PixelFormatInfo("A8",              PixelFormatContent_ColorRGBA,    0,                  0,                  0,                  0xFF,               PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormatType_BGR8]            = PixelFormatInfo("BGR8",            PixelFormatContent_ColorRGBA,    0x0000FF,           0x00FF00,           0xFF0000,           0,                  PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormatType_BGRA8]           = PixelFormatInfo("BGRA8",           PixelFormatContent_ColorRGBA,    0x0000FF00,         0x00FF0000,         0xFF000000,         0x000000FF,         PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormatType_DXT1]            = PixelFormatInfo("DXT1",            PixelFormatContent_ColorRGBA,    8,                                                                              PixelFormatSubType_Compressed);
		s_pixelFormatInfos[PixelFormatType_DXT3]            = PixelFormatInfo("DXT3",            PixelFormatContent_ColorRGBA,    16,                                                                             PixelFormatSubType_Compressed);
		s_pixelFormatInfos[PixelFormatType_DXT5]            = PixelFormatInfo("DXT5",            PixelFormatContent_ColorRGBA,    16,                                                                             PixelFormatSubType_Compressed);
		s_pixelFormatInfos[PixelFormatType_L8]              = PixelFormatInfo("L8",              PixelFormatContent_ColorRGBA,    0xFF,               0xFF,               0xFF,               0,                  PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormatType_LA8]             = PixelFormatInfo("LA8",             PixelFormatContent_ColorRGBA,    0xFF00,             0xFF00,             0xFF00,             0x00FF,             PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormatType_R8]              = PixelFormatInfo("R8",              PixelFormatContent_ColorRGBA,    0xFF,               0,                  0,                  0,                  PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormatType_R8I]             = PixelFormatInfo("R8I",             PixelFormatContent_ColorRGBA,    0xFF,               0,                  0,                  0,                  PixelFormatSubType_Int);
		s_pixelFormatInfos[PixelFormatType_R8UI]            = PixelFormatInfo("R8UI",            PixelFormatContent_ColorRGBA,    0xFF,               0,                  0,                  0,                  PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormatType_R16]             = PixelFormatInfo("R16",             PixelFormatContent_ColorRGBA,    0xFFFF,             0,                  0,                  0,                  PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormatType_R16F]            = PixelFormatInfo("R16F",            PixelFormatContent_ColorRGBA,    0xFFFF,             0,                  0,                  0,                  PixelFormatSubType_Half);
		s_pixelFormatInfos[PixelFormatType_R16I]            = PixelFormatInfo("R16I",            PixelFormatContent_ColorRGBA,    0xFFFF,             0,                  0,                  0,                  PixelFormatSubType_Int);
		s_pixelFormatInfos[PixelFormatType_R16UI]           = PixelFormatInfo("R16UI",           PixelFormatContent_ColorRGBA,    0xFFFF,             0,                  0,                  0,                  PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormatType_R32F]            = PixelFormatInfo("R32F",            PixelFormatContent_ColorRGBA,    0xFFFFFFFF,         0,                  0,                  0,                  PixelFormatSubType_Float);
		s_pixelFormatInfos[PixelFormatType_R32I]            = PixelFormatInfo("R32I",            PixelFormatContent_ColorRGBA,    0xFFFFFFFF,         0,                  0,                  0,                  PixelFormatSubType_Int);
		s_pixelFormatInfos[PixelFormatType_R32UI]           = PixelFormatInfo("R32UI",           PixelFormatContent_ColorRGBA,    0xFFFFFFFF,         0,                  0,                  0,                  PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormatType_RG8]             = PixelFormatInfo("RG8",             PixelFormatContent_ColorRGBA,    0xFF00,             0x00FF,             0,                  0,                  PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormatType_RG8I]            = PixelFormatInfo("RG8I",            PixelFormatContent_ColorRGBA,    0xFF00,             0x00FF,             0,                  0,                  PixelFormatSubType_Int);
		s_pixelFormatInfos[PixelFormatType_RG8UI]           = PixelFormatInfo("RG8UI",           PixelFormatContent_ColorRGBA,    0xFF00,             0x00FF,             0,                  0,                  PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormatType_RG16]            = PixelFormatInfo("RG16",            PixelFormatContent_ColorRGBA,    0xFFFF0000,         0x0000FFFF,         0,                  0,                  PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormatType_RG16F]           = PixelFormatInfo("RG16F",           PixelFormatContent_ColorRGBA,    0xFFFF0000,         0x0000FFFF,         0,                  0,                  PixelFormatSubType_Half);
		s_pixelFormatInfos[PixelFormatType_RG16I]           = PixelFormatInfo("RG16I",           PixelFormatContent_ColorRGBA,    0xFFFF0000,         0x0000FFFF,         0,                  0,                  PixelFormatSubType_Int);
		s_pixelFormatInfos[PixelFormatType_RG16UI]          = PixelFormatInfo("RG16UI",          PixelFormatContent_ColorRGBA,    0xFFFF0000,         0x0000FFFF,         0,                  0,                  PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormatType_RG32F]           = PixelFormatInfo("RG32F",           PixelFormatContent_ColorRGBA,    0xFFFFFFFF00000000, 0x00000000FFFFFFFF, 0,                  0,                  PixelFormatSubType_Float);
		s_pixelFormatInfos[PixelFormatType_RG32I]           = PixelFormatInfo("RG32I",           PixelFormatContent_ColorRGBA,    0xFFFFFFFF00000000, 0x00000000FFFFFFFF, 0,                  0,                  PixelFormatSubType_Int);
		s_pixelFormatInfos[PixelFormatType_RG32UI]          = PixelFormatInfo("RG32UI",          PixelFormatContent_ColorRGBA,    0xFFFFFFFF00000000, 0x00000000FFFFFFFF, 0,                  0,                  PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormatType_RGB8]            = PixelFormatInfo("RGB8",            PixelFormatContent_ColorRGBA,    0xFF000000,         0x00FF0000,         0x0000FF00,         0,                  PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormatType_RGB16F]          = PixelFormatInfo("RGB16F",          PixelFormatContent_ColorRGBA,    0xFFFF00000000,     0x0000FFFF0000,     0x00000000FFFF,     0,                  PixelFormatSubType_Half);
		s_pixelFormatInfos[PixelFormatType_RGB16I]          = PixelFormatInfo("RGB16I",          PixelFormatContent_ColorRGBA,    0xFFFF00000000,     0x0000FFFF0000,     0x00000000FFFF,     0,                  PixelFormatSubType_Int);
		s_pixelFormatInfos[PixelFormatType_RGB16UI]         = PixelFormatInfo("RGB16UI",         PixelFormatContent_ColorRGBA,    0xFFFF000000000000, 0x0000FFFF00000000, 0x00000000FFFF0000, 0,                  PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormatType_RGB32F]          = PixelFormatInfo("RGB32F",          PixelFormatContent_ColorRGBA,    b32,                b32 >> 32,          b32 >> 64,          0,                  PixelFormatSubType_Float);
		s_pixelFormatInfos[PixelFormatType_RGB32I]          = PixelFormatInfo("RGB32I",          PixelFormatContent_ColorRGBA,    b32,                b32 >> 32,          b32 >> 64,          0,                  PixelFormatSubType_Int);
		s_pixelFormatInfos[PixelFormatType_RGB32UI]         = PixelFormatInfo("RGB32UI",         PixelFormatContent_ColorRGBA,    b32,                b32 >> 32,          b32 >> 64,          0,                  PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormatType_RGBA4]           = PixelFormatInfo("RGBA4",           PixelFormatContent_ColorRGBA,    0xF000,             0x0F00,             0x00F0,             0x000F,             PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormatType_RGB5A1]          = PixelFormatInfo("RGB5A1",          PixelFormatContent_ColorRGBA,    0xF800,             0x07C0,             0x003E,             0x0001,             PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormatType_RGBA8]           = PixelFormatInfo("RGBA8",           PixelFormatContent_ColorRGBA,    0xFF000000,         0x00FF0000,         0x0000FF00,         0x000000FF,         PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormatType_RGBA16F]         = PixelFormatInfo("RGBA16F",         PixelFormatContent_ColorRGBA,    0xFFFF000000000000, 0x0000FFFF00000000, 0x00000000FFFF0000, 0x000000000000FFFF, PixelFormatSubType_Half);
		s_pixelFormatInfos[PixelFormatType_RGBA16I]         = PixelFormatInfo("RGBA16I",         PixelFormatContent_ColorRGBA,    0xFFFF000000000000, 0x0000FFFF00000000, 0x00000000FFFF0000, 0x000000000000FFFF, PixelFormatSubType_Int);
		s_pixelFormatInfos[PixelFormatType_RGBA16UI]        = PixelFormatInfo("RGBA16UI",        PixelFormatContent_ColorRGBA,    0xFFFF000000000000, 0x0000FFFF00000000, 0x00000000FFFF0000, 0x000000000000FFFF, PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormatType_RGBA32F]         = PixelFormatInfo("RGBA32F",         PixelFormatContent_ColorRGBA,    b32,                b32 >> 32,          b32 >> 64,          b32 >> 96,          PixelFormatSubType_Float);
		s_pixelFormatInfos[PixelFormatType_RGBA32I]         = PixelFormatInfo("RGBA32I",         PixelFormatContent_ColorRGBA,    b32,                b32 >> 32,          b32 >> 64,          b32 >> 96,          PixelFormatSubType_Int);
		s_pixelFormatInfos[PixelFormatType_RGBA32UI]        = PixelFormatInfo("RGBA32UI",        PixelFormatContent_ColorRGBA,    b32,                b32 >> 32,          b32 >> 64,          b32 >> 96,          PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormatType_Depth16]         = PixelFormatInfo("Depth16",         PixelFormatContent_DepthStencil, 0xFFFF,             0,                  0,                  0,                  PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormatType_Depth24]         = PixelFormatInfo("Depth24",         PixelFormatContent_DepthStencil, 0xFFFFFF,           0,                  0,                  0,                  PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormatType_Depth24Stencil8] = PixelFormatInfo("Depth24Stencil8", PixelFormatContent_DepthStencil, 0xFFFFFF00,         0x000000FF,         0,                  0,                  PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormatType_Depth32]         = PixelFormatInfo("Depth32",         PixelFormatContent_DepthStencil, 0xFFFFFFFF,         0,                  0,                  0,                  PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormatType_Stencil1]        = PixelFormatInfo("Stencil1",        PixelFormatContent_Stencil,      0x1,                0,                  0,                  0,                  PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormatType_Stencil4]        = PixelFormatInfo("Stencil4",        PixelFormatContent_Stencil,      0xF,                0,                  0,                  0,                  PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormatType_Stencil8]        = PixelFormatInfo("Stencil8",        PixelFormatContent_Stencil,      0xFF,               0,                  0,                  0,                  PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormatType_Stencil16]       = PixelFormatInfo("Stencil16",       PixelFormatContent_Stencil,      0xFFFF,             0,                  0,                  0,                  PixelFormatSubType_Unsigned);

		for (unsigned int i = 0; i <= PixelFormatType_Max; ++i)
		{
			if (!s_pixelFormatInfos[i].Validate())
				NazaraWarning("Pixel format 0x" + String::Number(i, 16) + " (" + GetName(static_cast<Nz::PixelFormatType>(i)) + ") failed validation tests");
		}

		// Reset functions
		std::memset(s_convertFunctions, 0, (PixelFormatType_Max+1)*(PixelFormatType_Max+1)*sizeof(PixelFormat::ConvertFunction));

		/***********************************A8************************************/
		RegisterConverter<PixelFormatType_A8, PixelFormatType_BGRA8>();
		RegisterConverter<PixelFormatType_A8, PixelFormatType_LA8>();
		RegisterConverter<PixelFormatType_A8, PixelFormatType_RGB5A1>();
		RegisterConverter<PixelFormatType_A8, PixelFormatType_RGBA4>();
		RegisterConverter<PixelFormatType_A8, PixelFormatType_RGBA8>();

		/**********************************BGR8***********************************/
		RegisterConverter<PixelFormatType_BGR8, PixelFormatType_BGRA8>();
		RegisterConverter<PixelFormatType_BGR8, PixelFormatType_L8>();
		RegisterConverter<PixelFormatType_BGR8, PixelFormatType_LA8>();/*
		RegisterConverter<PixelFormatType_BGR8, PixelFormatType_RGB16F>();
		RegisterConverter<PixelFormatType_BGR8, PixelFormatType_RGB16I>();
		RegisterConverter<PixelFormatType_BGR8, PixelFormatType_RGB32F>();
		RegisterConverter<PixelFormatType_BGR8, PixelFormatType_RGB32I>();
		RegisterConverter<PixelFormatType_BGR8, PixelFormatType_RGBA16F>();
		RegisterConverter<PixelFormatType_BGR8, PixelFormatType_RGBA16I>();
		RegisterConverter<PixelFormatType_BGR8, PixelFormatType_RGBA32F>();
		RegisterConverter<PixelFormatType_BGR8, PixelFormatType_RGBA32I>();*/
		RegisterConverter<PixelFormatType_BGR8, PixelFormatType_RGB5A1>();
		RegisterConverter<PixelFormatType_BGR8, PixelFormatType_RGB8>();
		RegisterConverter<PixelFormatType_BGR8, PixelFormatType_RGBA4>();
		RegisterConverter<PixelFormatType_BGR8, PixelFormatType_RGBA8>();

		/**********************************BGRA8**********************************/
		RegisterConverter<PixelFormatType_BGRA8, PixelFormatType_A8>();
		RegisterConverter<PixelFormatType_BGRA8, PixelFormatType_BGR8>();
		RegisterConverter<PixelFormatType_BGRA8, PixelFormatType_L8>();
		RegisterConverter<PixelFormatType_BGRA8, PixelFormatType_LA8>();/*
		RegisterConverter<PixelFormatType_BGRA8, PixelFormatType_RGB16F>();
		RegisterConverter<PixelFormatType_BGRA8, PixelFormatType_RGB16I>();
		RegisterConverter<PixelFormatType_BGRA8, PixelFormatType_RGB32F>();
		RegisterConverter<PixelFormatType_BGRA8, PixelFormatType_RGB32I>();
		RegisterConverter<PixelFormatType_BGRA8, PixelFormatType_RGBA16F>();
		RegisterConverter<PixelFormatType_BGRA8, PixelFormatType_RGBA16I>();
		RegisterConverter<PixelFormatType_BGRA8, PixelFormatType_RGBA32F>();
		RegisterConverter<PixelFormatType_BGRA8, PixelFormatType_RGBA32I>();*/
		RegisterConverter<PixelFormatType_BGRA8, PixelFormatType_RGB5A1>();
		RegisterConverter<PixelFormatType_BGRA8, PixelFormatType_RGB8>();
		RegisterConverter<PixelFormatType_BGRA8, PixelFormatType_RGBA4>();
		RegisterConverter<PixelFormatType_BGRA8, PixelFormatType_RGBA8>();

		/**********************************DXT1***********************************/
		///TODO: Décompresseur DXT1
	/*
		RegisterConverter<PixelFormatType_DXT1, PixelFormatType_BGR8>();
		RegisterConverter<PixelFormatType_DXT1, PixelFormatType_BGRA8>();
		RegisterConverter<PixelFormatType_DXT1, PixelFormatType_DXT3>();
		RegisterConverter<PixelFormatType_DXT1, PixelFormatType_DXT5>();
		RegisterConverter<PixelFormatType_DXT1, PixelFormatType_L8>();
		RegisterConverter<PixelFormatType_DXT1, PixelFormatType_LA8>();
		RegisterConverter<PixelFormatType_DXT1, PixelFormatType_RGB16F>();
		RegisterConverter<PixelFormatType_DXT1, PixelFormatType_RGB16I>();
		RegisterConverter<PixelFormatType_DXT1, PixelFormatType_RGB32F>();
		RegisterConverter<PixelFormatType_DXT1, PixelFormatType_RGB32I>();
		RegisterConverter<PixelFormatType_DXT1, PixelFormatType_RGBA16F>();
		RegisterConverter<PixelFormatType_DXT1, PixelFormatType_RGBA16I>();
		RegisterConverter<PixelFormatType_DXT1, PixelFormatType_RGBA32F>();
		RegisterConverter<PixelFormatType_DXT1, PixelFormatType_RGBA32I>();
		RegisterConverter<PixelFormatType_DXT1, PixelFormatType_RGB5A1>();
		RegisterConverter<PixelFormatType_DXT1, PixelFormatType_RGB8>();
		RegisterConverter<PixelFormatType_DXT1, PixelFormatType_RGBA4>();
		RegisterConverter<PixelFormatType_DXT1, PixelFormatType_RGBA8>();
	*/

		/**********************************DXT3***********************************/
		///TODO: Décompresseur DXT3
	/*
		RegisterConverter<PixelFormatType_DXT3, PixelFormatType_BGR8>();
		RegisterConverter<PixelFormatType_DXT3, PixelFormatType_BGRA8>();
		RegisterConverter<PixelFormatType_DXT3, PixelFormatType_DXT1>();
		RegisterConverter<PixelFormatType_DXT3, PixelFormatType_DXT5>();
		RegisterConverter<PixelFormatType_DXT3, PixelFormatType_L8>();
		RegisterConverter<PixelFormatType_DXT3, PixelFormatType_LA8>();
		RegisterConverter<PixelFormatType_DXT3, PixelFormatType_RGB16F>();
		RegisterConverter<PixelFormatType_DXT3, PixelFormatType_RGB16I>();
		RegisterConverter<PixelFormatType_DXT3, PixelFormatType_RGB32F>();
		RegisterConverter<PixelFormatType_DXT3, PixelFormatType_RGB32I>();
		RegisterConverter<PixelFormatType_DXT3, PixelFormatType_RGBA16F>();
		RegisterConverter<PixelFormatType_DXT3, PixelFormatType_RGBA16I>();
		RegisterConverter<PixelFormatType_DXT3, PixelFormatType_RGBA32F>();
		RegisterConverter<PixelFormatType_DXT3, PixelFormatType_RGBA32I>();
		RegisterConverter<PixelFormatType_DXT3, PixelFormatType_RGB5A1>();
		RegisterConverter<PixelFormatType_DXT3, PixelFormatType_RGB8>();
		RegisterConverter<PixelFormatType_DXT3, PixelFormatType_RGBA4>();
		RegisterConverter<PixelFormatType_DXT3, PixelFormatType_RGBA8>();
	*/

		/**********************************DXT5***********************************/
		///TODO: Décompresseur DXT5
	/*
		RegisterConverter<PixelFormatType_DXT5, PixelFormatType_BGR8>();
		RegisterConverter<PixelFormatType_DXT5, PixelFormatType_BGRA8>();
		RegisterConverter<PixelFormatType_DXT5, PixelFormatType_DXT1>();
		RegisterConverter<PixelFormatType_DXT5, PixelFormatType_DXT3>();
		RegisterConverter<PixelFormatType_DXT5, PixelFormatType_L8>();
		RegisterConverter<PixelFormatType_DXT5, PixelFormatType_LA8>();
		RegisterConverter<PixelFormatType_DXT5, PixelFormatType_RGB16F>();
		RegisterConverter<PixelFormatType_DXT5, PixelFormatType_RGB16I>();
		RegisterConverter<PixelFormatType_DXT5, PixelFormatType_RGB32F>();
		RegisterConverter<PixelFormatType_DXT5, PixelFormatType_RGB32I>();
		RegisterConverter<PixelFormatType_DXT5, PixelFormatType_RGBA16F>();
		RegisterConverter<PixelFormatType_DXT5, PixelFormatType_RGBA16I>();
		RegisterConverter<PixelFormatType_DXT5, PixelFormatType_RGBA32F>();
		RegisterConverter<PixelFormatType_DXT5, PixelFormatType_RGBA32I>();
		RegisterConverter<PixelFormatType_DXT5, PixelFormatType_RGB5A1>();
		RegisterConverter<PixelFormatType_DXT5, PixelFormatType_RGB8>();
		RegisterConverter<PixelFormatType_DXT5, PixelFormatType_RGBA4>();
		RegisterConverter<PixelFormatType_DXT5, PixelFormatType_RGBA8>();
	*/

		/***********************************L8************************************/
		RegisterConverter<PixelFormatType_L8, PixelFormatType_BGR8>();
		RegisterConverter<PixelFormatType_L8, PixelFormatType_BGRA8>();
		RegisterConverter<PixelFormatType_L8, PixelFormatType_LA8>();/*
		RegisterConverter<PixelFormatType_L8, PixelFormatType_RGB16F>();
		RegisterConverter<PixelFormatType_L8, PixelFormatType_RGB16I>();
		RegisterConverter<PixelFormatType_L8, PixelFormatType_RGB32F>();
		RegisterConverter<PixelFormatType_L8, PixelFormatType_RGB32I>();
		RegisterConverter<PixelFormatType_L8, PixelFormatType_RGBA16F>();
		RegisterConverter<PixelFormatType_L8, PixelFormatType_RGBA16I>();
		RegisterConverter<PixelFormatType_L8, PixelFormatType_RGBA32F>();
		RegisterConverter<PixelFormatType_L8, PixelFormatType_RGBA32I>();*/
		RegisterConverter<PixelFormatType_L8, PixelFormatType_RGB5A1>();
		RegisterConverter<PixelFormatType_L8, PixelFormatType_RGB8>();
		RegisterConverter<PixelFormatType_L8, PixelFormatType_RGBA4>();
		RegisterConverter<PixelFormatType_L8, PixelFormatType_RGBA8>();

		/***********************************LA8***********************************/
		RegisterConverter<PixelFormatType_LA8, PixelFormatType_A8>();
		RegisterConverter<PixelFormatType_LA8, PixelFormatType_BGR8>();
		RegisterConverter<PixelFormatType_LA8, PixelFormatType_BGRA8>();
		RegisterConverter<PixelFormatType_LA8, PixelFormatType_L8>();/*
		RegisterConverter<PixelFormatType_LA8, PixelFormatType_RGB16F>();
		RegisterConverter<PixelFormatType_LA8, PixelFormatType_RGB16I>();
		RegisterConverter<PixelFormatType_LA8, PixelFormatType_RGB32F>();
		RegisterConverter<PixelFormatType_LA8, PixelFormatType_RGB32I>();
		RegisterConverter<PixelFormatType_LA8, PixelFormatType_RGBA16F>();
		RegisterConverter<PixelFormatType_LA8, PixelFormatType_RGBA16I>();
		RegisterConverter<PixelFormatType_LA8, PixelFormatType_RGBA32F>();
		RegisterConverter<PixelFormatType_LA8, PixelFormatType_RGBA32I>();*/
		RegisterConverter<PixelFormatType_LA8, PixelFormatType_RGB5A1>();
		RegisterConverter<PixelFormatType_LA8, PixelFormatType_RGB8>();
		RegisterConverter<PixelFormatType_LA8, PixelFormatType_RGBA4>();
		RegisterConverter<PixelFormatType_LA8, PixelFormatType_RGBA8>();

		/**********************************RGBA4**********************************/
		RegisterConverter<PixelFormatType_RGBA4, PixelFormatType_A8>();
		RegisterConverter<PixelFormatType_RGBA4, PixelFormatType_BGR8>();
		RegisterConverter<PixelFormatType_RGBA4, PixelFormatType_BGRA8>();
		RegisterConverter<PixelFormatType_RGBA4, PixelFormatType_L8>();
		RegisterConverter<PixelFormatType_RGBA4, PixelFormatType_LA8>();/*
		RegisterConverter<PixelFormatType_RGBA4, PixelFormatType_RGB16F>();
		RegisterConverter<PixelFormatType_RGBA4, PixelFormatType_RGB16I>();
		RegisterConverter<PixelFormatType_RGBA4, PixelFormatType_RGB32F>();
		RegisterConverter<PixelFormatType_RGBA4, PixelFormatType_RGB32I>();
		RegisterConverter<PixelFormatType_RGBA4, PixelFormatType_RGBA16F>();
		RegisterConverter<PixelFormatType_RGBA4, PixelFormatType_RGBA16I>();
		RegisterConverter<PixelFormatType_RGBA4, PixelFormatType_RGBA32F>();
		RegisterConverter<PixelFormatType_RGBA4, PixelFormatType_RGBA32I>();*/
		RegisterConverter<PixelFormatType_RGBA4, PixelFormatType_RGB5A1>();
		RegisterConverter<PixelFormatType_RGBA4, PixelFormatType_RGB8>();
		RegisterConverter<PixelFormatType_RGBA4, PixelFormatType_RGBA8>();

		/*********************************RGB5A1**********************************/
		RegisterConverter<PixelFormatType_RGB5A1, PixelFormatType_A8>();
		RegisterConverter<PixelFormatType_RGB5A1, PixelFormatType_BGR8>();
		RegisterConverter<PixelFormatType_RGB5A1, PixelFormatType_BGRA8>();
		RegisterConverter<PixelFormatType_RGB5A1, PixelFormatType_L8>();
		RegisterConverter<PixelFormatType_RGB5A1, PixelFormatType_LA8>();/*
		RegisterConverter<PixelFormatType_RGB5A1, PixelFormatType_RGB16F>();
		RegisterConverter<PixelFormatType_RGB5A1, PixelFormatType_RGB16I>();
		RegisterConverter<PixelFormatType_RGB5A1, PixelFormatType_RGB32F>();
		RegisterConverter<PixelFormatType_RGB5A1, PixelFormatType_RGB32I>();
		RegisterConverter<PixelFormatType_RGB5A1, PixelFormatType_RGBA16F>();
		RegisterConverter<PixelFormatType_RGB5A1, PixelFormatType_RGBA16I>();
		RegisterConverter<PixelFormatType_RGB5A1, PixelFormatType_RGBA32F>();
		RegisterConverter<PixelFormatType_RGB5A1, PixelFormatType_RGBA32I>();*/
		RegisterConverter<PixelFormatType_RGB5A1, PixelFormatType_RGB8>();
		RegisterConverter<PixelFormatType_RGB5A1, PixelFormatType_RGBA4>();
		RegisterConverter<PixelFormatType_RGB5A1, PixelFormatType_RGBA8>();

		/**********************************RGB8***********************************/
		RegisterConverter<PixelFormatType_RGB8, PixelFormatType_BGR8>();
		RegisterConverter<PixelFormatType_RGB8, PixelFormatType_BGRA8>();
		RegisterConverter<PixelFormatType_RGB8, PixelFormatType_L8>();
		RegisterConverter<PixelFormatType_RGB8, PixelFormatType_LA8>();/*
		RegisterConverter<PixelFormatType_RGB8, PixelFormatType_RGB16F>();
		RegisterConverter<PixelFormatType_RGB8, PixelFormatType_RGB16I>();
		RegisterConverter<PixelFormatType_RGB8, PixelFormatType_RGB32F>();
		RegisterConverter<PixelFormatType_RGB8, PixelFormatType_RGB32I>();
		RegisterConverter<PixelFormatType_RGB8, PixelFormatType_RGBA16F>();
		RegisterConverter<PixelFormatType_RGB8, PixelFormatType_RGBA16I>();
		RegisterConverter<PixelFormatType_RGB8, PixelFormatType_RGBA32F>();
		RegisterConverter<PixelFormatType_RGB8, PixelFormatType_RGBA32I>();*/
		RegisterConverter<PixelFormatType_RGB8, PixelFormatType_RGB5A1>();
		RegisterConverter<PixelFormatType_RGB8, PixelFormatType_RGBA4>();
		RegisterConverter<PixelFormatType_RGB8, PixelFormatType_RGBA8>();

		/**********************************RGBA8**********************************/
		RegisterConverter<PixelFormatType_RGBA8, PixelFormatType_A8>();
		RegisterConverter<PixelFormatType_RGBA8, PixelFormatType_BGR8>();
		RegisterConverter<PixelFormatType_RGBA8, PixelFormatType_BGRA8>();
		RegisterConverter<PixelFormatType_RGBA8, PixelFormatType_L8>();
		RegisterConverter<PixelFormatType_RGBA8, PixelFormatType_LA8>();/*
		RegisterConverter<PixelFormatType_RGBA8, PixelFormatType_RGB16F>();
		RegisterConverter<PixelFormatType_RGBA8, PixelFormatType_RGB16I>();
		RegisterConverter<PixelFormatType_RGBA8, PixelFormatType_RGB32F>();
		RegisterConverter<PixelFormatType_RGBA8, PixelFormatType_RGB32I>();
		RegisterConverter<PixelFormatType_RGBA8, PixelFormatType_RGBA16F>();
		RegisterConverter<PixelFormatType_RGBA8, PixelFormatType_RGBA16I>();
		RegisterConverter<PixelFormatType_RGBA8, PixelFormatType_RGBA32F>();
		RegisterConverter<PixelFormatType_RGBA8, PixelFormatType_RGBA32I>();*/
		RegisterConverter<PixelFormatType_RGBA8, PixelFormatType_RGB5A1>();
		RegisterConverter<PixelFormatType_RGBA8, PixelFormatType_RGB8>();
		RegisterConverter<PixelFormatType_RGBA8, PixelFormatType_RGBA4>();

		return true;
	}

	void PixelFormat::Uninitialize()
	{
		for (unsigned int i = 0; i <= PixelFormatType_Max; ++i)
			s_pixelFormatInfos[i].Clear();

		std::memset(s_convertFunctions, 0, (PixelFormatType_Max+1)*(PixelFormatType_Max+1)*sizeof(PixelFormat::ConvertFunction));

		for (unsigned int i = 0; i <= PixelFlipping_Max; ++i)
			s_flipFunctions[i].clear();
	}

	PixelFormatInfo PixelFormat::s_pixelFormatInfos[PixelFormatType_Max + 1];
	PixelFormat::ConvertFunction PixelFormat::s_convertFunctions[PixelFormatType_Max+1][PixelFormatType_Max+1];
	std::map<PixelFormatType, PixelFormat::FlipFunction> PixelFormat::s_flipFunctions[PixelFlipping_Max+1];
}
