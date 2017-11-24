// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VIDEOMODEIMPL_HPP
#define NAZARA_VIDEOMODEIMPL_HPP

#include <Nazara/Prerequesites.hpp>
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

#endif // NNAZARA_VIDEOMODEIMPL_HPP
