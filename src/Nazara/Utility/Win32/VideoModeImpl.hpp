// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VIDEOMODEIMPL_HPP
#define NAZARA_VIDEOMODEIMPL_HPP

#include <Nazara/Utility/VideoMode.hpp>

class NzVideoModeImpl
{
	public:
		static NzVideoMode GetDesktopMode();
		static void GetFullscreenModes(std::vector<NzVideoMode>& modes);
};

#endif // NNAZARA_VIDEOMODEIMPL_HPP
