// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PLATFORM_VIDEOMODEIMPL_HPP
#define NAZARA_PLATFORM_VIDEOMODEIMPL_HPP

#include <vector>

class VideoMode;

class VideoModeImpl
{
	public:
		static VideoMode GetDesktopMode();
		static void GetFullscreenModes(std::vector<VideoMode>& modes);
};

#endif // NAZARA_PLATFORM_VIDEOMODEIMPL_HPP
