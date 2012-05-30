// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PIXELFORMAT_HPP
#define NAZARA_PIXELFORMAT_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/String.hpp>

enum nzPixelFormat
{
	nzPixelFormat_Undefined,

	nzPixelFormat_BGR8,    // 3*nzUInt8
	nzPixelFormat_BGRA8,   // 4*nzUInt8
	nzPixelFormat_DXT1,
	nzPixelFormat_DXT3,
	nzPixelFormat_DXT5,
	nzPixelFormat_L8,      // 1*nzUInt8
	nzPixelFormat_LA8,     // 2*nzUInt8
	/*
	nzPixelFormat_RGB16F,
	nzPixelFormat_RGB16I,  // 4*nzUInt16
	nzPixelFormat_RGB32F,
	nzPixelFormat_RGB32I,  // 4*nzUInt32
	nzPixelFormat_RGBA16F,
	nzPixelFormat_RGBA16I, // 4*nzUInt16
	nzPixelFormat_RGBA32F,
	nzPixelFormat_RGBA32I, // 4*nzUInt32
	*/
	nzPixelFormat_RGBA4,   // 1*nzUInt16
	nzPixelFormat_RGB5A1,  // 1*nzUInt16
	nzPixelFormat_RGB8,    // 3*nzUInt8
	nzPixelFormat_RGBA8,   // 4*nzUInt8

	nzPixelFormat_Count
};

class NzUtility;

class NzPixelFormat
{
	friend class NzUtility;

	public:
		typedef nzUInt8* (*ConvertFunction)(const nzUInt8* start, const nzUInt8* end, nzUInt8* dst);

		static bool Convert(nzPixelFormat srcFormat, nzPixelFormat dstFormat, const void* src, void* dst);
		static bool Convert(nzPixelFormat srcFormat, nzPixelFormat dstFormat, const void* start, const void* end, void* dst);

		static nzUInt8 GetBPP(nzPixelFormat format);

		static bool IsCompressed(nzPixelFormat format);
		static bool IsConversionSupported(nzPixelFormat srcFormat, nzPixelFormat dstFormat);
		static bool IsValid(nzPixelFormat format);

		static void SetConvertFunction(nzPixelFormat srcFormat, nzPixelFormat dstFormat, ConvertFunction);

		static NzString ToString(nzPixelFormat format);

	private:
		static bool Initialize();
		static void Uninitialize();

		static ConvertFunction s_convertFunctions[nzPixelFormat_Count][nzPixelFormat_Count];
};

#include <Nazara/Utility/PixelFormat.inl>

#endif // NAZARA_PIXELFORMAT_HPP
