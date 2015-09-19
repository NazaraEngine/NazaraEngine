// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CURSORIMPL_HPP
#define NAZARA_CURSORIMPL_HPP

#include <xcb/xcb_cursor.h>

class NzImage;

class NzCursorImpl
{
	public:
		bool Create(const NzImage& image, int hotSpotX, int hotSpotY);
		void Destroy();

		xcb_cursor_t GetCursor();

	private:
		xcb_cursor_t m_cursor;
};

#endif // NAZARA_CURSORIMPL_HPP
