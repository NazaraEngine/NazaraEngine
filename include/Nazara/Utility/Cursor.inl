// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Cursor.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	inline Cursor::Cursor() :
	m_impl(nullptr)
	{
	}

	inline Cursor::Cursor(const Image& cursor, const Vector2i& hotSpot, SystemCursor placeholder)
	{
		ErrorFlags flags(ErrorFlag_ThrowException, true);
		Create(cursor, hotSpot, placeholder);
	}

	inline Cursor* Cursor::Get(SystemCursor cursor)
	{
		return &s_systemCursors[cursor];
	}

	inline Cursor::Cursor(SystemCursor systemCursor) :
	Cursor()
	{
		ErrorFlags flags(ErrorFlag_ThrowException, true);
		Create(systemCursor);
	}

	inline Cursor::~Cursor()
	{
		Destroy();
	}

	inline const Image& Cursor::GetImage() const
	{
		NazaraAssert(IsValid(), "Invalid cursor");
		NazaraAssert(m_cursorImage.IsValid(), "System cursors have no image");

		return m_cursorImage;
	}

	inline SystemCursor Cursor::GetSystemCursor() const
	{
		NazaraAssert(IsValid(), "Invalid cursor");

		return m_systemCursor;
	}

	inline bool Cursor::IsValid() const
	{
		return m_impl != nullptr;
	}

	template<typename... Args>
	CursorRef Cursor::New(Args&&... args)
	{
		std::unique_ptr<Cursor> object(new Cursor(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}
}

#include <Nazara/Utility/DebugOff.hpp>
