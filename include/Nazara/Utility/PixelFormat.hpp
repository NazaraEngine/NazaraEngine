// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PIXELFORMAT_HPP
#define NAZARA_PIXELFORMAT_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Bitset.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <functional>
#include <map>

///TODO: Permettre la conversion automatique entre les formats via des renseignements de bits et de type pour chaque format.
///      Ce serait plus lent que la conversion spécialisée (qui ne disparaîtra donc pas) mais ça permettrait au moteur de faire la conversion
///      entre n'importe quels formats non-compressés.

namespace Nz
{
	struct PixelFormatInfo
	{
		inline PixelFormatInfo();
		inline PixelFormatInfo(PixelFormatContent formatContent, UInt8 bpp, PixelFormatSubType subType);
		inline PixelFormatInfo(const String& formatName, PixelFormatContent formatContent, UInt8 bpp, PixelFormatSubType subType);
		inline PixelFormatInfo(const String& formatName, PixelFormatContent formatContent, Bitset<> rMask, Bitset<> gMask, Bitset<> bMask, Bitset<> aMask, PixelFormatSubType subType);
		inline PixelFormatInfo(const String& formatName, PixelFormatContent formatContent, PixelFormatSubType rType, Bitset<> rMask, PixelFormatSubType gType, Bitset<> gMask, PixelFormatSubType bType, Bitset<> bMask, PixelFormatSubType aType, Bitset<> aMask, UInt8 bpp = 0);

		inline void Clear();

		inline bool IsCompressed() const;
		inline bool IsValid() const;

		inline void RecomputeBitsPerPixel();

		inline bool Validate() const;

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
		String name;
		UInt8 bitsPerPixel;
	};

	class NAZARA_UTILITY_API PixelFormat
	{
		friend class Utility;

		public:
			using ConvertFunction = std::function<UInt8*(const UInt8* start, const UInt8* end, UInt8* dst)>;
			using FlipFunction = std::function<void(unsigned int width, unsigned int height, unsigned int depth, const UInt8* src, UInt8* dst)>;

			static inline std::size_t ComputeSize(PixelFormatType format, unsigned int width, unsigned int height, unsigned int depth);

			static inline bool Convert(PixelFormatType srcFormat, PixelFormatType dstFormat, const void* src, void* dst);
			static inline bool Convert(PixelFormatType srcFormat, PixelFormatType dstFormat, const void* start, const void* end, void* dst);

			static bool Flip(PixelFlipping flipping, PixelFormatType format, unsigned int width, unsigned int height, unsigned int depth, const void* src, void* dst);

			static inline UInt8 GetBitsPerPixel(PixelFormatType format);
			static inline PixelFormatContent GetContent(PixelFormatType format);
			static inline UInt8 GetBytesPerPixel(PixelFormatType format);
			static inline const PixelFormatInfo& GetInfo(PixelFormatType format);
			static inline const String& GetName(PixelFormatType format);

			static inline bool HasAlpha(PixelFormatType format);

			static PixelFormatType IdentifyFormat(const PixelFormatInfo& info);

			static inline bool IsCompressed(PixelFormatType format);
			static inline bool IsConversionSupported(PixelFormatType srcFormat, PixelFormatType dstFormat);
			static inline bool IsValid(PixelFormatType format);

			static inline void SetConvertFunction(PixelFormatType srcFormat, PixelFormatType dstFormat, ConvertFunction func);
			static inline void SetFlipFunction(PixelFlipping flipping, PixelFormatType format, FlipFunction func);

		private:
			static bool Initialize();
			static void Uninitialize();

			static PixelFormatInfo s_pixelFormatInfos[PixelFormatType_Max + 1];
			static ConvertFunction s_convertFunctions[PixelFormatType_Max+1][PixelFormatType_Max+1];
			static std::map<PixelFormatType, FlipFunction> s_flipFunctions[PixelFlipping_Max+1];
	};
}

#include <Nazara/Utility/PixelFormat.inl>

#endif // NAZARA_PIXELFORMAT_HPP
