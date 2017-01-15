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
	bool Cursor::Create(const Image& cursor, const Vector2i& hotSpot)
	{
		Destroy();

		std::unique_ptr<CursorImpl> impl(new CursorImpl);
		if (!impl->Create(cursor, hotSpot.x, hotSpot.y))
		{
			NazaraError("Failed to create cursor implementation");
			return false;
		}

		m_cursorImage = cursor;
		m_impl = impl.release();

		return true;
	}

	inline bool Cursor::Create(SystemCursor cursor)
	{
		Destroy();

		std::unique_ptr<CursorImpl> impl(new CursorImpl);
		if (!impl->Create(cursor))
		{
			NazaraError("Failed to create cursor implementation");
			return false;
		}

		m_impl = impl.release();
		m_systemCursor = cursor;
		m_usesSystemCursor = true;

		return true;
	}

	void Cursor::Destroy()
	{
		if (m_impl)
		{
			m_impl->Destroy();

			delete m_impl;
			m_impl = nullptr;
		}

		m_usesSystemCursor = false;
	}

	bool Cursor::Initialize()
	{
		return CursorImpl::Initialize();
	}

	void Cursor::Uninitialize()
	{
		CursorImpl::Uninitialize();
	}
}
