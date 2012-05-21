// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PIXELFORMAT_HPP
#define NAZARA_PIXELFORMAT_HPP

#include <Nazara/Prerequesites.hpp>

enum nzPixelFormat
{
	nzPixelFormat_Undefined,

	nzPixelFormat_B8G8R8,
	nzPixelFormat_B8G8R8A8,
	nzPixelFormat_DXT1,
	nzPixelFormat_DXT3,
	nzPixelFormat_DXT5,
	nzPixelFormat_L8,
	nzPixelFormat_L8A8,
	nzPixelFormat_R4G4A4A4,
	nzPixelFormat_R5G5A5A1,
	nzPixelFormat_R8,
	nzPixelFormat_R8G8,
	nzPixelFormat_R8G8B8,
	nzPixelFormat_R8G8B8A8
};

class NzPixelFormat
{
	public:
		static nzUInt8 GetBPP(nzPixelFormat format);
		static bool IsCompressed(nzPixelFormat format);
};

#include <Nazara/Utility/PixelFormat.inl>

#endif // NAZARA_PIXELFORMAT_HPP
