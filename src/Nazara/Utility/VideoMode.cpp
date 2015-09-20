// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/VideoMode.hpp>
#include <algorithm>
#include <functional>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#include <Nazara/Utility/Win32/VideoModeImpl.hpp>
#elif defined(NAZARA_PLATFORM_X11)
	#include <Nazara/Utility/X11/VideoModeImpl.hpp>
#else
	#error Lack of implementation: Window
#endif

#include <Nazara/Utility/Debug.hpp>

NzVideoMode::NzVideoMode() :
bitsPerPixel(0),
height(0),
width(0)
{
}

NzVideoMode::NzVideoMode(unsigned int w, unsigned int h, nzUInt8 bpp) :
bitsPerPixel(bpp),
height(h),
width(w)
{
}

bool NzVideoMode::IsFullscreenValid() const
{
	const std::vector<NzVideoMode>& modes = GetFullscreenModes();

	return std::binary_search(modes.begin(), modes.end(), *this, std::greater<NzVideoMode>());
}

NzVideoMode NzVideoMode::GetDesktopMode()
{
	return NzVideoModeImpl::GetDesktopMode();
}

const std::vector<NzVideoMode>& NzVideoMode::GetFullscreenModes()
{
	static std::vector<NzVideoMode> modes;
	if (modes.empty())
	{
		NzVideoModeImpl::GetFullscreenModes(modes);
		std::sort(modes.begin(), modes.end(), std::greater<NzVideoMode>());
	}

	return modes;
}

bool operator==(const NzVideoMode& left, const NzVideoMode& right)
{
	return left.width == right.width && left.height == right.height && left.bitsPerPixel == right.bitsPerPixel;
}

bool operator!=(const NzVideoMode& left, const NzVideoMode& right)
{
	return left.width != right.width || left.height != right.height || left.bitsPerPixel != right.bitsPerPixel;
}

bool operator<(const NzVideoMode& left, const NzVideoMode& right)
{
	if (left.bitsPerPixel == right.bitsPerPixel)
	{
		if (left.width == right.width)
			return left.height < right.height;
		else
			return left.width < right.width;
	}
	else
		return left.bitsPerPixel < right.bitsPerPixel;
}

bool operator<=(const NzVideoMode& left, const NzVideoMode& right)
{
	if (left.bitsPerPixel == right.bitsPerPixel)
	{
		if (left.width == right.width)
			return left.height <= right.height;
		else
			return left.width < right.width;
	}
	else
		return left.bitsPerPixel < right.bitsPerPixel;
}

bool operator>(const NzVideoMode& left, const NzVideoMode& right)
{
	return right < left;
}

bool operator>=(const NzVideoMode& left, const NzVideoMode& right)
{
	return right <= left;
}
