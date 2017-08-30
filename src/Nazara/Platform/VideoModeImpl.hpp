// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VIDEOMODEIMPL_HPP
#define NAZARA_VIDEOMODEIMPL_HPP

#include <vector>

class VideoMode;

class VideoModeImpl
{
	public:
		static VideoMode GetDesktopMode();
		static void GetFullscreenModes(std::vector<VideoMode>& modes);
};

#endif // NAZARA_VIDEOMODEIMPL_HPP
