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
		UInt8* ConvertPixels<PixelFormat::A8, PixelFormat::BGRA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::A8, PixelFormat::LA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::A8, PixelFormat::RGB5A1>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::A8, PixelFormat::RGBA4>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::A8, PixelFormat::RGBA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::BGR8, PixelFormat::BGRA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::BGR8, PixelFormat::BGR8_SRGB>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			//FIXME: Not correct
			std::size_t count = end - start;
			std::memcpy(dst, start, count);
			return dst + count;
		}

		template<>
		UInt8* ConvertPixels<PixelFormat::BGR8, PixelFormat::L8>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			while (start < end)
			{
				*dst++ = static_cast<UInt8>(start[2] * 0.3f + start[1] * 0.59f + start[0] * 0.11f);

				start += 3;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormat::BGR8, PixelFormat::LA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::BGR8, PixelFormat::RGB5A1>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::BGR8, PixelFormat::RGB8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::BGR8, PixelFormat::RGBA4>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::BGR8, PixelFormat::RGBA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::BGRA8, PixelFormat::A8>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			while (start < end)
			{
				*dst++ = start[3];

				start += 4;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormat::BGRA8, PixelFormat::BGR8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::BGRA8, PixelFormat::BGRA8_SRGB>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			//FIXME: Not correct
			std::size_t count = end - start;
			std::memcpy(dst, start, count);
			return dst + count;
		}

		template<>
		UInt8* ConvertPixels<PixelFormat::BGRA8, PixelFormat::L8>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			while (start < end)
			{
				*dst++ = static_cast<UInt8>(start[2] * 0.3f + start[1] * 0.59f + start[0] * 0.11f);

				start += 4;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormat::BGRA8, PixelFormat::LA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::BGRA8, PixelFormat::RGBA4>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::BGRA8, PixelFormat::RGB5A1>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::BGRA8, PixelFormat::RGB8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::BGRA8, PixelFormat::RGBA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::L8, PixelFormat::BGR8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::L8, PixelFormat::BGRA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::L8, PixelFormat::LA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::L8, PixelFormat::RGB5A1>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::L8, PixelFormat::RGB8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::L8, PixelFormat::RGBA4>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::L8, PixelFormat::RGBA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::LA8, PixelFormat::A8>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			while (start < end)
			{
				*dst++ = start[1];

				start += 2;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormat::LA8, PixelFormat::BGR8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::LA8, PixelFormat::BGRA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::LA8, PixelFormat::L8>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			while (start < end)
			{
				*dst++ = start[0];

				start += 2;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormat::LA8, PixelFormat::RGB5A1>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::LA8, PixelFormat::RGB8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::LA8, PixelFormat::RGBA4>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::LA8, PixelFormat::RGBA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::RGBA4, PixelFormat::A8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::RGBA4, PixelFormat::BGR8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::RGBA4, PixelFormat::BGRA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::RGBA4, PixelFormat::L8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::RGBA4, PixelFormat::LA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::RGBA4, PixelFormat::RGB5A1>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::RGBA4, PixelFormat::RGB8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::RGBA4, PixelFormat::RGBA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::RGB5A1, PixelFormat::A8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::RGB5A1, PixelFormat::BGR8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::RGB5A1, PixelFormat::BGRA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::RGB5A1, PixelFormat::L8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::RGB5A1, PixelFormat::LA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::RGB5A1, PixelFormat::RGB8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::RGB5A1, PixelFormat::RGBA4>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::RGB5A1, PixelFormat::RGBA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::RGB8, PixelFormat::BGR8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::RGB8, PixelFormat::BGRA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::RGB8, PixelFormat::L8>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			while (start < end)
			{
				*dst++ = static_cast<UInt8>(start[0] * 0.3f + start[1] * 0.59f + start[2] * 0.11f);

				start += 3;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormat::RGB8, PixelFormat::LA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::RGB8, PixelFormat::RGB5A1>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::RGB8, PixelFormat::RGBA4>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::RGB8, PixelFormat::RGB8_SRGB>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			//FIXME: Not correct
			std::size_t count = end - start;
			std::memcpy(dst, start, count);
			return dst + count;
		}

		template<>
		UInt8* ConvertPixels<PixelFormat::RGB8, PixelFormat::RGBA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::RGBA8, PixelFormat::A8>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			while (start < end)
			{
				*dst++ = start[3];

				start += 4;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormat::RGBA8, PixelFormat::BGR8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::RGBA8, PixelFormat::BGRA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::RGBA8, PixelFormat::L8>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			while (start < end)
			{
				*dst++ = static_cast<UInt8>(start[0] * 0.3f + start[1] * 0.59f + start[2] * 0.11f);

				start += 4;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormat::RGBA8, PixelFormat::LA8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::RGBA8, PixelFormat::RGB5A1>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::RGBA8, PixelFormat::RGB8>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::RGBA8, PixelFormat::RGBA4>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		UInt8* ConvertPixels<PixelFormat::RGBA8, PixelFormat::RGBA8_SRGB>(const UInt8* start, const UInt8* end, UInt8* dst)
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
		NazaraAssert(IsValid(format), "invalid pixel format");

		auto& flipFunction = s_flipFunctions[UnderlyingCast(format)][UnderlyingCast(flipping)];
		if (flipFunction)
			flipFunction(width, height, depth, reinterpret_cast<const UInt8*>(src), reinterpret_cast<UInt8*>(dst));
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
				case PixelFlipping::Horizontally:
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

				case PixelFlipping::Vertically:
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
		for (unsigned int i = 0; i < PixelFormatCount; ++i)
		{
			PixelFormatDescription& info2 = s_pixelFormatInfos[i];
			if (info.bitsPerPixel == info2.bitsPerPixel && info.content == info2.content &&
			    info.redMask == info2.redMask && info.greenMask == info2.greenMask && info.blueMask == info2.blueMask && info.alphaMask == info2.alphaMask &&
			    info.redType == info2.redType && info.greenType == info2.greenType && info.blueType == info2.blueType && info.alphaType == info2.alphaType)
				return static_cast<PixelFormat>(i);
		}

		return PixelFormat::Undefined;
	}

	bool PixelFormatInfo::Initialize()
	{
		auto SetupPixelFormat = [](PixelFormat format, PixelFormatDescription&& desc)
		{
			s_pixelFormatInfos[UnderlyingCast(format)] = std::move(desc);
		};

		Bitset<> b8(0xFF);
		b8.Resize(128);

		Bitset<> b32(0xFFFFFFFF);
		b32.Resize(128);

		// Setup informations about every pixel format
		SetupPixelFormat(PixelFormat::A8,               PixelFormatDescription("A8",               PixelFormatContent::ColorRGBA,    0,                  0,                  0,                  0xFF,               PixelFormatSubType::Unsigned));
		SetupPixelFormat(PixelFormat::BGR8,             PixelFormatDescription("BGR8",             PixelFormatContent::ColorRGBA,    0x0000FF,           0x00FF00,           0xFF0000,           0,                  PixelFormatSubType::Unsigned));
		SetupPixelFormat(PixelFormat::BGR8_SRGB,        PixelFormatDescription("BGR8_SRGB",        PixelFormatContent::ColorRGBA,    0x0000FF,           0x00FF00,           0xFF0000,           0,                  PixelFormatSubType::Unsigned));
		SetupPixelFormat(PixelFormat::BGRA8,            PixelFormatDescription("BGRA8",            PixelFormatContent::ColorRGBA,    0x0000FF00,         0x00FF0000,         0xFF000000,         0x000000FF,         PixelFormatSubType::Unsigned));
		SetupPixelFormat(PixelFormat::BGRA8_SRGB,       PixelFormatDescription("BGRA8_SRGB",       PixelFormatContent::ColorRGBA,    0x0000FF00,         0x00FF0000,         0xFF000000,         0x000000FF,         PixelFormatSubType::Unsigned));
		SetupPixelFormat(PixelFormat::DXT1,             PixelFormatDescription("DXT1",             PixelFormatContent::ColorRGBA,    8,                                                                              PixelFormatSubType::Compressed));
		SetupPixelFormat(PixelFormat::DXT3,             PixelFormatDescription("DXT3",             PixelFormatContent::ColorRGBA,    16,                                                                             PixelFormatSubType::Compressed));
		SetupPixelFormat(PixelFormat::DXT5,             PixelFormatDescription("DXT5",             PixelFormatContent::ColorRGBA,    16,                                                                             PixelFormatSubType::Compressed));
		SetupPixelFormat(PixelFormat::L8,               PixelFormatDescription("L8",               PixelFormatContent::ColorRGBA,    0xFF,               0xFF,               0xFF,               0,                  PixelFormatSubType::Unsigned));
		SetupPixelFormat(PixelFormat::LA8,              PixelFormatDescription("LA8",              PixelFormatContent::ColorRGBA,    0xFF00,             0xFF00,             0xFF00,             0x00FF,             PixelFormatSubType::Unsigned));
		SetupPixelFormat(PixelFormat::R8,               PixelFormatDescription("R8",               PixelFormatContent::ColorRGBA,    0xFF,               0,                  0,                  0,                  PixelFormatSubType::Unsigned));
		SetupPixelFormat(PixelFormat::R8I,              PixelFormatDescription("R8I",              PixelFormatContent::ColorRGBA,    0xFF,               0,                  0,                  0,                  PixelFormatSubType::Int));
		SetupPixelFormat(PixelFormat::R8UI,             PixelFormatDescription("R8UI",             PixelFormatContent::ColorRGBA,    0xFF,               0,                  0,                  0,                  PixelFormatSubType::Unsigned));
		SetupPixelFormat(PixelFormat::R16,              PixelFormatDescription("R16",              PixelFormatContent::ColorRGBA,    0xFFFF,             0,                  0,                  0,                  PixelFormatSubType::Unsigned));
		SetupPixelFormat(PixelFormat::R16F,             PixelFormatDescription("R16F",             PixelFormatContent::ColorRGBA,    0xFFFF,             0,                  0,                  0,                  PixelFormatSubType::Half));
		SetupPixelFormat(PixelFormat::R16I,             PixelFormatDescription("R16I",             PixelFormatContent::ColorRGBA,    0xFFFF,             0,                  0,                  0,                  PixelFormatSubType::Int));
		SetupPixelFormat(PixelFormat::R16UI,            PixelFormatDescription("R16UI",            PixelFormatContent::ColorRGBA,    0xFFFF,             0,                  0,                  0,                  PixelFormatSubType::Unsigned));
		SetupPixelFormat(PixelFormat::R32F,             PixelFormatDescription("R32F",             PixelFormatContent::ColorRGBA,    0xFFFFFFFF,         0,                  0,                  0,                  PixelFormatSubType::Float));
		SetupPixelFormat(PixelFormat::R32I,             PixelFormatDescription("R32I",             PixelFormatContent::ColorRGBA,    0xFFFFFFFF,         0,                  0,                  0,                  PixelFormatSubType::Int));
		SetupPixelFormat(PixelFormat::R32UI,            PixelFormatDescription("R32UI",            PixelFormatContent::ColorRGBA,    0xFFFFFFFF,         0,                  0,                  0,                  PixelFormatSubType::Unsigned));
		SetupPixelFormat(PixelFormat::RG8,              PixelFormatDescription("RG8",              PixelFormatContent::ColorRGBA,    0xFF00,             0x00FF,             0,                  0,                  PixelFormatSubType::Unsigned));
		SetupPixelFormat(PixelFormat::RG8I,             PixelFormatDescription("RG8I",             PixelFormatContent::ColorRGBA,    0xFF00,             0x00FF,             0,                  0,                  PixelFormatSubType::Int));
		SetupPixelFormat(PixelFormat::RG8UI,            PixelFormatDescription("RG8UI",            PixelFormatContent::ColorRGBA,    0xFF00,             0x00FF,             0,                  0,                  PixelFormatSubType::Unsigned));
		SetupPixelFormat(PixelFormat::RG16,             PixelFormatDescription("RG16",             PixelFormatContent::ColorRGBA,    0xFFFF0000,         0x0000FFFF,         0,                  0,                  PixelFormatSubType::Unsigned));
		SetupPixelFormat(PixelFormat::RG16F,            PixelFormatDescription("RG16F",            PixelFormatContent::ColorRGBA,    0xFFFF0000,         0x0000FFFF,         0,                  0,                  PixelFormatSubType::Half));
		SetupPixelFormat(PixelFormat::RG16I,            PixelFormatDescription("RG16I",            PixelFormatContent::ColorRGBA,    0xFFFF0000,         0x0000FFFF,         0,                  0,                  PixelFormatSubType::Int));
		SetupPixelFormat(PixelFormat::RG16UI,           PixelFormatDescription("RG16UI",           PixelFormatContent::ColorRGBA,    0xFFFF0000,         0x0000FFFF,         0,                  0,                  PixelFormatSubType::Unsigned));
		SetupPixelFormat(PixelFormat::RG32F,            PixelFormatDescription("RG32F",            PixelFormatContent::ColorRGBA,    0xFFFFFFFF00000000, 0x00000000FFFFFFFF, 0,                  0,                  PixelFormatSubType::Float));
		SetupPixelFormat(PixelFormat::RG32I,            PixelFormatDescription("RG32I",            PixelFormatContent::ColorRGBA,    0xFFFFFFFF00000000, 0x00000000FFFFFFFF, 0,                  0,                  PixelFormatSubType::Int));
		SetupPixelFormat(PixelFormat::RG32UI,           PixelFormatDescription("RG32UI",           PixelFormatContent::ColorRGBA,    0xFFFFFFFF00000000, 0x00000000FFFFFFFF, 0,                  0,                  PixelFormatSubType::Unsigned));
		SetupPixelFormat(PixelFormat::RGB8,             PixelFormatDescription("RGB8",             PixelFormatContent::ColorRGBA,    0xFF000000,         0x00FF0000,         0x0000FF00,         0,                  PixelFormatSubType::Unsigned));
		SetupPixelFormat(PixelFormat::RGB8_SRGB,        PixelFormatDescription("RGB8_SRGB",        PixelFormatContent::ColorRGBA,    0xFF000000,         0x00FF0000,         0x0000FF00,         0,                  PixelFormatSubType::Unsigned));
		SetupPixelFormat(PixelFormat::RGB16F,           PixelFormatDescription("RGB16F",           PixelFormatContent::ColorRGBA,    0xFFFF00000000,     0x0000FFFF0000,     0x00000000FFFF,     0,                  PixelFormatSubType::Half));
		SetupPixelFormat(PixelFormat::RGB16I,           PixelFormatDescription("RGB16I",           PixelFormatContent::ColorRGBA,    0xFFFF00000000,     0x0000FFFF0000,     0x00000000FFFF,     0,                  PixelFormatSubType::Int));
		SetupPixelFormat(PixelFormat::RGB16UI,          PixelFormatDescription("RGB16UI",          PixelFormatContent::ColorRGBA,    0xFFFF000000000000, 0x0000FFFF00000000, 0x00000000FFFF0000, 0,                  PixelFormatSubType::Unsigned));
		SetupPixelFormat(PixelFormat::RGB32F,           PixelFormatDescription("RGB32F",           PixelFormatContent::ColorRGBA,    b32,                b32 >> 32,          b32 >> 64,          0,                  PixelFormatSubType::Float));
		SetupPixelFormat(PixelFormat::RGB32I,           PixelFormatDescription("RGB32I",           PixelFormatContent::ColorRGBA,    b32,                b32 >> 32,          b32 >> 64,          0,                  PixelFormatSubType::Int));
		SetupPixelFormat(PixelFormat::RGB32UI,          PixelFormatDescription("RGB32UI",          PixelFormatContent::ColorRGBA,    b32,                b32 >> 32,          b32 >> 64,          0,                  PixelFormatSubType::Unsigned));
		SetupPixelFormat(PixelFormat::RGBA4,            PixelFormatDescription("RGBA4",            PixelFormatContent::ColorRGBA,    0xF000,             0x0F00,             0x00F0,             0x000F,             PixelFormatSubType::Unsigned));
		SetupPixelFormat(PixelFormat::RGB5A1,           PixelFormatDescription("RGB5A1",           PixelFormatContent::ColorRGBA,    0xF800,             0x07C0,             0x003E,             0x0001,             PixelFormatSubType::Unsigned));
		SetupPixelFormat(PixelFormat::RGBA8,            PixelFormatDescription("RGBA8",            PixelFormatContent::ColorRGBA,    0xFF000000,         0x00FF0000,         0x0000FF00,         0x000000FF,         PixelFormatSubType::Unsigned));
		SetupPixelFormat(PixelFormat::RGBA8_SRGB,       PixelFormatDescription("RGBA8_SRGB",       PixelFormatContent::ColorRGBA,    0xFF000000,         0x00FF0000,         0x0000FF00,         0x000000FF,         PixelFormatSubType::Unsigned));
		SetupPixelFormat(PixelFormat::RGBA16F,          PixelFormatDescription("RGBA16F",          PixelFormatContent::ColorRGBA,    0xFFFF000000000000, 0x0000FFFF00000000, 0x00000000FFFF0000, 0x000000000000FFFF, PixelFormatSubType::Half));
		SetupPixelFormat(PixelFormat::RGBA16I,          PixelFormatDescription("RGBA16I",          PixelFormatContent::ColorRGBA,    0xFFFF000000000000, 0x0000FFFF00000000, 0x00000000FFFF0000, 0x000000000000FFFF, PixelFormatSubType::Int));
		SetupPixelFormat(PixelFormat::RGBA16UI,         PixelFormatDescription("RGBA16UI",         PixelFormatContent::ColorRGBA,    0xFFFF000000000000, 0x0000FFFF00000000, 0x00000000FFFF0000, 0x000000000000FFFF, PixelFormatSubType::Unsigned));
		SetupPixelFormat(PixelFormat::RGBA32F,          PixelFormatDescription("RGBA32F",          PixelFormatContent::ColorRGBA,    b32,                b32 >> 32,          b32 >> 64,          b32 >> 96,          PixelFormatSubType::Float));
		SetupPixelFormat(PixelFormat::RGBA32I,          PixelFormatDescription("RGBA32I",          PixelFormatContent::ColorRGBA,    b32,                b32 >> 32,          b32 >> 64,          b32 >> 96,          PixelFormatSubType::Int));
		SetupPixelFormat(PixelFormat::RGBA32UI,         PixelFormatDescription("RGBA32UI",         PixelFormatContent::ColorRGBA,    b32,                b32 >> 32,          b32 >> 64,          b32 >> 96,          PixelFormatSubType::Unsigned));
		SetupPixelFormat(PixelFormat::Depth16,          PixelFormatDescription("Depth16",          PixelFormatContent::Depth,        0xFFFF,             0,                  0,                  0,                  PixelFormatSubType::Unsigned));
		SetupPixelFormat(PixelFormat::Depth16Stencil8,  PixelFormatDescription("Depth16Stencil8",  PixelFormatContent::DepthStencil, 0xFFFF0000,         0x0000FF00,         0,                  0,                  PixelFormatSubType::Unsigned));
		SetupPixelFormat(PixelFormat::Depth24,          PixelFormatDescription("Depth24",          PixelFormatContent::Depth,        0xFFFFFF,           0,                  0,                  0,                  PixelFormatSubType::Unsigned));
		SetupPixelFormat(PixelFormat::Depth24Stencil8,  PixelFormatDescription("Depth24Stencil8",  PixelFormatContent::DepthStencil, 0xFFFFFF00,         0x000000FF,         0,                  0,                  PixelFormatSubType::Unsigned));
		SetupPixelFormat(PixelFormat::Depth32F,         PixelFormatDescription("Depth32F",         PixelFormatContent::Depth,        0xFFFFFFFF,         0,                  0,                  0,                  PixelFormatSubType::Unsigned));
		SetupPixelFormat(PixelFormat::Depth32FStencil8, PixelFormatDescription("Depth32FStencil8", PixelFormatContent::DepthStencil, b32,                b8 >> 32,           0,                  0,                  PixelFormatSubType::Unsigned));
		SetupPixelFormat(PixelFormat::Stencil1,         PixelFormatDescription("Stencil1",         PixelFormatContent::Stencil,      0x1,                0,                  0,                  0,                  PixelFormatSubType::Unsigned));
		SetupPixelFormat(PixelFormat::Stencil4,         PixelFormatDescription("Stencil4",         PixelFormatContent::Stencil,      0xF,                0,                  0,                  0,                  PixelFormatSubType::Unsigned));
		SetupPixelFormat(PixelFormat::Stencil8,         PixelFormatDescription("Stencil8",         PixelFormatContent::Stencil,      0xFF,               0,                  0,                  0,                  PixelFormatSubType::Unsigned));
		SetupPixelFormat(PixelFormat::Stencil16,        PixelFormatDescription("Stencil16",        PixelFormatContent::Stencil,      0xFFFF,             0,                  0,                  0,                  PixelFormatSubType::Unsigned));

		for (unsigned int i = 0; i < PixelFormatCount; ++i)
		{
			if (!s_pixelFormatInfos[i].Validate())
				NazaraWarning("Pixel format 0x" + NumberToString(i, 16) + " (" + GetName(static_cast<Nz::PixelFormat>(i)) + ") failed validation tests");
		}

		/***********************************A8************************************/
		RegisterConverter<PixelFormat::A8, PixelFormat::BGRA8>();
		RegisterConverter<PixelFormat::A8, PixelFormat::LA8>();
		RegisterConverter<PixelFormat::A8, PixelFormat::RGB5A1>();
		RegisterConverter<PixelFormat::A8, PixelFormat::RGBA4>();
		RegisterConverter<PixelFormat::A8, PixelFormat::RGBA8>();

		/**********************************BGR8***********************************/
		RegisterConverter<PixelFormat::BGR8, PixelFormat::BGR8_SRGB>();
		RegisterConverter<PixelFormat::BGR8, PixelFormat::BGRA8>();
		RegisterConverter<PixelFormat::BGR8, PixelFormat::L8>();
		RegisterConverter<PixelFormat::BGR8, PixelFormat::LA8>();/*
		RegisterConverter<PixelFormat::BGR8, PixelFormat::RGB16F>();
		RegisterConverter<PixelFormat::BGR8, PixelFormat::RGB16I>();
		RegisterConverter<PixelFormat::BGR8, PixelFormat::RGB32F>();
		RegisterConverter<PixelFormat::BGR8, PixelFormat::RGB32I>();
		RegisterConverter<PixelFormat::BGR8, PixelFormat::RGBA16F>();
		RegisterConverter<PixelFormat::BGR8, PixelFormat::RGBA16I>();
		RegisterConverter<PixelFormat::BGR8, PixelFormat::RGBA32F>();
		RegisterConverter<PixelFormat::BGR8, PixelFormat::RGBA32I>();*/
		RegisterConverter<PixelFormat::BGR8, PixelFormat::RGB5A1>();
		RegisterConverter<PixelFormat::BGR8, PixelFormat::RGB8>();
		RegisterConverter<PixelFormat::BGR8, PixelFormat::RGBA4>();
		RegisterConverter<PixelFormat::BGR8, PixelFormat::RGBA8>();

		/**********************************BGRA8**********************************/
		RegisterConverter<PixelFormat::BGRA8, PixelFormat::A8>();
		RegisterConverter<PixelFormat::BGRA8, PixelFormat::BGR8>();
		RegisterConverter<PixelFormat::BGRA8, PixelFormat::BGRA8_SRGB>();
		RegisterConverter<PixelFormat::BGRA8, PixelFormat::L8>();
		RegisterConverter<PixelFormat::BGRA8, PixelFormat::LA8>();/*
		RegisterConverter<PixelFormat::BGRA8, PixelFormat::RGB16F>();
		RegisterConverter<PixelFormat::BGRA8, PixelFormat::RGB16I>();
		RegisterConverter<PixelFormat::BGRA8, PixelFormat::RGB32F>();
		RegisterConverter<PixelFormat::BGRA8, PixelFormat::RGB32I>();
		RegisterConverter<PixelFormat::BGRA8, PixelFormat::RGBA16F>();
		RegisterConverter<PixelFormat::BGRA8, PixelFormat::RGBA16I>();
		RegisterConverter<PixelFormat::BGRA8, PixelFormat::RGBA32F>();
		RegisterConverter<PixelFormat::BGRA8, PixelFormat::RGBA32I>();*/
		RegisterConverter<PixelFormat::BGRA8, PixelFormat::RGB5A1>();
		RegisterConverter<PixelFormat::BGRA8, PixelFormat::RGB8>();
		RegisterConverter<PixelFormat::BGRA8, PixelFormat::RGBA4>();
		RegisterConverter<PixelFormat::BGRA8, PixelFormat::RGBA8>();

		/**********************************DXT1***********************************/
		///TODO: Décompresseur DXT1
	/*
		RegisterConverter<PixelFormat::DXT1, PixelFormat::BGR8>();
		RegisterConverter<PixelFormat::DXT1, PixelFormat::BGRA8>();
		RegisterConverter<PixelFormat::DXT1, PixelFormat::DXT3>();
		RegisterConverter<PixelFormat::DXT1, PixelFormat::DXT5>();
		RegisterConverter<PixelFormat::DXT1, PixelFormat::L8>();
		RegisterConverter<PixelFormat::DXT1, PixelFormat::LA8>();
		RegisterConverter<PixelFormat::DXT1, PixelFormat::RGB16F>();
		RegisterConverter<PixelFormat::DXT1, PixelFormat::RGB16I>();
		RegisterConverter<PixelFormat::DXT1, PixelFormat::RGB32F>();
		RegisterConverter<PixelFormat::DXT1, PixelFormat::RGB32I>();
		RegisterConverter<PixelFormat::DXT1, PixelFormat::RGBA16F>();
		RegisterConverter<PixelFormat::DXT1, PixelFormat::RGBA16I>();
		RegisterConverter<PixelFormat::DXT1, PixelFormat::RGBA32F>();
		RegisterConverter<PixelFormat::DXT1, PixelFormat::RGBA32I>();
		RegisterConverter<PixelFormat::DXT1, PixelFormat::RGB5A1>();
		RegisterConverter<PixelFormat::DXT1, PixelFormat::RGB8>();
		RegisterConverter<PixelFormat::DXT1, PixelFormat::RGBA4>();
		RegisterConverter<PixelFormat::DXT1, PixelFormat::RGBA8>();
	*/

		/**********************************DXT3***********************************/
		///TODO: Décompresseur DXT3
	/*
		RegisterConverter<PixelFormat::DXT3, PixelFormat::BGR8>();
		RegisterConverter<PixelFormat::DXT3, PixelFormat::BGRA8>();
		RegisterConverter<PixelFormat::DXT3, PixelFormat::DXT1>();
		RegisterConverter<PixelFormat::DXT3, PixelFormat::DXT5>();
		RegisterConverter<PixelFormat::DXT3, PixelFormat::L8>();
		RegisterConverter<PixelFormat::DXT3, PixelFormat::LA8>();
		RegisterConverter<PixelFormat::DXT3, PixelFormat::RGB16F>();
		RegisterConverter<PixelFormat::DXT3, PixelFormat::RGB16I>();
		RegisterConverter<PixelFormat::DXT3, PixelFormat::RGB32F>();
		RegisterConverter<PixelFormat::DXT3, PixelFormat::RGB32I>();
		RegisterConverter<PixelFormat::DXT3, PixelFormat::RGBA16F>();
		RegisterConverter<PixelFormat::DXT3, PixelFormat::RGBA16I>();
		RegisterConverter<PixelFormat::DXT3, PixelFormat::RGBA32F>();
		RegisterConverter<PixelFormat::DXT3, PixelFormat::RGBA32I>();
		RegisterConverter<PixelFormat::DXT3, PixelFormat::RGB5A1>();
		RegisterConverter<PixelFormat::DXT3, PixelFormat::RGB8>();
		RegisterConverter<PixelFormat::DXT3, PixelFormat::RGBA4>();
		RegisterConverter<PixelFormat::DXT3, PixelFormat::RGBA8>();
	*/

		/**********************************DXT5***********************************/
		///TODO: Décompresseur DXT5
	/*
		RegisterConverter<PixelFormat::DXT5, PixelFormat::BGR8>();
		RegisterConverter<PixelFormat::DXT5, PixelFormat::BGRA8>();
		RegisterConverter<PixelFormat::DXT5, PixelFormat::DXT1>();
		RegisterConverter<PixelFormat::DXT5, PixelFormat::DXT3>();
		RegisterConverter<PixelFormat::DXT5, PixelFormat::L8>();
		RegisterConverter<PixelFormat::DXT5, PixelFormat::LA8>();
		RegisterConverter<PixelFormat::DXT5, PixelFormat::RGB16F>();
		RegisterConverter<PixelFormat::DXT5, PixelFormat::RGB16I>();
		RegisterConverter<PixelFormat::DXT5, PixelFormat::RGB32F>();
		RegisterConverter<PixelFormat::DXT5, PixelFormat::RGB32I>();
		RegisterConverter<PixelFormat::DXT5, PixelFormat::RGBA16F>();
		RegisterConverter<PixelFormat::DXT5, PixelFormat::RGBA16I>();
		RegisterConverter<PixelFormat::DXT5, PixelFormat::RGBA32F>();
		RegisterConverter<PixelFormat::DXT5, PixelFormat::RGBA32I>();
		RegisterConverter<PixelFormat::DXT5, PixelFormat::RGB5A1>();
		RegisterConverter<PixelFormat::DXT5, PixelFormat::RGB8>();
		RegisterConverter<PixelFormat::DXT5, PixelFormat::RGBA4>();
		RegisterConverter<PixelFormat::DXT5, PixelFormat::RGBA8>();
	*/

		/***********************************L8************************************/
		RegisterConverter<PixelFormat::L8, PixelFormat::BGR8>();
		RegisterConverter<PixelFormat::L8, PixelFormat::BGRA8>();
		RegisterConverter<PixelFormat::L8, PixelFormat::LA8>();/*
		RegisterConverter<PixelFormat::L8, PixelFormat::RGB16F>();
		RegisterConverter<PixelFormat::L8, PixelFormat::RGB16I>();
		RegisterConverter<PixelFormat::L8, PixelFormat::RGB32F>();
		RegisterConverter<PixelFormat::L8, PixelFormat::RGB32I>();
		RegisterConverter<PixelFormat::L8, PixelFormat::RGBA16F>();
		RegisterConverter<PixelFormat::L8, PixelFormat::RGBA16I>();
		RegisterConverter<PixelFormat::L8, PixelFormat::RGBA32F>();
		RegisterConverter<PixelFormat::L8, PixelFormat::RGBA32I>();*/
		RegisterConverter<PixelFormat::L8, PixelFormat::RGB5A1>();
		RegisterConverter<PixelFormat::L8, PixelFormat::RGB8>();
		RegisterConverter<PixelFormat::L8, PixelFormat::RGBA4>();
		RegisterConverter<PixelFormat::L8, PixelFormat::RGBA8>();

		/***********************************LA8***********************************/
		RegisterConverter<PixelFormat::LA8, PixelFormat::A8>();
		RegisterConverter<PixelFormat::LA8, PixelFormat::BGR8>();
		RegisterConverter<PixelFormat::LA8, PixelFormat::BGRA8>();
		RegisterConverter<PixelFormat::LA8, PixelFormat::L8>();/*
		RegisterConverter<PixelFormat::LA8, PixelFormat::RGB16F>();
		RegisterConverter<PixelFormat::LA8, PixelFormat::RGB16I>();
		RegisterConverter<PixelFormat::LA8, PixelFormat::RGB32F>();
		RegisterConverter<PixelFormat::LA8, PixelFormat::RGB32I>();
		RegisterConverter<PixelFormat::LA8, PixelFormat::RGBA16F>();
		RegisterConverter<PixelFormat::LA8, PixelFormat::RGBA16I>();
		RegisterConverter<PixelFormat::LA8, PixelFormat::RGBA32F>();
		RegisterConverter<PixelFormat::LA8, PixelFormat::RGBA32I>();*/
		RegisterConverter<PixelFormat::LA8, PixelFormat::RGB5A1>();
		RegisterConverter<PixelFormat::LA8, PixelFormat::RGB8>();
		RegisterConverter<PixelFormat::LA8, PixelFormat::RGBA4>();
		RegisterConverter<PixelFormat::LA8, PixelFormat::RGBA8>();

		/**********************************RGBA4**********************************/
		RegisterConverter<PixelFormat::RGBA4, PixelFormat::A8>();
		RegisterConverter<PixelFormat::RGBA4, PixelFormat::BGR8>();
		RegisterConverter<PixelFormat::RGBA4, PixelFormat::BGRA8>();
		RegisterConverter<PixelFormat::RGBA4, PixelFormat::L8>();
		RegisterConverter<PixelFormat::RGBA4, PixelFormat::LA8>();/*
		RegisterConverter<PixelFormat::RGBA4, PixelFormat::RGB16F>();
		RegisterConverter<PixelFormat::RGBA4, PixelFormat::RGB16I>();
		RegisterConverter<PixelFormat::RGBA4, PixelFormat::RGB32F>();
		RegisterConverter<PixelFormat::RGBA4, PixelFormat::RGB32I>();
		RegisterConverter<PixelFormat::RGBA4, PixelFormat::RGBA16F>();
		RegisterConverter<PixelFormat::RGBA4, PixelFormat::RGBA16I>();
		RegisterConverter<PixelFormat::RGBA4, PixelFormat::RGBA32F>();
		RegisterConverter<PixelFormat::RGBA4, PixelFormat::RGBA32I>();*/
		RegisterConverter<PixelFormat::RGBA4, PixelFormat::RGB5A1>();
		RegisterConverter<PixelFormat::RGBA4, PixelFormat::RGB8>();
		RegisterConverter<PixelFormat::RGBA4, PixelFormat::RGBA8>();

		/*********************************RGB5A1**********************************/
		RegisterConverter<PixelFormat::RGB5A1, PixelFormat::A8>();
		RegisterConverter<PixelFormat::RGB5A1, PixelFormat::BGR8>();
		RegisterConverter<PixelFormat::RGB5A1, PixelFormat::BGRA8>();
		RegisterConverter<PixelFormat::RGB5A1, PixelFormat::L8>();
		RegisterConverter<PixelFormat::RGB5A1, PixelFormat::LA8>();/*
		RegisterConverter<PixelFormat::RGB5A1, PixelFormat::RGB16F>();
		RegisterConverter<PixelFormat::RGB5A1, PixelFormat::RGB16I>();
		RegisterConverter<PixelFormat::RGB5A1, PixelFormat::RGB32F>();
		RegisterConverter<PixelFormat::RGB5A1, PixelFormat::RGB32I>();
		RegisterConverter<PixelFormat::RGB5A1, PixelFormat::RGBA16F>();
		RegisterConverter<PixelFormat::RGB5A1, PixelFormat::RGBA16I>();
		RegisterConverter<PixelFormat::RGB5A1, PixelFormat::RGBA32F>();
		RegisterConverter<PixelFormat::RGB5A1, PixelFormat::RGBA32I>();*/
		RegisterConverter<PixelFormat::RGB5A1, PixelFormat::RGB8>();
		RegisterConverter<PixelFormat::RGB5A1, PixelFormat::RGBA4>();
		RegisterConverter<PixelFormat::RGB5A1, PixelFormat::RGBA8>();

		/**********************************RGB8***********************************/
		RegisterConverter<PixelFormat::RGB8, PixelFormat::BGR8>();
		RegisterConverter<PixelFormat::RGB8, PixelFormat::BGRA8>();
		RegisterConverter<PixelFormat::RGB8, PixelFormat::L8>();
		RegisterConverter<PixelFormat::RGB8, PixelFormat::LA8>();/*
		RegisterConverter<PixelFormat::RGB8, PixelFormat::RGB16F>();
		RegisterConverter<PixelFormat::RGB8, PixelFormat::RGB16I>();
		RegisterConverter<PixelFormat::RGB8, PixelFormat::RGB32F>();
		RegisterConverter<PixelFormat::RGB8, PixelFormat::RGB32I>();
		RegisterConverter<PixelFormat::RGB8, PixelFormat::RGBA16F>();
		RegisterConverter<PixelFormat::RGB8, PixelFormat::RGBA16I>();
		RegisterConverter<PixelFormat::RGB8, PixelFormat::RGBA32F>();
		RegisterConverter<PixelFormat::RGB8, PixelFormat::RGBA32I>();*/
		RegisterConverter<PixelFormat::RGB8, PixelFormat::RGB5A1>();
		RegisterConverter<PixelFormat::RGB8, PixelFormat::RGB8_SRGB>();
		RegisterConverter<PixelFormat::RGB8, PixelFormat::RGBA4>();
		RegisterConverter<PixelFormat::RGB8, PixelFormat::RGBA8>();

		/**********************************RGBA8**********************************/
		RegisterConverter<PixelFormat::RGBA8, PixelFormat::A8>();
		RegisterConverter<PixelFormat::RGBA8, PixelFormat::BGR8>();
		RegisterConverter<PixelFormat::RGBA8, PixelFormat::BGRA8>();
		RegisterConverter<PixelFormat::RGBA8, PixelFormat::L8>();
		RegisterConverter<PixelFormat::RGBA8, PixelFormat::LA8>();/*
		RegisterConverter<PixelFormat::RGBA8, PixelFormat::RGB16F>();
		RegisterConverter<PixelFormat::RGBA8, PixelFormat::RGB16I>();
		RegisterConverter<PixelFormat::RGBA8, PixelFormat::RGB32F>();
		RegisterConverter<PixelFormat::RGBA8, PixelFormat::RGB32I>();
		RegisterConverter<PixelFormat::RGBA8, PixelFormat::RGBA16F>();
		RegisterConverter<PixelFormat::RGBA8, PixelFormat::RGBA16I>();
		RegisterConverter<PixelFormat::RGBA8, PixelFormat::RGBA32F>();
		RegisterConverter<PixelFormat::RGBA8, PixelFormat::RGBA32I>();*/
		RegisterConverter<PixelFormat::RGBA8, PixelFormat::RGB5A1>();
		RegisterConverter<PixelFormat::RGBA8, PixelFormat::RGB8>();
		RegisterConverter<PixelFormat::RGBA8, PixelFormat::RGBA4>();
		RegisterConverter<PixelFormat::RGBA8, PixelFormat::RGBA8_SRGB>();

		return true;
	}

	void PixelFormatInfo::Uninitialize()
	{
		for (std::size_t i = 0; i < PixelFormatCount; ++i)
		{
			s_pixelFormatInfos[i].Clear();
			for (std::size_t j = 0; j < PixelFormatCount; ++j)
				s_convertFunctions[i][j] = nullptr;

			for (std::size_t j = 0; j < PixelFlippingCount; ++j)
				s_flipFunctions[i][j] = nullptr;
		}
	}

	std::array<std::array<PixelFormatInfo::ConvertFunction, PixelFormatCount>, PixelFormatCount> PixelFormatInfo::s_convertFunctions;
	std::array<std::array<PixelFormatInfo::FlipFunction, PixelFlippingCount>, PixelFormatCount> PixelFormatInfo::s_flipFunctions;
	std::array<PixelFormatDescription, PixelFormatCount> PixelFormatInfo::s_pixelFormatInfos;
}
