// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// Interface inspirée de la SFML par Laurent Gomila

#pragma once

#ifndef NAZARA_VIDEOMODE_HPP
#define NAZARA_VIDEOMODE_HPP

#include <Nazara/Prerequesites.hpp>
#include <vector>

class NAZARA_API NzVideoMode
{
	public:
		NzVideoMode();
		NzVideoMode(unsigned int w, unsigned int h, nzUInt8 bpp);

		bool IsFullscreenValid() const;

		nzUInt8 bitsPerPixel;
		unsigned int height;
		unsigned int width;

		static NzVideoMode GetDesktopMode();
		static const std::vector<NzVideoMode>& GetFullscreenModes();
};

bool operator==(const NzVideoMode& left, const NzVideoMode& right);
bool operator!=(const NzVideoMode& left, const NzVideoMode& right);
bool operator<(const NzVideoMode& left, const NzVideoMode& right);
bool operator<=(const NzVideoMode& left, const NzVideoMode& right);
bool operator>(const NzVideoMode& left, const NzVideoMode& right);
bool operator>=(const NzVideoMode& left, const NzVideoMode& right);

#endif // NAZARA_VIDEOMODE_HPP
