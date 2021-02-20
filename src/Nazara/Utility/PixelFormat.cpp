// Copyright (C) 2020 Jérôme Leclercq
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

		template<PixelFormat from, PixelFormat to>
		UInt8* ConvertPixels(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			NazaraUnused(start);
			NazaraUnused(dst);
			NazaraUnused(end);

			NazaraInternalError("Conversion from " + PixelFormatInfo::GetName(from) + " to " + PixelFormatInfo::GetName(to) + " is not supported");
			return nullptr;
		}

		/**********************************A8***********************************/
		template<>
		UInt8* ConvertPixels<PixelFormat_A8, PixelFormat_BGRA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_A8, PixelFormat_LA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_A8, PixelFormat_RGB5A1>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_A8, PixelFormat_RGBA4>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_A8, PixelFormat_RGBA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_BGR8, PixelFormat_BGRA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_BGR8, PixelFormat_BGR8_SRGB>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			//FIXME: Not correct
			std::size_t count = end - start;
			std::memcpy(dst, start, count);
			return dst + count;
		}

		template<>
		UInt8* ConvertPixels<PixelFormat_BGR8, PixelFormat_L8>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			while (start < end)
			{
				*dst++ = static_cast<UInt8>(start[2] * 0.3f + start[1] * 0.59f + start[0] * 0.11f);

				start += 3;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormat_BGR8, PixelFormat_LA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_BGR8, PixelFormat_RGB5A1>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_BGR8, PixelFormat_RGB8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_BGR8, PixelFormat_RGBA4>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_BGR8, PixelFormat_RGBA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_BGRA8, PixelFormat_A8>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			while (start < end)
			{
				*dst++ = start[3];

				start += 4;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormat_BGRA8, PixelFormat_BGR8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_BGRA8, PixelFormat_BGRA8_SRGB>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			//FIXME: Not correct
			std::size_t count = end - start;
			std::memcpy(dst, start, count);
			return dst + count;
		}

		template<>
		UInt8* ConvertPixels<PixelFormat_BGRA8, PixelFormat_L8>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			while (start < end)
			{
				*dst++ = static_cast<UInt8>(start[2] * 0.3f + start[1] * 0.59f + start[0] * 0.11f);

				start += 4;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormat_BGRA8, PixelFormat_LA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_BGRA8, PixelFormat_RGBA4>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_BGRA8, PixelFormat_RGB5A1>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_BGRA8, PixelFormat_RGB8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_BGRA8, PixelFormat_RGBA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_L8, PixelFormat_BGR8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_L8, PixelFormat_BGRA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_L8, PixelFormat_LA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_L8, PixelFormat_RGB5A1>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_L8, PixelFormat_RGB8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_L8, PixelFormat_RGBA4>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_L8, PixelFormat_RGBA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_LA8, PixelFormat_A8>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			while (start < end)
			{
				*dst++ = start[1];

				start += 2;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormat_LA8, PixelFormat_BGR8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_LA8, PixelFormat_BGRA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_LA8, PixelFormat_L8>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			while (start < end)
			{
				*dst++ = start[0];

				start += 2;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormat_LA8, PixelFormat_RGB5A1>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_LA8, PixelFormat_RGB8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_LA8, PixelFormat_RGBA4>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_LA8, PixelFormat_RGBA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_RGBA4, PixelFormat_A8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_RGBA4, PixelFormat_BGR8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_RGBA4, PixelFormat_BGRA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_RGBA4, PixelFormat_L8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_RGBA4, PixelFormat_LA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_RGBA4, PixelFormat_RGB5A1>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_RGBA4, PixelFormat_RGB8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_RGBA4, PixelFormat_RGBA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_RGB5A1, PixelFormat_A8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_RGB5A1, PixelFormat_BGR8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_RGB5A1, PixelFormat_BGRA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_RGB5A1, PixelFormat_L8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_RGB5A1, PixelFormat_LA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_RGB5A1, PixelFormat_RGB8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_RGB5A1, PixelFormat_RGBA4>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_RGB5A1, PixelFormat_RGBA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_RGB8, PixelFormat_BGR8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_RGB8, PixelFormat_BGRA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_RGB8, PixelFormat_L8>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			while (start < end)
			{
				*dst++ = static_cast<UInt8>(start[0] * 0.3f + start[1] * 0.59f + start[2] * 0.11f);

				start += 3;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormat_RGB8, PixelFormat_LA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_RGB8, PixelFormat_RGB5A1>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_RGB8, PixelFormat_RGBA4>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_RGB8, PixelFormat_RGB8_SRGB>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			//FIXME: Not correct
			std::size_t count = end - start;
			std::memcpy(dst, start, count);
			return dst + count;
		}

		template<>
		UInt8* ConvertPixels<PixelFormat_RGB8, PixelFormat_RGBA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_RGBA8, PixelFormat_A8>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			while (start < end)
			{
				*dst++ = start[3];

				start += 4;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormat_RGBA8, PixelFormat_BGR8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_RGBA8, PixelFormat_BGRA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_RGBA8, PixelFormat_L8>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			while (start < end)
			{
				*dst++ = static_cast<UInt8>(start[0] * 0.3f + start[1] * 0.59f + start[2] * 0.11f);

				start += 4;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormat_RGBA8, PixelFormat_LA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_RGBA8, PixelFormat_RGB5A1>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_RGBA8, PixelFormat_RGB8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat_RGBA8, PixelFormat_RGBA4>(const UInt8* start, const UInt8* end, UInt8* dst)
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

		template<>
		UInt8* ConvertPixels<PixelFormat_RGBA8, PixelFormat_RGBA8_SRGB>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			//FIXME: Not correct
			std::size_t count = end - start;
			std::memcpy(dst, start, count);
			return dst + count;
		}


		template<PixelFormat Format1, PixelFormat Format2>
		void RegisterConverter()
		{
			PixelFormatInfo::SetConvertFunction(Format1, Format2, &ConvertPixels<Format1, Format2>);
		}
	}

	bool PixelFormatInfo::Flip(PixelFlipping flipping, PixelFormat format, unsigned int width, unsigned int height, unsigned int depth, const void* src, void* dst)
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
							for (unsigned int y = 0; y < height / 2; ++y)
								std::swap_ranges(&ptr[y*lineStride], &ptr[(y + 1)*lineStride - 1], &ptr[(height - y - 1)*lineStride]);
						}
					}
					else
					{
						for (unsigned int z = 0; z < depth; ++z)
						{
							const UInt8* srcPtr = reinterpret_cast<const UInt8*>(src);
							UInt8* dstPtr = reinterpret_cast<UInt8*>(dst) + (width - 1)*height*depth*bpp;
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
								for (unsigned int x = 0; x < width / 2; ++x)
									std::swap_ranges(&ptr[x*bpp], &ptr[(x + 1)*bpp], &ptr[(width - x)*bpp]);

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
									std::memcpy(&ptr[x*bpp], &ptr[(width - x)*bpp], bpp);

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

	PixelFormat PixelFormatInfo::IdentifyFormat(const PixelFormatDescription& info)
	{
		for (unsigned int i = 0; i <= PixelFormat_Max; ++i)
		{
			PixelFormatDescription& info2 = s_pixelFormatInfos[i];
			if (info.bitsPerPixel == info2.bitsPerPixel && info.content == info2.content &&
			    info.redMask == info2.redMask && info.greenMask == info2.greenMask && info.blueMask == info2.blueMask && info.alphaMask == info2.alphaMask &&
			    info.redType == info2.redType && info.greenType == info2.greenType && info.blueType == info2.blueType && info.alphaType == info2.alphaType)
				return static_cast<PixelFormat>(i);
		}

		return PixelFormat_Undefined;
	}

	bool PixelFormatInfo::Initialize()
	{
		Bitset<> b32(0xFFFFFFFF);
		b32.Resize(128);

		// Setup informations about every pixel format
		s_pixelFormatInfos[PixelFormat_A8]              = PixelFormatDescription("A8",              PixelFormatContent_ColorRGBA,    0,                  0,                  0,                  0xFF,               PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormat_BGR8]            = PixelFormatDescription("BGR8",            PixelFormatContent_ColorRGBA,    0x0000FF,           0x00FF00,           0xFF0000,           0,                  PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormat_BGR8_SRGB]       = PixelFormatDescription("BGR8_SRGB",       PixelFormatContent_ColorRGBA,    0x0000FF,           0x00FF00,           0xFF0000,           0,                  PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormat_BGRA8]           = PixelFormatDescription("BGRA8",           PixelFormatContent_ColorRGBA,    0x0000FF00,         0x00FF0000,         0xFF000000,         0x000000FF,         PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormat_BGRA8_SRGB]      = PixelFormatDescription("BGRA8_SRGB",      PixelFormatContent_ColorRGBA,    0x0000FF00,         0x00FF0000,         0xFF000000,         0x000000FF,         PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormat_DXT1]            = PixelFormatDescription("DXT1",            PixelFormatContent_ColorRGBA,    8,                                                                              PixelFormatSubType_Compressed);
		s_pixelFormatInfos[PixelFormat_DXT3]            = PixelFormatDescription("DXT3",            PixelFormatContent_ColorRGBA,    16,                                                                             PixelFormatSubType_Compressed);
		s_pixelFormatInfos[PixelFormat_DXT5]            = PixelFormatDescription("DXT5",            PixelFormatContent_ColorRGBA,    16,                                                                             PixelFormatSubType_Compressed);
		s_pixelFormatInfos[PixelFormat_L8]              = PixelFormatDescription("L8",              PixelFormatContent_ColorRGBA,    0xFF,               0xFF,               0xFF,               0,                  PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormat_LA8]             = PixelFormatDescription("LA8",             PixelFormatContent_ColorRGBA,    0xFF00,             0xFF00,             0xFF00,             0x00FF,             PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormat_R8]              = PixelFormatDescription("R8",              PixelFormatContent_ColorRGBA,    0xFF,               0,                  0,                  0,                  PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormat_R8I]             = PixelFormatDescription("R8I",             PixelFormatContent_ColorRGBA,    0xFF,               0,                  0,                  0,                  PixelFormatSubType_Int);
		s_pixelFormatInfos[PixelFormat_R8UI]            = PixelFormatDescription("R8UI",            PixelFormatContent_ColorRGBA,    0xFF,               0,                  0,                  0,                  PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormat_R16]             = PixelFormatDescription("R16",             PixelFormatContent_ColorRGBA,    0xFFFF,             0,                  0,                  0,                  PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormat_R16F]            = PixelFormatDescription("R16F",            PixelFormatContent_ColorRGBA,    0xFFFF,             0,                  0,                  0,                  PixelFormatSubType_Half);
		s_pixelFormatInfos[PixelFormat_R16I]            = PixelFormatDescription("R16I",            PixelFormatContent_ColorRGBA,    0xFFFF,             0,                  0,                  0,                  PixelFormatSubType_Int);
		s_pixelFormatInfos[PixelFormat_R16UI]           = PixelFormatDescription("R16UI",           PixelFormatContent_ColorRGBA,    0xFFFF,             0,                  0,                  0,                  PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormat_R32F]            = PixelFormatDescription("R32F",            PixelFormatContent_ColorRGBA,    0xFFFFFFFF,         0,                  0,                  0,                  PixelFormatSubType_Float);
		s_pixelFormatInfos[PixelFormat_R32I]            = PixelFormatDescription("R32I",            PixelFormatContent_ColorRGBA,    0xFFFFFFFF,         0,                  0,                  0,                  PixelFormatSubType_Int);
		s_pixelFormatInfos[PixelFormat_R32UI]           = PixelFormatDescription("R32UI",           PixelFormatContent_ColorRGBA,    0xFFFFFFFF,         0,                  0,                  0,                  PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormat_RG8]             = PixelFormatDescription("RG8",             PixelFormatContent_ColorRGBA,    0xFF00,             0x00FF,             0,                  0,                  PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormat_RG8I]            = PixelFormatDescription("RG8I",            PixelFormatContent_ColorRGBA,    0xFF00,             0x00FF,             0,                  0,                  PixelFormatSubType_Int);
		s_pixelFormatInfos[PixelFormat_RG8UI]           = PixelFormatDescription("RG8UI",           PixelFormatContent_ColorRGBA,    0xFF00,             0x00FF,             0,                  0,                  PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormat_RG16]            = PixelFormatDescription("RG16",            PixelFormatContent_ColorRGBA,    0xFFFF0000,         0x0000FFFF,         0,                  0,                  PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormat_RG16F]           = PixelFormatDescription("RG16F",           PixelFormatContent_ColorRGBA,    0xFFFF0000,         0x0000FFFF,         0,                  0,                  PixelFormatSubType_Half);
		s_pixelFormatInfos[PixelFormat_RG16I]           = PixelFormatDescription("RG16I",           PixelFormatContent_ColorRGBA,    0xFFFF0000,         0x0000FFFF,         0,                  0,                  PixelFormatSubType_Int);
		s_pixelFormatInfos[PixelFormat_RG16UI]          = PixelFormatDescription("RG16UI",          PixelFormatContent_ColorRGBA,    0xFFFF0000,         0x0000FFFF,         0,                  0,                  PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormat_RG32F]           = PixelFormatDescription("RG32F",           PixelFormatContent_ColorRGBA,    0xFFFFFFFF00000000, 0x00000000FFFFFFFF, 0,                  0,                  PixelFormatSubType_Float);
		s_pixelFormatInfos[PixelFormat_RG32I]           = PixelFormatDescription("RG32I",           PixelFormatContent_ColorRGBA,    0xFFFFFFFF00000000, 0x00000000FFFFFFFF, 0,                  0,                  PixelFormatSubType_Int);
		s_pixelFormatInfos[PixelFormat_RG32UI]          = PixelFormatDescription("RG32UI",          PixelFormatContent_ColorRGBA,    0xFFFFFFFF00000000, 0x00000000FFFFFFFF, 0,                  0,                  PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormat_RGB8]            = PixelFormatDescription("RGB8",            PixelFormatContent_ColorRGBA,    0xFF000000,         0x00FF0000,         0x0000FF00,         0,                  PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormat_RGB8_SRGB]       = PixelFormatDescription("RGB8_SRGB",       PixelFormatContent_ColorRGBA,    0xFF000000,         0x00FF0000,         0x0000FF00,         0,                  PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormat_RGB16F]          = PixelFormatDescription("RGB16F",          PixelFormatContent_ColorRGBA,    0xFFFF00000000,     0x0000FFFF0000,     0x00000000FFFF,     0,                  PixelFormatSubType_Half);
		s_pixelFormatInfos[PixelFormat_RGB16I]          = PixelFormatDescription("RGB16I",          PixelFormatContent_ColorRGBA,    0xFFFF00000000,     0x0000FFFF0000,     0x00000000FFFF,     0,                  PixelFormatSubType_Int);
		s_pixelFormatInfos[PixelFormat_RGB16UI]         = PixelFormatDescription("RGB16UI",         PixelFormatContent_ColorRGBA,    0xFFFF000000000000, 0x0000FFFF00000000, 0x00000000FFFF0000, 0,                  PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormat_RGB32F]          = PixelFormatDescription("RGB32F",          PixelFormatContent_ColorRGBA,    b32,                b32 >> 32,          b32 >> 64,          0,                  PixelFormatSubType_Float);
		s_pixelFormatInfos[PixelFormat_RGB32I]          = PixelFormatDescription("RGB32I",          PixelFormatContent_ColorRGBA,    b32,                b32 >> 32,          b32 >> 64,          0,                  PixelFormatSubType_Int);
		s_pixelFormatInfos[PixelFormat_RGB32UI]         = PixelFormatDescription("RGB32UI",         PixelFormatContent_ColorRGBA,    b32,                b32 >> 32,          b32 >> 64,          0,                  PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormat_RGBA4]           = PixelFormatDescription("RGBA4",           PixelFormatContent_ColorRGBA,    0xF000,             0x0F00,             0x00F0,             0x000F,             PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormat_RGB5A1]          = PixelFormatDescription("RGB5A1",          PixelFormatContent_ColorRGBA,    0xF800,             0x07C0,             0x003E,             0x0001,             PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormat_RGBA8]           = PixelFormatDescription("RGBA8",           PixelFormatContent_ColorRGBA,    0xFF000000,         0x00FF0000,         0x0000FF00,         0x000000FF,         PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormat_RGBA8_SRGB]      = PixelFormatDescription("RGBA8_SRGB",      PixelFormatContent_ColorRGBA,    0xFF000000,         0x00FF0000,         0x0000FF00,         0x000000FF,         PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormat_RGBA16F]         = PixelFormatDescription("RGBA16F",         PixelFormatContent_ColorRGBA,    0xFFFF000000000000, 0x0000FFFF00000000, 0x00000000FFFF0000, 0x000000000000FFFF, PixelFormatSubType_Half);
		s_pixelFormatInfos[PixelFormat_RGBA16I]         = PixelFormatDescription("RGBA16I",         PixelFormatContent_ColorRGBA,    0xFFFF000000000000, 0x0000FFFF00000000, 0x00000000FFFF0000, 0x000000000000FFFF, PixelFormatSubType_Int);
		s_pixelFormatInfos[PixelFormat_RGBA16UI]        = PixelFormatDescription("RGBA16UI",        PixelFormatContent_ColorRGBA,    0xFFFF000000000000, 0x0000FFFF00000000, 0x00000000FFFF0000, 0x000000000000FFFF, PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormat_RGBA32F]         = PixelFormatDescription("RGBA32F",         PixelFormatContent_ColorRGBA,    b32,                b32 >> 32,          b32 >> 64,          b32 >> 96,          PixelFormatSubType_Float);
		s_pixelFormatInfos[PixelFormat_RGBA32I]         = PixelFormatDescription("RGBA32I",         PixelFormatContent_ColorRGBA,    b32,                b32 >> 32,          b32 >> 64,          b32 >> 96,          PixelFormatSubType_Int);
		s_pixelFormatInfos[PixelFormat_RGBA32UI]        = PixelFormatDescription("RGBA32UI",        PixelFormatContent_ColorRGBA,    b32,                b32 >> 32,          b32 >> 64,          b32 >> 96,          PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormat_Depth16]         = PixelFormatDescription("Depth16",         PixelFormatContent_Depth,        0xFFFF,             0,                  0,                  0,                  PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormat_Depth24]         = PixelFormatDescription("Depth24",         PixelFormatContent_Depth,        0xFFFFFF,           0,                  0,                  0,                  PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormat_Depth24Stencil8] = PixelFormatDescription("Depth24Stencil8", PixelFormatContent_DepthStencil, 0xFFFFFF00,         0x000000FF,         0,                  0,                  PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormat_Depth32]         = PixelFormatDescription("Depth32",         PixelFormatContent_Depth,        0xFFFFFFFF,         0,                  0,                  0,                  PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormat_Stencil1]        = PixelFormatDescription("Stencil1",        PixelFormatContent_Stencil,      0x1,                0,                  0,                  0,                  PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormat_Stencil4]        = PixelFormatDescription("Stencil4",        PixelFormatContent_Stencil,      0xF,                0,                  0,                  0,                  PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormat_Stencil8]        = PixelFormatDescription("Stencil8",        PixelFormatContent_Stencil,      0xFF,               0,                  0,                  0,                  PixelFormatSubType_Unsigned);
		s_pixelFormatInfos[PixelFormat_Stencil16]       = PixelFormatDescription("Stencil16",       PixelFormatContent_Stencil,      0xFFFF,             0,                  0,                  0,                  PixelFormatSubType_Unsigned);

		for (unsigned int i = 0; i <= PixelFormat_Max; ++i)
		{
			if (!s_pixelFormatInfos[i].Validate())
				NazaraWarning("Pixel format 0x" + NumberToString(i, 16) + " (" + GetName(static_cast<Nz::PixelFormat>(i)) + ") failed validation tests");
		}

		// Reset functions
		std::memset(s_convertFunctions, 0, (PixelFormat_Max+1)*(PixelFormat_Max+1)*sizeof(PixelFormatInfo::ConvertFunction));

		/***********************************A8************************************/
		RegisterConverter<PixelFormat_A8, PixelFormat_BGRA8>();
		RegisterConverter<PixelFormat_A8, PixelFormat_LA8>();
		RegisterConverter<PixelFormat_A8, PixelFormat_RGB5A1>();
		RegisterConverter<PixelFormat_A8, PixelFormat_RGBA4>();
		RegisterConverter<PixelFormat_A8, PixelFormat_RGBA8>();

		/**********************************BGR8***********************************/
		RegisterConverter<PixelFormat_BGR8, PixelFormat_BGR8_SRGB>();
		RegisterConverter<PixelFormat_BGR8, PixelFormat_BGRA8>();
		RegisterConverter<PixelFormat_BGR8, PixelFormat_L8>();
		RegisterConverter<PixelFormat_BGR8, PixelFormat_LA8>();/*
		RegisterConverter<PixelFormat_BGR8, PixelFormat_RGB16F>();
		RegisterConverter<PixelFormat_BGR8, PixelFormat_RGB16I>();
		RegisterConverter<PixelFormat_BGR8, PixelFormat_RGB32F>();
		RegisterConverter<PixelFormat_BGR8, PixelFormat_RGB32I>();
		RegisterConverter<PixelFormat_BGR8, PixelFormat_RGBA16F>();
		RegisterConverter<PixelFormat_BGR8, PixelFormat_RGBA16I>();
		RegisterConverter<PixelFormat_BGR8, PixelFormat_RGBA32F>();
		RegisterConverter<PixelFormat_BGR8, PixelFormat_RGBA32I>();*/
		RegisterConverter<PixelFormat_BGR8, PixelFormat_RGB5A1>();
		RegisterConverter<PixelFormat_BGR8, PixelFormat_RGB8>();
		RegisterConverter<PixelFormat_BGR8, PixelFormat_RGBA4>();
		RegisterConverter<PixelFormat_BGR8, PixelFormat_RGBA8>();

		/**********************************BGRA8**********************************/
		RegisterConverter<PixelFormat_BGRA8, PixelFormat_A8>();
		RegisterConverter<PixelFormat_BGRA8, PixelFormat_BGR8>();
		RegisterConverter<PixelFormat_BGRA8, PixelFormat_BGRA8_SRGB>();
		RegisterConverter<PixelFormat_BGRA8, PixelFormat_L8>();
		RegisterConverter<PixelFormat_BGRA8, PixelFormat_LA8>();/*
		RegisterConverter<PixelFormat_BGRA8, PixelFormat_RGB16F>();
		RegisterConverter<PixelFormat_BGRA8, PixelFormat_RGB16I>();
		RegisterConverter<PixelFormat_BGRA8, PixelFormat_RGB32F>();
		RegisterConverter<PixelFormat_BGRA8, PixelFormat_RGB32I>();
		RegisterConverter<PixelFormat_BGRA8, PixelFormat_RGBA16F>();
		RegisterConverter<PixelFormat_BGRA8, PixelFormat_RGBA16I>();
		RegisterConverter<PixelFormat_BGRA8, PixelFormat_RGBA32F>();
		RegisterConverter<PixelFormat_BGRA8, PixelFormat_RGBA32I>();*/
		RegisterConverter<PixelFormat_BGRA8, PixelFormat_RGB5A1>();
		RegisterConverter<PixelFormat_BGRA8, PixelFormat_RGB8>();
		RegisterConverter<PixelFormat_BGRA8, PixelFormat_RGBA4>();
		RegisterConverter<PixelFormat_BGRA8, PixelFormat_RGBA8>();

		/**********************************DXT1***********************************/
		///TODO: Décompresseur DXT1
	/*
		RegisterConverter<PixelFormat_DXT1, PixelFormat_BGR8>();
		RegisterConverter<PixelFormat_DXT1, PixelFormat_BGRA8>();
		RegisterConverter<PixelFormat_DXT1, PixelFormat_DXT3>();
		RegisterConverter<PixelFormat_DXT1, PixelFormat_DXT5>();
		RegisterConverter<PixelFormat_DXT1, PixelFormat_L8>();
		RegisterConverter<PixelFormat_DXT1, PixelFormat_LA8>();
		RegisterConverter<PixelFormat_DXT1, PixelFormat_RGB16F>();
		RegisterConverter<PixelFormat_DXT1, PixelFormat_RGB16I>();
		RegisterConverter<PixelFormat_DXT1, PixelFormat_RGB32F>();
		RegisterConverter<PixelFormat_DXT1, PixelFormat_RGB32I>();
		RegisterConverter<PixelFormat_DXT1, PixelFormat_RGBA16F>();
		RegisterConverter<PixelFormat_DXT1, PixelFormat_RGBA16I>();
		RegisterConverter<PixelFormat_DXT1, PixelFormat_RGBA32F>();
		RegisterConverter<PixelFormat_DXT1, PixelFormat_RGBA32I>();
		RegisterConverter<PixelFormat_DXT1, PixelFormat_RGB5A1>();
		RegisterConverter<PixelFormat_DXT1, PixelFormat_RGB8>();
		RegisterConverter<PixelFormat_DXT1, PixelFormat_RGBA4>();
		RegisterConverter<PixelFormat_DXT1, PixelFormat_RGBA8>();
	*/

		/**********************************DXT3***********************************/
		///TODO: Décompresseur DXT3
	/*
		RegisterConverter<PixelFormat_DXT3, PixelFormat_BGR8>();
		RegisterConverter<PixelFormat_DXT3, PixelFormat_BGRA8>();
		RegisterConverter<PixelFormat_DXT3, PixelFormat_DXT1>();
		RegisterConverter<PixelFormat_DXT3, PixelFormat_DXT5>();
		RegisterConverter<PixelFormat_DXT3, PixelFormat_L8>();
		RegisterConverter<PixelFormat_DXT3, PixelFormat_LA8>();
		RegisterConverter<PixelFormat_DXT3, PixelFormat_RGB16F>();
		RegisterConverter<PixelFormat_DXT3, PixelFormat_RGB16I>();
		RegisterConverter<PixelFormat_DXT3, PixelFormat_RGB32F>();
		RegisterConverter<PixelFormat_DXT3, PixelFormat_RGB32I>();
		RegisterConverter<PixelFormat_DXT3, PixelFormat_RGBA16F>();
		RegisterConverter<PixelFormat_DXT3, PixelFormat_RGBA16I>();
		RegisterConverter<PixelFormat_DXT3, PixelFormat_RGBA32F>();
		RegisterConverter<PixelFormat_DXT3, PixelFormat_RGBA32I>();
		RegisterConverter<PixelFormat_DXT3, PixelFormat_RGB5A1>();
		RegisterConverter<PixelFormat_DXT3, PixelFormat_RGB8>();
		RegisterConverter<PixelFormat_DXT3, PixelFormat_RGBA4>();
		RegisterConverter<PixelFormat_DXT3, PixelFormat_RGBA8>();
	*/

		/**********************************DXT5***********************************/
		///TODO: Décompresseur DXT5
	/*
		RegisterConverter<PixelFormat_DXT5, PixelFormat_BGR8>();
		RegisterConverter<PixelFormat_DXT5, PixelFormat_BGRA8>();
		RegisterConverter<PixelFormat_DXT5, PixelFormat_DXT1>();
		RegisterConverter<PixelFormat_DXT5, PixelFormat_DXT3>();
		RegisterConverter<PixelFormat_DXT5, PixelFormat_L8>();
		RegisterConverter<PixelFormat_DXT5, PixelFormat_LA8>();
		RegisterConverter<PixelFormat_DXT5, PixelFormat_RGB16F>();
		RegisterConverter<PixelFormat_DXT5, PixelFormat_RGB16I>();
		RegisterConverter<PixelFormat_DXT5, PixelFormat_RGB32F>();
		RegisterConverter<PixelFormat_DXT5, PixelFormat_RGB32I>();
		RegisterConverter<PixelFormat_DXT5, PixelFormat_RGBA16F>();
		RegisterConverter<PixelFormat_DXT5, PixelFormat_RGBA16I>();
		RegisterConverter<PixelFormat_DXT5, PixelFormat_RGBA32F>();
		RegisterConverter<PixelFormat_DXT5, PixelFormat_RGBA32I>();
		RegisterConverter<PixelFormat_DXT5, PixelFormat_RGB5A1>();
		RegisterConverter<PixelFormat_DXT5, PixelFormat_RGB8>();
		RegisterConverter<PixelFormat_DXT5, PixelFormat_RGBA4>();
		RegisterConverter<PixelFormat_DXT5, PixelFormat_RGBA8>();
	*/

		/***********************************L8************************************/
		RegisterConverter<PixelFormat_L8, PixelFormat_BGR8>();
		RegisterConverter<PixelFormat_L8, PixelFormat_BGRA8>();
		RegisterConverter<PixelFormat_L8, PixelFormat_LA8>();/*
		RegisterConverter<PixelFormat_L8, PixelFormat_RGB16F>();
		RegisterConverter<PixelFormat_L8, PixelFormat_RGB16I>();
		RegisterConverter<PixelFormat_L8, PixelFormat_RGB32F>();
		RegisterConverter<PixelFormat_L8, PixelFormat_RGB32I>();
		RegisterConverter<PixelFormat_L8, PixelFormat_RGBA16F>();
		RegisterConverter<PixelFormat_L8, PixelFormat_RGBA16I>();
		RegisterConverter<PixelFormat_L8, PixelFormat_RGBA32F>();
		RegisterConverter<PixelFormat_L8, PixelFormat_RGBA32I>();*/
		RegisterConverter<PixelFormat_L8, PixelFormat_RGB5A1>();
		RegisterConverter<PixelFormat_L8, PixelFormat_RGB8>();
		RegisterConverter<PixelFormat_L8, PixelFormat_RGBA4>();
		RegisterConverter<PixelFormat_L8, PixelFormat_RGBA8>();

		/***********************************LA8***********************************/
		RegisterConverter<PixelFormat_LA8, PixelFormat_A8>();
		RegisterConverter<PixelFormat_LA8, PixelFormat_BGR8>();
		RegisterConverter<PixelFormat_LA8, PixelFormat_BGRA8>();
		RegisterConverter<PixelFormat_LA8, PixelFormat_L8>();/*
		RegisterConverter<PixelFormat_LA8, PixelFormat_RGB16F>();
		RegisterConverter<PixelFormat_LA8, PixelFormat_RGB16I>();
		RegisterConverter<PixelFormat_LA8, PixelFormat_RGB32F>();
		RegisterConverter<PixelFormat_LA8, PixelFormat_RGB32I>();
		RegisterConverter<PixelFormat_LA8, PixelFormat_RGBA16F>();
		RegisterConverter<PixelFormat_LA8, PixelFormat_RGBA16I>();
		RegisterConverter<PixelFormat_LA8, PixelFormat_RGBA32F>();
		RegisterConverter<PixelFormat_LA8, PixelFormat_RGBA32I>();*/
		RegisterConverter<PixelFormat_LA8, PixelFormat_RGB5A1>();
		RegisterConverter<PixelFormat_LA8, PixelFormat_RGB8>();
		RegisterConverter<PixelFormat_LA8, PixelFormat_RGBA4>();
		RegisterConverter<PixelFormat_LA8, PixelFormat_RGBA8>();

		/**********************************RGBA4**********************************/
		RegisterConverter<PixelFormat_RGBA4, PixelFormat_A8>();
		RegisterConverter<PixelFormat_RGBA4, PixelFormat_BGR8>();
		RegisterConverter<PixelFormat_RGBA4, PixelFormat_BGRA8>();
		RegisterConverter<PixelFormat_RGBA4, PixelFormat_L8>();
		RegisterConverter<PixelFormat_RGBA4, PixelFormat_LA8>();/*
		RegisterConverter<PixelFormat_RGBA4, PixelFormat_RGB16F>();
		RegisterConverter<PixelFormat_RGBA4, PixelFormat_RGB16I>();
		RegisterConverter<PixelFormat_RGBA4, PixelFormat_RGB32F>();
		RegisterConverter<PixelFormat_RGBA4, PixelFormat_RGB32I>();
		RegisterConverter<PixelFormat_RGBA4, PixelFormat_RGBA16F>();
		RegisterConverter<PixelFormat_RGBA4, PixelFormat_RGBA16I>();
		RegisterConverter<PixelFormat_RGBA4, PixelFormat_RGBA32F>();
		RegisterConverter<PixelFormat_RGBA4, PixelFormat_RGBA32I>();*/
		RegisterConverter<PixelFormat_RGBA4, PixelFormat_RGB5A1>();
		RegisterConverter<PixelFormat_RGBA4, PixelFormat_RGB8>();
		RegisterConverter<PixelFormat_RGBA4, PixelFormat_RGBA8>();

		/*********************************RGB5A1**********************************/
		RegisterConverter<PixelFormat_RGB5A1, PixelFormat_A8>();
		RegisterConverter<PixelFormat_RGB5A1, PixelFormat_BGR8>();
		RegisterConverter<PixelFormat_RGB5A1, PixelFormat_BGRA8>();
		RegisterConverter<PixelFormat_RGB5A1, PixelFormat_L8>();
		RegisterConverter<PixelFormat_RGB5A1, PixelFormat_LA8>();/*
		RegisterConverter<PixelFormat_RGB5A1, PixelFormat_RGB16F>();
		RegisterConverter<PixelFormat_RGB5A1, PixelFormat_RGB16I>();
		RegisterConverter<PixelFormat_RGB5A1, PixelFormat_RGB32F>();
		RegisterConverter<PixelFormat_RGB5A1, PixelFormat_RGB32I>();
		RegisterConverter<PixelFormat_RGB5A1, PixelFormat_RGBA16F>();
		RegisterConverter<PixelFormat_RGB5A1, PixelFormat_RGBA16I>();
		RegisterConverter<PixelFormat_RGB5A1, PixelFormat_RGBA32F>();
		RegisterConverter<PixelFormat_RGB5A1, PixelFormat_RGBA32I>();*/
		RegisterConverter<PixelFormat_RGB5A1, PixelFormat_RGB8>();
		RegisterConverter<PixelFormat_RGB5A1, PixelFormat_RGBA4>();
		RegisterConverter<PixelFormat_RGB5A1, PixelFormat_RGBA8>();

		/**********************************RGB8***********************************/
		RegisterConverter<PixelFormat_RGB8, PixelFormat_BGR8>();
		RegisterConverter<PixelFormat_RGB8, PixelFormat_BGRA8>();
		RegisterConverter<PixelFormat_RGB8, PixelFormat_L8>();
		RegisterConverter<PixelFormat_RGB8, PixelFormat_LA8>();/*
		RegisterConverter<PixelFormat_RGB8, PixelFormat_RGB16F>();
		RegisterConverter<PixelFormat_RGB8, PixelFormat_RGB16I>();
		RegisterConverter<PixelFormat_RGB8, PixelFormat_RGB32F>();
		RegisterConverter<PixelFormat_RGB8, PixelFormat_RGB32I>();
		RegisterConverter<PixelFormat_RGB8, PixelFormat_RGBA16F>();
		RegisterConverter<PixelFormat_RGB8, PixelFormat_RGBA16I>();
		RegisterConverter<PixelFormat_RGB8, PixelFormat_RGBA32F>();
		RegisterConverter<PixelFormat_RGB8, PixelFormat_RGBA32I>();*/
		RegisterConverter<PixelFormat_RGB8, PixelFormat_RGB5A1>();
		RegisterConverter<PixelFormat_RGB8, PixelFormat_RGB8_SRGB>();
		RegisterConverter<PixelFormat_RGB8, PixelFormat_RGBA4>();
		RegisterConverter<PixelFormat_RGB8, PixelFormat_RGBA8>();

		/**********************************RGBA8**********************************/
		RegisterConverter<PixelFormat_RGBA8, PixelFormat_A8>();
		RegisterConverter<PixelFormat_RGBA8, PixelFormat_BGR8>();
		RegisterConverter<PixelFormat_RGBA8, PixelFormat_BGRA8>();
		RegisterConverter<PixelFormat_RGBA8, PixelFormat_L8>();
		RegisterConverter<PixelFormat_RGBA8, PixelFormat_LA8>();/*
		RegisterConverter<PixelFormat_RGBA8, PixelFormat_RGB16F>();
		RegisterConverter<PixelFormat_RGBA8, PixelFormat_RGB16I>();
		RegisterConverter<PixelFormat_RGBA8, PixelFormat_RGB32F>();
		RegisterConverter<PixelFormat_RGBA8, PixelFormat_RGB32I>();
		RegisterConverter<PixelFormat_RGBA8, PixelFormat_RGBA16F>();
		RegisterConverter<PixelFormat_RGBA8, PixelFormat_RGBA16I>();
		RegisterConverter<PixelFormat_RGBA8, PixelFormat_RGBA32F>();
		RegisterConverter<PixelFormat_RGBA8, PixelFormat_RGBA32I>();*/
		RegisterConverter<PixelFormat_RGBA8, PixelFormat_RGB5A1>();
		RegisterConverter<PixelFormat_RGBA8, PixelFormat_RGB8>();
		RegisterConverter<PixelFormat_RGBA8, PixelFormat_RGBA4>();
		RegisterConverter<PixelFormat_RGBA8, PixelFormat_RGBA8_SRGB>();

		return true;
	}

	void PixelFormatInfo::Uninitialize()
	{
		for (unsigned int i = 0; i <= PixelFormat_Max; ++i)
			s_pixelFormatInfos[i].Clear();

		std::memset(s_convertFunctions, 0, (PixelFormat_Max+1)*(PixelFormat_Max+1)*sizeof(PixelFormatInfo::ConvertFunction));

		for (unsigned int i = 0; i <= PixelFlipping_Max; ++i)
			s_flipFunctions[i].clear();
	}

	PixelFormatDescription PixelFormatInfo::s_pixelFormatInfos[PixelFormat_Max + 1];
	PixelFormatInfo::ConvertFunction PixelFormatInfo::s_convertFunctions[PixelFormat_Max+1][PixelFormat_Max+1];
	std::map<PixelFormat, PixelFormatInfo::FlipFunction> PixelFormatInfo::s_flipFunctions[PixelFlipping_Max+1];
}
