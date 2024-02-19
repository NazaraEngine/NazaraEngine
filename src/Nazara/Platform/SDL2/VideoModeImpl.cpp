// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Platform/SDL2/VideoModeImpl.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Platform/VideoMode.hpp>
#include <SDL_video.h>
#include <algorithm>

namespace Nz
{
	VideoMode VideoModeImpl::GetDesktopMode()
	{
		SDL_DisplayMode mode;
		if (SDL_GetDesktopDisplayMode(0, &mode) != 0) // handle multi screen ?
		{
			NazaraError(SDL_GetError());

			return VideoMode(800, 600, static_cast<UInt8>(32)); // useless ?
		}

		return VideoMode(mode.w, mode.h, SDL_BITSPERPIXEL(mode.format));
	}

	void VideoModeImpl::GetFullscreenModes(std::vector<VideoMode>& modes)
	{
		SDL_DisplayMode mode;

		int numModes = SDL_GetNumDisplayModes(0);
		if (numModes < 0)
		{
			NazaraError(SDL_GetError());

			return;
		}

		for (int i = 0; i < numModes; i++)
		{
			if (SDL_GetDisplayMode(0, i, &mode) != 0) // handle multi screen ?

				NazaraError(SDL_GetError());

			VideoMode vMode(mode.w, mode.h, SDL_BITSPERPIXEL(mode.format));

			if (std::find(modes.begin(), modes.end(), vMode) == modes.end())
				modes.push_back(vMode);
		}
	}
}
