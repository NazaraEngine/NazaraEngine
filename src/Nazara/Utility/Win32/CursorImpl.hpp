// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CURSORIMPL_HPP
#define NAZARA_CURSORIMPL_HPP

#include <windows.h>

class NzImage;

class NzCursorImpl
{
	public:
		bool Create(const NzImage& image, int hotSpotX, int hotSpotY);
		void Destroy();

		HCURSOR GetCursor();

	private:
		HICON m_cursor = nullptr;
};

#endif // NAZARA_CURSORIMPL_HPP
