// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Cursor.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	inline Cursor::Cursor() :
	m_impl(nullptr),
	m_usesSystemCursor(false)
	{
	}

	inline Cursor::Cursor(SystemCursor systemCursor) :
	Cursor()
	{
		ErrorFlags flags(ErrorFlag_ThrowException, true);
		Create(systemCursor);
	}

	inline Cursor::Cursor(Cursor&& cursor) noexcept :
	Cursor()
	{
		operator=(std::move(cursor));
	}

	inline Cursor::~Cursor()
	{
		Destroy();
	}

	inline const Image& Cursor::GetImage() const
	{
		NazaraAssert(IsValid(), "Invalid cursor");
		NazaraAssert(!m_usesSystemCursor, "System cursors have no image");

		return m_cursorImage;
	}

	inline SystemCursor Cursor::GetSystemCursor() const
	{
		NazaraAssert(IsValid(), "Invalid cursor");
		NazaraAssert(m_usesSystemCursor, "Custom cursor uses an image");

		return m_systemCursor;
	}

	inline bool Cursor::IsValid() const
	{
		return m_impl != nullptr;
	}

	inline Cursor& Cursor::operator=(Cursor&& cursor)
	{
		m_cursorImage = std::move(cursor.m_cursorImage);
		m_systemCursor = cursor.m_systemCursor;
		m_impl = cursor.m_impl;
		m_usesSystemCursor = cursor.m_usesSystemCursor;

		cursor.m_impl = nullptr;

		return *this;
	}
}

#include <Nazara/Utility/DebugOff.hpp>
