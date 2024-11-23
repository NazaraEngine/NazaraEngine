// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/PixelFormat.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <NazaraUtils/Endianness.hpp>

namespace Nz
{
	namespace NAZARA_ANONYMOUS_NAMESPACE
	{
		template<PixelFormat from, PixelFormat to>
		UInt8* ConvertPixels(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			NazaraUnused(start);
			NazaraUnused(dst);
			NazaraUnused(end);

			NazaraInternalError("Conversion from {0} to {1} is not supported", PixelFormatInfo::GetName(from), PixelFormatInfo::GetName(to));
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
				*ptr = ByteSwap(*ptr);
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
				*ptr = 0xFFF0 | (*start >> 4);

#ifdef NAZARA_BIG_ENDIAN
				*ptr = ByteSwap(*ptr);
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
				*ptr = (static_cast<UInt16>(start[2] >> 3) << 11) |
					   (static_cast<UInt16>(start[1] >> 3) << 6)  |
					   (static_cast<UInt16>(start[0] >> 3) << 1)  |
					   0x1;

#ifdef NAZARA_BIG_ENDIAN
				*ptr = ByteSwap(*ptr);
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
				*ptr = (static_cast<UInt16>(start[2] >> 4) << 12) |
					   (static_cast<UInt16>(start[1] >> 4) << 8)  |
					   (static_cast<UInt16>(start[0] >> 4) << 4)  |
					   0x0F;

#ifdef NAZARA_BIG_ENDIAN
				*ptr = ByteSwap(*ptr);
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

		template<>
		UInt8* ConvertPixels<PixelFormat::BGR8, PixelFormat::RGBA32F>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			float* ptr = reinterpret_cast<float*>(dst);
			while (start < end)
			{
				*ptr++ = start[2] / 255.f;
				*ptr++ = start[1] / 255.f;
				*ptr++ = start[0] / 255.f;
				*ptr++ = 1.f;

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
				*ptr = (static_cast<UInt16>(start[2] >> 4) << 12) |
					   (static_cast<UInt16>(start[1] >> 4) << 8)  |
					   (static_cast<UInt16>(start[0] >> 4) << 4)  |
					   (static_cast<UInt16>(start[3] >> 4) << 0);

#ifdef NAZARA_BIG_ENDIAN
				*ptr = ByteSwap(*ptr);
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
				*ptr = (static_cast<UInt16>(start[2] >> 3) << 11) |
					   (static_cast<UInt16>(start[1] >> 3) << 6)  |
					   (static_cast<UInt16>(start[0] >> 3) << 1)  |
					   ((start[3] > 0xF) ? 1 : 0); // > 128

#ifdef NAZARA_BIG_ENDIAN
				*ptr = ByteSwap(*ptr);
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

		template<>
		UInt8* ConvertPixels<PixelFormat::BGRA8, PixelFormat::RGBA32F>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			float* ptr = reinterpret_cast<float*>(dst);
			while (start < end)
			{
				*ptr++ = start[2] / 255.f;
				*ptr++ = start[1] / 255.f;
				*ptr++ = start[0] / 255.f;
				*ptr++ = start[3] / 255.f;

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
				UInt16 l = static_cast<UInt16>(start[0] >> 3);

				*ptr = (l << 11) |
					   (l << 6)  |
					   (l << 1)  |
					   1;

#ifdef NAZARA_BIG_ENDIAN
				*ptr = ByteSwap(*ptr);
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
				UInt16 l = static_cast<UInt16>(start[0] >> 4);

				*ptr = (l << 12) |
					   (l << 8)  |
					   (l << 4)  |
					   0x0F;

#ifdef NAZARA_BIG_ENDIAN
				*ptr = ByteSwap(*ptr);
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

		template<>
		UInt8* ConvertPixels<PixelFormat::L8, PixelFormat::RGBA32F>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			float* ptr = reinterpret_cast<float*>(dst);
			while (start < end)
			{
				*ptr++ = start[0] / 255.f;
				*ptr++ = start[0] / 255.f;
				*ptr++ = start[0] / 255.f;
				*ptr++ = 1.f;

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
				UInt16 l = static_cast<UInt16>(start[0] >> 3);

				*ptr = (l << 11) | (l << 6) | (l << 1) | ((start[1] > 0xF) ? 1 : 0);

#ifdef NAZARA_BIG_ENDIAN
				*ptr = ByteSwap(*ptr);
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
				UInt16 l = static_cast<UInt16>(start[0] >> 4);

				*ptr = (l << 12) | (l << 8) | (l << 4) | (SafeCast<UInt8>(start[1]) >> 4);

#ifdef NAZARA_BIG_ENDIAN
				*ptr = ByteSwap(*ptr);
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

		template<>
		UInt8* ConvertPixels<PixelFormat::LA8, PixelFormat::RGBA32F>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			float* ptr = reinterpret_cast<float*>(dst);
			while (start < end)
			{
				*ptr++ = start[0] / 255.f;
				*ptr++ = start[0] / 255.f;
				*ptr++ = start[0] / 255.f;
				*ptr++ = start[1] / 255.f;

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
				pixel = ByteSwap(pixel);
#endif

				*dst++ = SafeCast<UInt8>(pixel & 0x000F) << 4;

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
				pixel = ByteSwap(pixel);
#endif

				*dst++ = SafeCast<UInt8>((pixel & 0x00F0) >> 4)  << 4;
				*dst++ = SafeCast<UInt8>((pixel & 0x0F00) >> 8)  << 4;
				*dst++ = SafeCast<UInt8>((pixel & 0xF000) >> 12) << 4;

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
				pixel = ByteSwap(pixel);
#endif

				*dst++ = SafeCast<UInt8>((pixel & 0x00F0) >> 4)  << 4;
				*dst++ = SafeCast<UInt8>((pixel & 0x0F00) >> 8)  << 4;
				*dst++ = SafeCast<UInt8>((pixel & 0xF000) >> 12) << 4;
				*dst++ = SafeCast<UInt8>((pixel & 0x000F) >> 0)  << 4;

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
				pixel = ByteSwap(pixel);
#endif

				UInt16 r = SafeCast<UInt8>((pixel & 0xF000) >> 12) << 4;
				UInt16 g = SafeCast<UInt8>((pixel & 0x0F00) >> 8) << 4;
				UInt16 b = SafeCast<UInt8>((pixel & 0x00F0) >> 4) << 4;

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
				pixel = ByteSwap(pixel);
#endif

				UInt16 r = SafeCast<UInt8>((pixel & 0xF000) >> 12) << 4;
				UInt16 g = SafeCast<UInt8>((pixel & 0x0F00) >> 8) << 4;
				UInt16 b = SafeCast<UInt8>((pixel & 0x00F0) >> 4) << 4;

				*dst++ = static_cast<UInt8>(r * 0.3f + g * 0.59f + b * 0.11f);
				*dst++ = SafeCast<UInt8>((pixel & 0x000F) << 4);

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
				pixel = ByteSwap(pixel);
#endif

				UInt16 r = SafeCast<UInt8>((pixel & 0xF000) >> 12) << 1;
				UInt16 g = SafeCast<UInt8>((pixel & 0x0F00) >> 8)  << 1;
				UInt16 b = SafeCast<UInt8>((pixel & 0x00F0) >> 4)  << 1;
				UInt16 a = SafeCast<UInt8>((pixel & 0x000F) >> 0)  << 1;

				*ptr = (r << 11) | (g << 6) | (b << 1) | ((a > 0x3) ? 1 : 0);

#ifdef NAZARA_BIG_ENDIAN
				*ptr = ByteSwap(*ptr);
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
				pixel = ByteSwap(pixel);
#endif

				*dst++ = SafeCast<UInt8>((pixel & 0xF000) >> 12) << 4;
				*dst++ = SafeCast<UInt8>((pixel & 0x0F00) >> 8)  << 4;
				*dst++ = SafeCast<UInt8>((pixel & 0x00F0) >> 4)  << 4;

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
				pixel = ByteSwap(pixel);
#endif

				*dst++ = SafeCast<UInt8>((pixel & 0xF000) >> 12) << 4;
				*dst++ = SafeCast<UInt8>((pixel & 0x0F00) >> 8)  << 4;
				*dst++ = SafeCast<UInt8>((pixel & 0x00F0) >> 4)  << 4;
				*dst++ = SafeCast<UInt8>((pixel & 0x000F) >> 0)  << 4;

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
				pixel = ByteSwap(pixel);
#endif

				*dst++ = SafeCast<UInt8>((pixel & 0x1) * 0xFF);

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
				pixel = ByteSwap(pixel);
#endif

				*dst++ = SafeCast<UInt8>((pixel & 0x003E) >> 1)  << 3;
				*dst++ = SafeCast<UInt8>((pixel & 0x07C0) >> 6)  << 3;
				*dst++ = SafeCast<UInt8>((pixel & 0xF800) >> 11) << 3;

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
				pixel = ByteSwap(pixel);
#endif

				*dst++ = SafeCast<UInt8>((pixel & 0x003E) >> 1)  << 3;
				*dst++ = SafeCast<UInt8>((pixel & 0x07C0) >> 6)  << 3;
				*dst++ = SafeCast<UInt8>((pixel & 0xF800) >> 11) << 3;
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
				pixel = ByteSwap(pixel);
#endif

				UInt8 r = SafeCast<UInt8>((pixel & 0xF800) >> 11) << 3;
				UInt8 g = SafeCast<UInt8>((pixel & 0x07C0) >> 6)  << 3;
				UInt8 b = SafeCast<UInt8>((pixel & 0x003E) >> 1)  << 3;

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
				pixel = ByteSwap(pixel);
#endif

				UInt8 r = SafeCast<UInt8>((pixel & 0xF800) >> 11) << 3;
				UInt8 g = SafeCast<UInt8>((pixel & 0x07C0) >> 6)  << 3;
				UInt8 b = SafeCast<UInt8>((pixel & 0x003E) >> 1)  << 3;

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
				pixel = ByteSwap(pixel);
#endif

				*dst++ = SafeCast<UInt8>((pixel & 0xF800) >> 11) << 3;
				*dst++ = SafeCast<UInt8>((pixel & 0x07C0) >> 6)  << 3;
				*dst++ = SafeCast<UInt8>((pixel & 0x003E) >> 1)  << 3;

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
				pixel = ByteSwap(pixel);
#endif

				UInt8 r = SafeCast<UInt8>((pixel & 0xF800) >> 11) >> 1;
				UInt8 g = SafeCast<UInt8>((pixel & 0x07C0) >> 6)  >> 1;
				UInt8 b = SafeCast<UInt8>((pixel & 0x003E) >> 1)  >> 1;

				*ptr = (r << 12) | (g << 8) | (b << 4) | ((pixel & 0x1)*0x0F);

#ifdef NAZARA_BIG_ENDIAN
				*ptr = ByteSwap(*ptr);
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
				pixel = ByteSwap(pixel);
#endif

				*dst++ = SafeCast<UInt8>((pixel & 0xF800) >> 11) >> 1;
				*dst++ = SafeCast<UInt8>((pixel & 0x07C0) >> 6)  >> 1;
				*dst++ = SafeCast<UInt8>((pixel & 0x003E) >> 1)  >> 1;
				*dst++ = SafeCast<UInt8>((pixel & 0x1) * 0xFF);

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
				*ptr = (static_cast<UInt16>(start[0] >> 3) << 11) |
					   (static_cast<UInt16>(start[1] >> 3) << 6)  |
					   (static_cast<UInt16>(start[2] >> 3) << 1)  |
					   0x1;

#ifdef NAZARA_BIG_ENDIAN
				*ptr = ByteSwap(*ptr);
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
				*ptr = (static_cast<UInt16>(start[0] >> 4) << 12) |
					   (static_cast<UInt16>(start[1] >> 4) << 8)  |
					   (static_cast<UInt16>(start[2] >> 4) << 4)  |
					   0x0F;

#ifdef NAZARA_BIG_ENDIAN
				*ptr = ByteSwap(*ptr);
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

		template<>
		UInt8* ConvertPixels<PixelFormat::RGB8, PixelFormat::RGBA32F>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			float* ptr = reinterpret_cast<float*>(dst);
			while (start < end)
			{
				*ptr++ = start[0] / 255.f;
				*ptr++ = start[1] / 255.f;
				*ptr++ = start[2] / 255.f;
				*ptr++ = 1.f;

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
				*ptr = (static_cast<UInt16>(start[0] >> 3) << 11) |
					   (static_cast<UInt16>(start[1] >> 3) << 6)  |
					   (static_cast<UInt16>(start[2] >> 3) << 1)  |
					   ((start[3] > 0xF) ? 1 : 0); // > 128

#ifdef NAZARA_BIG_ENDIAN
				*ptr = ByteSwap(*ptr);
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
				*ptr = (static_cast<UInt16>(start[0] >> 4) << 12) |
					   (static_cast<UInt16>(start[1] >> 4) << 8)  |
					   (static_cast<UInt16>(start[2] >> 4) << 4)  |
					   (static_cast<UInt16>(start[3] >> 4) << 0);

#ifdef NAZARA_BIG_ENDIAN
				*ptr = ByteSwap(*ptr);
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

		template<>
		UInt8* ConvertPixels<PixelFormat::RGBA8, PixelFormat::RGBA32F>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			float* ptr = reinterpret_cast<float*>(dst);
			while (start < end)
			{
				*ptr++ = start[0] / 255.f;
				*ptr++ = start[1] / 255.f;
				*ptr++ = start[2] / 255.f;
				*ptr++ = start[3] / 255.f;

				start += 4;
			}

			return dst;
		}

		/**********************************RGBA32F**********************************/
		template<>
		UInt8* ConvertPixels<PixelFormat::RGBA32F, PixelFormat::RGBA8>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			while (start < end)
			{
				const float* ptr = reinterpret_cast<const float*>(start);

				*dst++ = static_cast<UInt8>(ptr[0] * 255.f);
				*dst++ = static_cast<UInt8>(ptr[1] * 255.f);
				*dst++ = static_cast<UInt8>(ptr[2] * 255.f);
				*dst++ = static_cast<UInt8>(ptr[3] * 255.f);

				start += 16;
			}

			return dst;
		}

		template<>
		UInt8* ConvertPixels<PixelFormat::RGBA32F, PixelFormat::RGBA8_SRGB>(const UInt8* start, const UInt8* end, UInt8* dst)
		{
			return ConvertPixels<PixelFormat::RGBA32F, PixelFormat::RGBA8>(start, end, dst);
		}

		template<PixelFormat Format1, PixelFormat Format2>
		void RegisterConverter()
		{
			PixelFormatInfo::SetConvertFunction(Format1, Format2, &ConvertPixels<Format1, Format2>);
		}
	}

	bool PixelFormatInfo::Flip(PixelFlipping flipping, PixelFormat format, unsigned int width, unsigned int height, unsigned int depth, const void* src, void* dst)
	{
		NazaraAssertMsg(IsValid(format), "invalid pixel format");

		auto& flipFunction = s_flipFunctions[format][flipping];
		if (flipFunction)
			flipFunction(width, height, depth, reinterpret_cast<const UInt8*>(src), reinterpret_cast<UInt8*>(dst));
		else
		{
			// Generic flipping
			NazaraAssertMsg(!IsCompressed(format), "not able to flip compressed formats");

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

	bool PixelFormatInfo::Initialize()
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		auto SetupPixelFormat = [](PixelFormat format, PixelFormatDescription&& desc)
		{
			s_pixelFormatInfos[format] = std::move(desc);
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

		for (auto&& [pixelFormat, pixelFormatInfo] : s_pixelFormatInfos.iter_kv())
		{
			if (!pixelFormatInfo.Validate())
				NazaraWarning("Pixel format {0:#x} {1} failed validation tests", UnderlyingCast(pixelFormat), GetName(pixelFormat));
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
		RegisterConverter<PixelFormat::BGR8, PixelFormat::RGBA32F>();

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
		RegisterConverter<PixelFormat::BGRA8, PixelFormat::RGBA32F>();

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
		RegisterConverter<PixelFormat::L8, PixelFormat::RGBA32F>();

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
		RegisterConverter<PixelFormat::LA8, PixelFormat::RGBA32F>();

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
		RegisterConverter<PixelFormat::RGB8, PixelFormat::RGBA32F>();

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
		RegisterConverter<PixelFormat::RGBA8, PixelFormat::RGBA32F>();

		/**********************************RGBA32F**********************************/
		RegisterConverter<PixelFormat::RGBA32F, PixelFormat::RGBA8>();
		RegisterConverter<PixelFormat::RGBA32F, PixelFormat::RGBA8_SRGB>();

		return true;
	}

	void PixelFormatInfo::Uninitialize()
	{
		for (auto&& [pixelFormat, formatInfo] : s_pixelFormatInfos.iter_kv())
		{
			formatInfo.Clear();

			for (auto& convertFuncs : s_convertFunctions)
				convertFuncs.fill(nullptr);

			for (auto& flipFuncs : s_flipFunctions)
				flipFuncs.fill(nullptr);
		}
	}

	EnumArray<PixelFormat, EnumArray<PixelFormat, PixelFormatInfo::ConvertFunction>> PixelFormatInfo::s_convertFunctions;
	EnumArray<PixelFormat, EnumArray<PixelFlipping, PixelFormatInfo::FlipFunction>> PixelFormatInfo::s_flipFunctions;
	EnumArray<PixelFormat, PixelFormatDescription> PixelFormatInfo::s_pixelFormatInfos;
}
