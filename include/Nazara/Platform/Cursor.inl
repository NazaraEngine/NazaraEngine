// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Platform/Cursor.hpp>
#include <Nazara/Platform/Debug.hpp>

namespace Nz
{
	inline SystemCursor Cursor::GetSystemCursor() const
	{
		NazaraAssert(IsValid(), "Invalid cursor");

		return m_systemCursor;
	}

	inline bool Cursor::IsValid() const
	{
		return m_impl != nullptr;
	}

	inline std::shared_ptr<Cursor>& Cursor::Get(SystemCursor cursor)
	{
		return s_systemCursors[UnderlyingCast(cursor)];
	}
}

#include <Nazara/Platform/DebugOff.hpp>
