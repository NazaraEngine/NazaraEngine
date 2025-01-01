// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	inline void CursorController::UpdateCursor(const std::shared_ptr<Cursor>& cursor)
	{
		OnCursorUpdated(this, cursor);
	}
}
