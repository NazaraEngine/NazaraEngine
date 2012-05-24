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

	nzPixelFormat_BGR8,
	nzPixelFormat_BGRA8,
	nzPixelFormat_DXT1,
	nzPixelFormat_DXT3,
	nzPixelFormat_DXT5,
	nzPixelFormat_L8,
	nzPixelFormat_LA8,
	/*
	nzPixelFormat_RGB16F,
	nzPixelFormat_RGB16I,
	nzPixelFormat_RGB32F,
	nzPixelFormat_RGB32I,
	*/
	nzPixelFormat_RGBA16F,
	nzPixelFormat_RGBA16I,
	/*
	nzPixelFormat_RGBA32F,
	nzPixelFormat_RGBA32I,
	*/
	nzPixelFormat_RGBA4,
	nzPixelFormat_RGB5A1,
	nzPixelFormat_RGB8,
	nzPixelFormat_RGBA8,

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
