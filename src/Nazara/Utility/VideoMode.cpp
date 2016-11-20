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

namespace Nz
{
	VideoMode::VideoMode() :
	bitsPerPixel(0),
	height(0),
	width(0)
	{
	}
	
	VideoMode::VideoMode(unsigned int w, unsigned int h) :
	VideoMode(w, h, GetDesktopMode().bitsPerPixel)
	{
	}

	VideoMode::VideoMode(unsigned int w, unsigned int h, UInt8 bpp) :
	bitsPerPixel(bpp),
	height(h),
	width(w)
	{
	}

	bool VideoMode::IsFullscreenValid() const
	{
		const std::vector<VideoMode>& modes = GetFullscreenModes();

		return std::binary_search(modes.begin(), modes.end(), *this, std::greater<VideoMode>());
	}

	VideoMode VideoMode::GetDesktopMode()
	{
		return VideoModeImpl::GetDesktopMode();
	}

	const std::vector<VideoMode>& VideoMode::GetFullscreenModes()
	{
		static std::vector<VideoMode> modes;
		if (modes.empty())
		{
			VideoModeImpl::GetFullscreenModes(modes);
			std::sort(modes.begin(), modes.end(), std::greater<VideoMode>());
		}

		return modes;
	}

	bool operator==(const VideoMode& left, const VideoMode& right)
	{
		return left.width == right.width && left.height == right.height && left.bitsPerPixel == right.bitsPerPixel;
	}

	bool operator!=(const VideoMode& left, const VideoMode& right)
	{
		return left.width != right.width || left.height != right.height || left.bitsPerPixel != right.bitsPerPixel;
	}

	bool operator<(const VideoMode& left, const VideoMode& right)
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

	bool operator<=(const VideoMode& left, const VideoMode& right)
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

	bool operator>(const VideoMode& left, const VideoMode& right)
	{
		return right < left;
	}

	bool operator>=(const VideoMode& left, const VideoMode& right)
	{
		return right <= left;
	}
}
