// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Win32/CursorImpl.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Utility/Image.hpp>
#include <Nazara/Utility/Debug.hpp>

bool NzCursorImpl::Create(const NzImage& cursor, int hotSpotX, int hotSpotY)
{
	NzImage windowsCursor(cursor);
	if (!windowsCursor.Convert(nzPixelFormat_BGRA8))
	{
		NazaraError("Failed to convert cursor to BGRA8");
		return false;
	}

	HBITMAP bitmap = CreateBitmap(windowsCursor.GetWidth(), windowsCursor.GetHeight(), 1, 32, windowsCursor.GetConstPixels());
	HBITMAP monoBitmap = CreateBitmap(windowsCursor.GetWidth(), windowsCursor.GetHeight(), 1, 1, nullptr);

	// http://msdn.microsoft.com/en-us/library/windows/desktop/ms648052(v=vs.85).aspx
	ICONINFO iconInfo;
	iconInfo.fIcon = FALSE;
	iconInfo.xHotspot = hotSpotX;
	iconInfo.yHotspot = hotSpotY;
	iconInfo.hbmMask = monoBitmap;
	iconInfo.hbmColor = bitmap;

	m_cursor = CreateIconIndirect(&iconInfo);

	DeleteObject(bitmap);
	DeleteObject(monoBitmap);

	if (!m_cursor)
	{
		NazaraError("Failed to create cursor: " + NzGetLastSystemError());
		return false;
	}

	return true;
}

void NzCursorImpl::Destroy()
{
	DestroyIcon(m_cursor);
}

HCURSOR NzCursorImpl::GetCursor()
{
	return m_cursor;
}
