// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Cursor.hpp>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#include <Nazara/Utility/Win32/CursorImpl.hpp>
#elif defined(NAZARA_PLATFORM_X11)
	#include <Nazara/Utility/X11/CursorImpl.hpp>
#else
	#error Lack of implementation: Cursor
#endif

#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	bool Cursor::Create(const Image& cursor, int hotSpotX, int hotSpotY)
	{
		Destroy();

		m_impl = new CursorImpl;
		if (!m_impl->Create(cursor, hotSpotX, hotSpotY))
		{
			NazaraError("Failed to create cursor implementation");
			delete m_impl;
			m_impl = nullptr;

			return false;
		}

		m_cursorImage = cursor;

		return true;
	}

	bool Cursor::Create(const Image& cursor, const Vector2i& hotSpot)
	{
		return Create(cursor, hotSpot.x, hotSpot.y);
	}

	void Cursor::Destroy()
	{
		if (m_impl)
		{
			m_impl->Destroy();

			delete m_impl;
			m_impl = nullptr;
		}
	}
}
