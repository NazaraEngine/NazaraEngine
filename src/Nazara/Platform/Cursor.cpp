// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Platform/Cursor.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Platform/SDL3/CursorImpl.hpp>

namespace Nz
{
	Cursor::Cursor() = default;

	Cursor::Cursor(const Image& cursor, const Vector2i& hotSpot, SystemCursor placeholder)
	{
		ErrorFlags flags(ErrorMode::ThrowException);
		Create(cursor, hotSpot, placeholder);
	}

	Cursor::Cursor(SystemCursor systemCursor)
	{
		ErrorFlags flags(ErrorMode::ThrowException);
		Create(systemCursor);
	}

	Cursor::Cursor(Cursor&&) noexcept = default;
	Cursor::~Cursor() = default;

	bool Cursor::Create(const Image& cursor, const Vector2i& hotSpot, SystemCursor placeholder)
	{
		Destroy();

		try
		{
			m_impl = std::make_unique<CursorImpl>(cursor, hotSpot);
		}
		catch (const std::exception& e)
		{
			NazaraError("{}", e.what());
			return false;
		}

		m_systemCursor = placeholder;

		return true;
	}

	void Cursor::Destroy()
	{
		m_impl.reset();
	}

	bool Cursor::Create(SystemCursor cursor)
	{
		Destroy();

		try
		{
			m_impl = std::make_unique<CursorImpl>(cursor);
		}
		catch (const std::exception& e)
		{
			NazaraError("{}", e.what());
			return false;
		}

		m_systemCursor = cursor;

		return true;
	}

	Cursor& Cursor::operator=(Cursor&&) noexcept = default;

	bool Cursor::Initialize()
	{
		for (auto&& [cursor, cursorPtr] : s_systemCursors.iter_kv())
		{
			cursorPtr = std::make_shared<Cursor>();
			cursorPtr->Create(cursor);
		}

		return true;
	}

	void Cursor::Uninitialize()
	{
		for (std::shared_ptr<Cursor>& cursor : s_systemCursors)
			cursor.reset();
	}

	EnumArray<SystemCursor, std::shared_ptr<Cursor>> Cursor::s_systemCursors;
}
