// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Win32/VideoModeImpl.hpp>
#include <Nazara/Utility/VideoMode.hpp>
#include <algorithm>
#include <windows.h>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	VideoMode VideoModeImpl::GetDesktopMode()
	{
		DEVMODE mode;
		mode.dmSize = sizeof(DEVMODE);
		EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &mode);

		return VideoMode(mode.dmPelsWidth, mode.dmPelsHeight, static_cast<UInt8>(mode.dmBitsPerPel));
	}

	void VideoModeImpl::GetFullscreenModes(std::vector<VideoMode>& modes)
	{
		DEVMODE win32Mode;
		win32Mode.dmSize = sizeof(DEVMODE);
		for (unsigned int i = 0; EnumDisplaySettings(nullptr, i, &win32Mode); ++i)
		{
			VideoMode mode(win32Mode.dmPelsWidth, win32Mode.dmPelsHeight, static_cast<UInt8>(win32Mode.dmBitsPerPel));

			// Il existe plusieurs modes avec ces trois caractéristques identiques
			if (std::find(modes.begin(), modes.end(), mode) == modes.end())
				modes.push_back(mode);
		}
}
}
