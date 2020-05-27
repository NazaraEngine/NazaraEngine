// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Platform/Debug.hpp>
#include <Nazara/Platform/SDL2/CursorImpl.hpp>
#include <Nazara/Utility/Image.hpp>
#include <Nazara/Utility/PixelFormat.hpp>

namespace Nz
{
	bool CursorImpl::Create(const Image& cursor, int hotSpotX, int hotSpotY)
	{
		m_iconImage = cursor;
		if (!m_iconImage.Convert(PixelFormat_BGRA8))
		{
			NazaraError("Failed to convert icon to BGRA8");
			return false;
		}

		m_icon = SDL_CreateRGBSurfaceWithFormatFrom(
			m_iconImage.GetPixels(),
			m_iconImage.GetWidth(),
			m_iconImage.GetHeight(),
			32,
			32 * m_iconImage.GetWidth(),
			SDL_PIXELFORMAT_BGRA8888
			);

		if (!m_icon)
		{
			NazaraError(SDL_GetError());

			return false;
		}

		m_cursor = SDL_CreateColorCursor(m_icon, hotSpotX, hotSpotY);

		if (!m_cursor)
		{
			NazaraError(SDL_GetError());

			return false;
		}

		return true;
	}

	bool CursorImpl::Create(SystemCursor cursor)
	{
		if (cursor != SystemCursor_None)
			m_cursor = SDL_CreateSystemCursor(s_systemCursorIds[cursor]);
		else
			m_cursor = nullptr;

		m_icon = nullptr;

		return true;
	}

	void CursorImpl::Destroy()
	{
		if (m_icon)
			SDL_FreeSurface(m_icon);

		if (m_cursor)
			SDL_FreeCursor(m_cursor);
	}

	SDL_Cursor* CursorImpl::GetCursor()
	{
		return m_cursor;
	}

	bool CursorImpl::Initialize()
	{
		return true;
	}

	void CursorImpl::Uninitialize()
	{
	}

	std::array<SDL_SystemCursor, SystemCursor_Max + 1> CursorImpl::s_systemCursorIds =
	{
		SDL_SYSTEM_CURSOR_CROSSHAIR,                     // SystemCursor_Crosshair
		SDL_SYSTEM_CURSOR_ARROW,                         // SystemCursor_Default
		SDL_SYSTEM_CURSOR_HAND,                          // SystemCursor_Hand
		SDL_SYSTEM_CURSOR_ARROW,                         // SystemCursor_Help
		SDL_SYSTEM_CURSOR_SIZEALL,                       // SystemCursor_Move
		SDL_NUM_SYSTEM_CURSORS,                          // SystemCursor_None
		SDL_SYSTEM_CURSOR_HAND,                          // SystemCursor_Pointer
		SDL_SYSTEM_CURSOR_WAITARROW,                     // SystemCursor_Progress
		SDL_SYSTEM_CURSOR_SIZEWE,                        // SystemCursor_ResizeE
		SDL_SYSTEM_CURSOR_SIZENS,                        // SystemCursor_ResizeN
		SDL_SYSTEM_CURSOR_SIZENESW,                      // SystemCursor_ResizeNE
		SDL_SYSTEM_CURSOR_SIZENWSE,                      // SystemCursor_ResizeNW
		SDL_SYSTEM_CURSOR_SIZENS,                        // SystemCursor_ResizeS
		SDL_SYSTEM_CURSOR_SIZENWSE,                      // SystemCursor_ResizeSE
		SDL_SYSTEM_CURSOR_SIZENESW,                      // SystemCursor_ResizeSW
		SDL_SYSTEM_CURSOR_SIZEWE,                        // SystemCursor_ResizeW
		SDL_SYSTEM_CURSOR_IBEAM,                         // SystemCursor_Text
		SDL_SYSTEM_CURSOR_WAIT                           // SystemCursor_Wait
	};

	static_assert(SystemCursor_Max + 1 == 18, "System cursor array is incomplete");
}
