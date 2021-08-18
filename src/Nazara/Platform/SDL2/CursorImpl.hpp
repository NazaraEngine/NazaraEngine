// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CURSORIMPL_HPP
#define NAZARA_CURSORIMPL_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/MovablePtr.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Platform/Enums.hpp>
#include <Nazara/Utility/Image.hpp>
#include <SDL2/SDL_mouse.h>

namespace Nz
{
	class CursorImpl
	{
		public:
			CursorImpl(const Image& image, const Vector2i& hotSpot);
			CursorImpl(SystemCursor cursor);
			CursorImpl(const CursorImpl&) = delete;
			CursorImpl(CursorImpl&&) noexcept = default;
			~CursorImpl();

			SDL_Cursor* GetCursor();

			CursorImpl& operator=(const CursorImpl&) = delete;
			CursorImpl& operator=(CursorImpl&&) noexcept = default;

		private:
			MovablePtr<SDL_Cursor> m_cursor;
			MovablePtr<SDL_Surface> m_surface;
			Image m_cursorImage;
	};
}

#endif // NAZARA_CURSORIMPL_HPP
