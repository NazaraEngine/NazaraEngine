// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Platform/VideoMode.hpp>
#include <Nazara/Platform/SDL2/VideoModeImpl.hpp>
#include <algorithm>
#include <functional>

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
