// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	inline SystemCursor Cursor::GetSystemCursor() const
	{
		NazaraAssertMsg(IsValid(), "Invalid cursor");

		return m_systemCursor;
	}

	inline bool Cursor::IsValid() const
	{
		return m_impl != nullptr;
	}

	inline std::shared_ptr<Cursor>& Cursor::Get(SystemCursor cursor)
	{
		return s_systemCursors[cursor];
	}
}

