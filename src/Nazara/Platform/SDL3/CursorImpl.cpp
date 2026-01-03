// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Platform/SDL3/CursorImpl.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/PixelFormat.hpp>
#include <array>

namespace Nz
{
	namespace NAZARA_ANONYMOUS_NAMESPACE
	{
		constexpr EnumArray<SystemCursor, SDL_SystemCursor> s_systemCursorIds =
		{
			SDL_SYSTEM_CURSOR_CROSSHAIR, // SystemCursor::Crosshair
			SDL_SYSTEM_CURSOR_DEFAULT,   // SystemCursor::Default
			SDL_SYSTEM_CURSOR_POINTER,   // SystemCursor::Hand
			SDL_SYSTEM_CURSOR_DEFAULT,   // SystemCursor::Help
			SDL_SYSTEM_CURSOR_MOVE,      // SystemCursor::Move
			SDL_SYSTEM_CURSOR_COUNT,     // SystemCursor::None
			SDL_SYSTEM_CURSOR_POINTER,   // SystemCursor::Pointer
			SDL_SYSTEM_CURSOR_PROGRESS,  // SystemCursor::Progress
			SDL_SYSTEM_CURSOR_E_RESIZE,  // SystemCursor::ResizeE
			SDL_SYSTEM_CURSOR_N_RESIZE,  // SystemCursor::ResizeN
			SDL_SYSTEM_CURSOR_NE_RESIZE, // SystemCursor::ResizeNE
			SDL_SYSTEM_CURSOR_NW_RESIZE, // SystemCursor::ResizeNW
			SDL_SYSTEM_CURSOR_S_RESIZE,  // SystemCursor::ResizeS
			SDL_SYSTEM_CURSOR_SE_RESIZE, // SystemCursor::ResizeSE
			SDL_SYSTEM_CURSOR_SW_RESIZE, // SystemCursor::ResizeSW
			SDL_SYSTEM_CURSOR_W_RESIZE,  // SystemCursor::ResizeW
			SDL_SYSTEM_CURSOR_TEXT,      // SystemCursor::Text
			SDL_SYSTEM_CURSOR_WAIT       // SystemCursor::Wait
		};

		static_assert(SystemCursorCount == 18, "System cursor array is incomplete");
	}

	CursorImpl::CursorImpl(const Image& cursor, const Vector2i& hotSpot)
	{
		ErrorFlags errFlags(ErrorMode::ThrowException);

		m_cursorImage = cursor;
		if (!m_cursorImage.Convert(PixelFormat::BGRA8))
			NazaraError("failed to convert icon to BGRA8");

		m_surface = SDL_CreateSurfaceFrom(
			m_cursorImage.GetWidth(),
			m_cursorImage.GetHeight(),
			SDL_PIXELFORMAT_BGRA32,
			m_cursorImage.GetPixels(),
			4 * m_cursorImage.GetWidth()
		);

		if (!m_surface)
			NazaraError("failed to create SDL Surface for cursor: {0}", SDL_GetError());

		m_cursor = SDL_CreateColorCursor(m_surface, hotSpot.x, hotSpot.y);
		if (!m_cursor)
		{
			if (m_surface) //< Just in case exceptions were disabled
				SDL_DestroySurface(m_surface);

			NazaraError("failed to create SDL cursor: {0}", SDL_GetError());
		}
	}

	CursorImpl::CursorImpl(SystemCursor cursor)
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		ErrorFlags errFlags(ErrorMode::ThrowException);

		if (cursor != SystemCursor::None)
		{
			m_cursor = SDL_CreateSystemCursor(s_systemCursorIds[cursor]);
			if (!m_cursor)
				NazaraError("failed to create SDL cursor: {0}", SDL_GetError());
		}
	}

	CursorImpl::~CursorImpl()
	{
		if (m_surface)
			SDL_DestroySurface(m_surface);

		if (m_cursor)
			SDL_DestroyCursor(m_cursor);
	}

	SDL_Cursor* CursorImpl::GetCursor()
	{
		return m_cursor;
	}
}
