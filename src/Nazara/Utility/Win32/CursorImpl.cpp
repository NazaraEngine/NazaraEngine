// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Win32/CursorImpl.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Utility/Image.hpp>
#include <Nazara/Utility/PixelFormat.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	bool CursorImpl::Create(const Image& cursor, int hotSpotX, int hotSpotY)
	{
		Image windowsCursor(cursor);
		if (!windowsCursor.Convert(PixelFormatType_BGRA8))
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

		m_icon = CreateIconIndirect(&iconInfo);

		DeleteObject(bitmap);
		DeleteObject(monoBitmap);

		if (!m_icon)
		{
			NazaraError("Failed to create cursor: " + Error::GetLastSystemError());
			return false;
		}

		m_cursor = m_icon;

		return true;
	}

	bool CursorImpl::Create(SystemCursor cursor)
	{
		if (cursor != SystemCursor_Move)
			m_cursor = static_cast<HCURSOR>(LoadImage(nullptr, s_systemCursorIds[cursor], IMAGE_CURSOR, 0, 0, LR_SHARED));
		else
			m_cursor = nullptr;

		// No need to free the cursor if shared
		// http://msdn.microsoft.com/en-us/library/windows/desktop/ms648045(v=vs.85).aspx
		m_icon = nullptr;

		return true;
	}

	void CursorImpl::Destroy()
	{
		if (m_icon)
			DestroyIcon(m_icon);
	}

	HCURSOR CursorImpl::GetCursor()
	{
		return m_cursor;
	}

	bool CursorImpl::Initialize()
	{
		return true;
	}

	void CursorImpl::Uninitialize()
	{
	}

	std::array<LPTSTR, SystemCursor_Max + 1> CursorImpl::s_systemCursorIds =
	{
		IDC_CROSS,       // SystemCursor_Crosshair
		IDC_ARROW,       // SystemCursor_Default
		IDC_HAND,        // SystemCursor_Hand
		IDC_HELP,        // SystemCursor_Help
		IDC_SIZEALL,     // SystemCursor_Move
		nullptr,         // SystemCursor_None
		IDC_HAND,        // SystemCursor_Pointer
		IDC_APPSTARTING, // SystemCursor_Progress
		IDC_SIZEWE,      // SystemCursor_ResizeE
		IDC_SIZENS,      // SystemCursor_ResizeN
		IDC_SIZENESW,    // SystemCursor_ResizeNE
		IDC_SIZENWSE,    // SystemCursor_ResizeNW
		IDC_SIZENS,      // SystemCursor_ResizeS
		IDC_SIZENWSE,    // SystemCursor_ResizeSE
		IDC_SIZENESW,    // SystemCursor_ResizeSW
		IDC_SIZEWE,      // SystemCursor_ResizeW
		IDC_IBEAM,       // SystemCursor_Text
		IDC_WAIT         // SystemCursor_Wait
	};

	static_assert(SystemCursor_Max + 1 == 18, "System cursor array is incomplete");
}
