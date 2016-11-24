// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// Interface inspirée de la SFML par Laurent Gomila

#pragma once

#ifndef NAZARA_VIDEOMODE_HPP
#define NAZARA_VIDEOMODE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Utility/Config.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_UTILITY_API VideoMode
	{
		public:
			VideoMode();
			VideoMode(unsigned int w, unsigned int h);
			VideoMode(unsigned int w, unsigned int h, UInt8 bpp);

			bool IsFullscreenValid() const;

			UInt8 bitsPerPixel;
			unsigned int height;
			unsigned int width;

			static VideoMode GetDesktopMode();
			static const std::vector<VideoMode>& GetFullscreenModes();
	};

	bool operator==(const VideoMode& left, const VideoMode& right);
	bool operator!=(const VideoMode& left, const VideoMode& right);
	bool operator<(const VideoMode& left, const VideoMode& right);
	bool operator<=(const VideoMode& left, const VideoMode& right);
	bool operator>(const VideoMode& left, const VideoMode& right);
	bool operator>=(const VideoMode& left, const VideoMode& right);
}

#endif // NAZARA_VIDEOMODE_HPP
