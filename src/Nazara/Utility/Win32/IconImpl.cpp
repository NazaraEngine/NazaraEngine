// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Win32/IconImpl.hpp>
#include <Nazara/Utility/Image.hpp>
#include <Nazara/Utility/Debug.hpp>

bool NzIconImpl::Create(const NzImage& icon)
{
	NzImage windowsIcon(icon); // Vive le COW
	if (!windowsIcon.Convert(nzPixelFormat_BGRA8))
	{
		NazaraError("Failed to convert icon to BGRA8");
		return false;
	}

	HBITMAP bitmap = CreateBitmap(windowsIcon.GetWidth(), windowsIcon.GetHeight(), 1, 32, windowsIcon.GetConstPixels());
	HBITMAP monoBitmap = CreateBitmap(windowsIcon.GetWidth(), windowsIcon.GetHeight(), 1, 1, nullptr);

	// http://msdn.microsoft.com/en-us/library/windows/desktop/ms648052(v=vs.85).aspx
	ICONINFO iconInfo;
	iconInfo.fIcon = TRUE;
	iconInfo.hbmMask = monoBitmap;
	iconInfo.hbmColor = bitmap;

	m_icon = CreateIconIndirect(&iconInfo);

	DeleteObject(bitmap);
	DeleteObject(monoBitmap);

	if (!m_icon)
	{
		NazaraError("Failed to create icon: " + NzError::GetLastSystemError());
		return false;
	}

	return true;
}

void NzIconImpl::Destroy()
{
	DestroyIcon(m_icon);
}

HICON NzIconImpl::GetIcon()
{
	return m_icon;
}
