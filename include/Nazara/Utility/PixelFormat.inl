// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Debug.hpp>

inline nzUInt8 NzPixelFormat::GetBPP(nzPixelFormat format)
{
	switch (format)
	{
		case nzPixelFormat_Undefined:
			return 0;

		case nzPixelFormat_B8G8R8:
			return 3;

		case nzPixelFormat_B8G8R8A8:
			return 4;

		case nzPixelFormat_DXT1:
			return 1;

		case nzPixelFormat_DXT3:
			return 2;

		case nzPixelFormat_DXT5:
			return 2;

		case nzPixelFormat_L8:
			return 1;

		case nzPixelFormat_L8A8:
			return 2;

		case nzPixelFormat_R4G4A4A4:
			return 2;

		case nzPixelFormat_R5G5A5A1:
			return 2;

		case nzPixelFormat_R8:
			return 1;

		case nzPixelFormat_R8G8:
			return 2;

		case nzPixelFormat_R8G8B8:
			return 3;

		case nzPixelFormat_R8G8B8A8:
			return 4;
	}

	return 0;
}

inline bool NzPixelFormat::IsCompressed(nzPixelFormat format)
{
	switch (format)
	{
		case nzPixelFormat_DXT1:
		case nzPixelFormat_DXT3:
		case nzPixelFormat_DXT5:
			return true;

		default:
			return false;
	}
}

#include <Nazara/Core/DebugOff.hpp>
