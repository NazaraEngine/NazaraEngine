// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Cursor.hpp>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#include <Nazara/Utility/Win32/CursorImpl.hpp>
#elif defined(NAZARA_PLATFORM_LINUX)
	#include <Nazara/Utility/Linux/CursorImpl.hpp>
#else
	#error Lack of implementation: Cursor
#endif

#include <Nazara/Utility/Debug.hpp>

NzCursor::NzCursor() :
m_impl(nullptr)
{
}

NzCursor::~NzCursor()
{
	Destroy();
}

bool NzCursor::Create(const NzImage& cursor, int hotSpotX, int hotSpotY)
{
	Destroy();

	m_impl = new NzCursorImpl;
	if (!m_impl->Create(cursor, hotSpotX, hotSpotY))
	{
		NazaraError("Failed to create cursor implementation");
		delete m_impl;
		m_impl = nullptr;
		return false;
	}

	return true;
}

bool NzCursor::Create(const NzImage& cursor, const NzVector2i& hotSpot)
{
	return Create(cursor, hotSpot.x, hotSpot.y);
}

void NzCursor::Destroy()
{
	if (m_impl)
	{
		m_impl->Destroy();
		delete m_impl;
		m_impl = nullptr;
	}
}

bool NzCursor::IsValid() const
{
	return m_impl != nullptr;
}
