// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PIXELFORMAT_HPP
#define NAZARA_PIXELFORMAT_HPP

#include <Nazara/Prerequesites.hpp>
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
	class PixelFormat
	{
		friend class Utility;

		public:
			using ConvertFunction = std::function<UInt8*(const UInt8* start, const UInt8* end, UInt8* dst)>;
			using FlipFunction = std::function<void(unsigned int width, unsigned int height, unsigned int depth, const UInt8* src, UInt8* dst)>;

			static bool Convert(PixelFormatType srcFormat, PixelFormatType dstFormat, const void* src, void* dst);
			static bool Convert(PixelFormatType srcFormat, PixelFormatType dstFormat, const void* start, const void* end, void* dst);

			static bool Flip(PixelFlipping flipping, PixelFormatType format, unsigned int width, unsigned int height, unsigned int depth, const void* src, void* dst);

			static UInt8 GetBitsPerPixel(PixelFormatType format);
			static UInt8 GetBytesPerPixel(PixelFormatType format);
			static PixelFormatTypeType GetType(PixelFormatType format);

			static bool HasAlpha(PixelFormatType format);

			static bool IsCompressed(PixelFormatType format);
			static bool IsConversionSupported(PixelFormatType srcFormat, PixelFormatType dstFormat);
			static bool IsValid(PixelFormatType format);

			static void SetConvertFunction(PixelFormatType srcFormat, PixelFormatType dstFormat, ConvertFunction func);
			static void SetFlipFunction(PixelFlipping flipping, PixelFormatType format, FlipFunction func);

			static String ToString(PixelFormatType format);

		private:
			static bool Initialize();
			static void Uninitialize();

			static NAZARA_UTILITY_API ConvertFunction s_convertFunctions[PixelFormatType_Max+1][PixelFormatType_Max+1];
			static NAZARA_UTILITY_API std::map<PixelFormatType, FlipFunction> s_flipFunctions[PixelFlipping_Max+1];
	};
}

#include <Nazara/Utility/PixelFormat.inl>

#endif // NAZARA_PIXELFORMAT_HPP
