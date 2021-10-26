// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Platform/Cursor.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Platform/SDL2/CursorImpl.hpp>
#include <Nazara/Platform/Debug.hpp>

namespace Nz
{
	Cursor::Cursor() = default;

	Cursor::Cursor(const Image& cursor, const Vector2i& hotSpot, SystemCursor placeholder)
	{
		ErrorFlags flags(ErrorMode::ThrowException, true);
		Create(cursor, hotSpot, placeholder);
	}

	Cursor::Cursor(SystemCursor systemCursor)
	{
		ErrorFlags flags(ErrorMode::ThrowException, true);
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
			NazaraError(e.what());
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
			NazaraError(e.what());
			return false;
		}

		m_systemCursor = cursor;

		return true;
	}

	Cursor& Cursor::operator=(Cursor&&) noexcept = default;

	bool Cursor::Initialize()
	{
		for (std::size_t i = 0; i < SystemCursorCount; ++i)
		{
			s_systemCursors[i] = std::make_shared<Cursor>();
			s_systemCursors[i]->Create(static_cast<SystemCursor>(i));
		}

		return true;
	}

	void Cursor::Uninitialize()
	{
		for (std::shared_ptr<Cursor>& cursor : s_systemCursors)
			cursor.reset();
	}

	std::array<std::shared_ptr<Cursor>, SystemCursorCount> Cursor::s_systemCursors;
}
