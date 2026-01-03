// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Platform/SDL3/VideoModeImpl.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Platform/VideoMode.hpp>
#include <SDL3/SDL_video.h>
#include <algorithm>

namespace Nz
{
	VideoMode VideoModeImpl::GetDesktopMode()
	{
		SDL_DisplayID primaryDisplay = SDL_GetPrimaryDisplay();
		if (primaryDisplay == 0)
		{
			NazaraError("SDL error: {}", SDL_GetError());
			return VideoMode(0, 0, UInt8(32));
		}

		const SDL_DisplayMode* mode = SDL_GetDesktopDisplayMode(primaryDisplay);
		if (!mode)
		{
			NazaraError("SDL error: {}", SDL_GetError());
			return VideoMode(0, 0, UInt8(32));
		}

		return VideoMode(mode->w, mode->h, SDL_BITSPERPIXEL(mode->format));
	}

	void VideoModeImpl::GetFullscreenModes(std::vector<VideoMode>& modeList)
	{
		SDL_DisplayID primaryDisplay = SDL_GetPrimaryDisplay();

		int count;
		SDL_DisplayMode** modes = SDL_GetFullscreenDisplayModes(primaryDisplay, &count);
		if (!modes)
		{
			NazaraError("SDL error: {}", SDL_GetError());
			return;
		}

		for (int i = 0; i < count; ++i)
			modeList.emplace_back(modes[i]->w, modes[i]->h, SDL_BITSPERPIXEL(modes[i]->format));
	}
}
