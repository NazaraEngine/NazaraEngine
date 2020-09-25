// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PIXELFORMAT_HPP
#define NAZARA_PIXELFORMAT_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Bitset.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <functional>
#include <map>

///TODO: Permettre la conversion automatique entre les formats via des renseignements de bits et de type pour chaque format.
///      Ce serait plus lent que la conversion spécialisée (qui ne disparaîtra donc pas) mais ça permettrait au moteur de faire la conversion
///      entre n'importe quels formats non-compressés.

namespace Nz
{
	struct PixelFormatDescription
	{
		inline PixelFormatDescription();
		inline PixelFormatDescription(PixelFormatContent formatContent, UInt8 bpp, PixelFormatSubType subType);
		inline PixelFormatDescription(const std::string& formatName, PixelFormatContent formatContent, UInt8 bpp, PixelFormatSubType subType);
		inline PixelFormatDescription(const std::string& formatName, PixelFormatContent formatContent, Bitset<> rMask, Bitset<> gMask, Bitset<> bMask, Bitset<> aMask, PixelFormatSubType subType);
		inline PixelFormatDescription(const std::string& formatName, PixelFormatContent formatContent, PixelFormatSubType rType, Bitset<> rMask, PixelFormatSubType gType, Bitset<> gMask, PixelFormatSubType bType, Bitset<> bMask, PixelFormatSubType aType, Bitset<> aMask, UInt8 bpp = 0);

		inline void Clear();

		inline bool IsCompressed() const;
		inline bool IsValid() const;

		inline void RecomputeBitsPerPixel();

		inline bool Validate() const;

		std::string name;
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
			static inline const std::string& GetName(PixelFormat format);

			static inline bool HasAlpha(PixelFormat format);

			static PixelFormat IdentifyFormat(const PixelFormatDescription& info);

			static inline bool IsCompressed(PixelFormat format);
			static inline bool IsConversionSupported(PixelFormat srcFormat, PixelFormat dstFormat);
			static inline bool IsValid(PixelFormat format);

			static inline void SetConvertFunction(PixelFormat srcFormat, PixelFormat dstFormat, ConvertFunction func);
			static inline void SetFlipFunction(PixelFlipping flipping, PixelFormat format, FlipFunction func);

		private:
			static bool Initialize();
			static void Uninitialize();

			static PixelFormatDescription s_pixelFormatInfos[PixelFormat_Max + 1];
			static ConvertFunction s_convertFunctions[PixelFormat_Max+1][PixelFormat_Max+1];
			static std::map<PixelFormat, FlipFunction> s_flipFunctions[PixelFlipping_Max+1];
	};
}

#include <Nazara/Utility/PixelFormat.inl>

#endif // NAZARA_PIXELFORMAT_HPP
