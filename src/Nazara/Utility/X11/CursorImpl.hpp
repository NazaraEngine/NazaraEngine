// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CURSORIMPL_HPP
#define NAZARA_CURSORIMPL_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <xcb/xcb_cursor.h>

namespace Nz
{
	class Image;

	class CursorImpl
	{
		friend class Cursor;

		public:
			bool Create(const Image& image, int hotSpotX, int hotSpotY);
			bool Create(SystemCursor cursor);
			
			void Destroy();

			xcb_cursor_t GetCursor();

		private:
			static bool Initialize();
			static void Uninitialize();

			xcb_cursor_t m_cursor = 0;
			xcb_cursor_context_t* m_cursorContext = nullptr;
			
			static std::array<const char*, SystemCursor_Max + 1> s_systemCursorIds;
	};
}

#endif // NAZARA_CURSORIMPL_HPP
