// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UTILITY_PIXELFORMAT_HPP
#define NAZARA_UTILITY_PIXELFORMAT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <NazaraUtils/Bitset.hpp>
#include <NazaraUtils/EnumArray.hpp>
#include <array>
#include <functional>

///TODO: Permettre la conversion automatique entre les formats via des renseignements de bits et de type pour chaque format.
///      Ce serait plus lent que la conversion spécialisée (qui ne disparaîtra donc pas) mais ça permettrait au moteur de faire la conversion
///      entre n'importe quels formats non-compressés.

namespace Nz
{
	struct PixelFormatDescription
	{
		inline PixelFormatDescription();
		inline PixelFormatDescription(PixelFormatContent formatContent, UInt8 bpp, PixelFormatSubType subType);
		inline PixelFormatDescription(std::string_view formatName, PixelFormatContent formatContent, UInt8 bpp, PixelFormatSubType subType);
		inline PixelFormatDescription(std::string_view formatName, PixelFormatContent formatContent, Bitset<> rMask, Bitset<> gMask, Bitset<> bMask, Bitset<> aMask, PixelFormatSubType subType);
		inline PixelFormatDescription(std::string_view formatName, PixelFormatContent formatContent, PixelFormatSubType rType, Bitset<> rMask, PixelFormatSubType gType, Bitset<> gMask, PixelFormatSubType bType, Bitset<> bMask, PixelFormatSubType aType, Bitset<> aMask, UInt8 bpp = 0);

		inline void Clear();

		inline bool IsCompressed() const;
		inline bool IsValid() const;

		inline void RecomputeBitsPerPixel();

		inline bool Validate() const;

		std::string_view name;
		// Warning: Masks bit order is reversed
		Bitset<> redMask;
		Bitset<> greenMask;
		Bitset<> blueMask;
		Bitset<> alphaMask;
		PixelFormatContent content;
		PixelFormatSubType redType;
		PixelFormatSubType greenType;
		PixelFormatSubType blueType;
		PixelFormatSubType alphaType;
		UInt8 bitsPerPixel;
	};

	class NAZARA_UTILITY_API PixelFormatInfo
	{
		friend class Utility;

		public:
			using ConvertFunction = std::function<UInt8*(const UInt8* start, const UInt8* end, UInt8* dst)>;
			using FlipFunction = std::function<void(unsigned int width, unsigned int height, unsigned int depth, const UInt8* src, UInt8* dst)>;

			static inline std::size_t ComputeSize(PixelFormat format, unsigned int width, unsigned int height, unsigned int depth);

			static inline bool Convert(PixelFormat srcFormat, PixelFormat dstFormat, const void* src, void* dst);
			static inline bool Convert(PixelFormat srcFormat, PixelFormat dstFormat, const void* start, const void* end, void* dst);

			static bool Flip(PixelFlipping flipping, PixelFormat format, unsigned int width, unsigned int height, unsigned int depth, const void* src, void* dst);

			static inline UInt8 GetBitsPerPixel(PixelFormat format);
			static inline PixelFormatContent GetContent(PixelFormat format);
			static inline UInt8 GetBytesPerPixel(PixelFormat format);
			static inline const PixelFormatDescription& GetInfo(PixelFormat format);
			static inline std::string_view GetName(PixelFormat format);

			static inline bool HasAlpha(PixelFormat format);

			static inline PixelFormat IdentifyFormat(const PixelFormatDescription& info);
			static inline PixelFormat IdentifyFormat(std::string_view formatName);

			static inline bool IsCompressed(PixelFormat format);
			static inline bool IsConversionSupported(PixelFormat srcFormat, PixelFormat dstFormat);
			static inline bool IsValid(PixelFormat format);

			static inline void SetConvertFunction(PixelFormat srcFormat, PixelFormat dstFormat, ConvertFunction func);
			static inline void SetFlipFunction(PixelFlipping flipping, PixelFormat format, FlipFunction func);

		private:
			static bool Initialize();
			static void Uninitialize();

			static EnumArray<PixelFormat, EnumArray<PixelFormat, ConvertFunction>> s_convertFunctions;
			static EnumArray<PixelFormat, EnumArray<PixelFlipping, FlipFunction>> s_flipFunctions;
			static EnumArray<PixelFormat, PixelFormatDescription> s_pixelFormatInfos;
	};
}

#include <Nazara/Utility/PixelFormat.inl>

#endif // NAZARA_UTILITY_PIXELFORMAT_HPP
