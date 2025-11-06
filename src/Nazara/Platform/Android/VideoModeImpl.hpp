// Copyright (C) 2025 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_PLATFORM_ANDROID_VIDEOMODEIMPL_HPP
#define NAZARA_PLATFORM_ANDROID_VIDEOMODEIMPL_HPP

#include <Nazara/Platform/VideoMode.hpp>

namespace Nz
{
	class VideoModeImpl
	{
		public:
			static VideoMode GetDesktopMode();
			static void GetFullscreenModes(std::vector<VideoMode>& modes);
	};
}

#endif // NAZARA_PLATFORM_ANDROID_VIDEOMODEIMPL_HPP
