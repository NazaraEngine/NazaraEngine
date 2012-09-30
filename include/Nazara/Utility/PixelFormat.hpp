// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PIXELFORMAT_HPP
#define NAZARA_PIXELFORMAT_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <map>

class NzUtility;

class NzPixelFormat
{
	friend class NzUtility;

	public:
		using ConvertFunction = nzUInt8* (*)(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst);
		using FlipFunction = void (*)(unsigned int width, unsigned int height, unsigned int depth, const nzUInt8* src, nzUInt8* dst);

		static bool Convert(nzPixelFormat srcFormat, nzPixelFormat dstFormat, const void* src, void* dst);
		static bool Convert(nzPixelFormat srcFormat, nzPixelFormat dstFormat, const void* start, const void* end, void* dst);

		static bool Flip(nzPixelFlipping flipping, nzPixelFormat format, unsigned int width, unsigned int height, unsigned int depth, const void* src, void* dst);

		static nzUInt8 GetBitsPerPixel(nzPixelFormat format);
		static nzUInt8 GetBytesPerPixel(nzPixelFormat format);
		static nzPixelFormatType GetType(nzPixelFormat format);

		static bool HasAlpha(nzPixelFormat format);

		static bool IsCompressed(nzPixelFormat format);
		static bool IsConversionSupported(nzPixelFormat srcFormat, nzPixelFormat dstFormat);
		static bool IsValid(nzPixelFormat format);

		static void SetConvertFunction(nzPixelFormat srcFormat, nzPixelFormat dstFormat, ConvertFunction func);
		static void SetFlipFunction(nzPixelFlipping flipping, nzPixelFormat format, FlipFunction func);

		static NzString ToString(nzPixelFormat format);

	private:
		static bool Initialize();
		static void Uninitialize();

		static NAZARA_API ConvertFunction s_convertFunctions[nzPixelFormat_Max+1][nzPixelFormat_Max+1];
		static NAZARA_API std::map<nzPixelFormat, FlipFunction> s_flipFunctions[nzPixelFlipping_Max+1];
};

#include <Nazara/Utility/PixelFormat.inl>

#endif // NAZARA_PIXELFORMAT_HPP
