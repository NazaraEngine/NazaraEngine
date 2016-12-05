// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Widgets/TextAreaWidget.hpp>

namespace Ndk
{
	inline void TextAreaWidget::Clear()
	{
		m_cursorPosition = 0;
		m_drawer.Clear();
		m_textSprite->Update(m_drawer);

		RefreshCursor();
	}

	inline std::size_t TextAreaWidget::GetCursorPosition() const
	{
		return m_cursorPosition;
	}

	inline std::size_t TextAreaWidget::GetLineCount() const
	{
		return m_drawer.GetLineCount();
	}

	inline const Nz::String& TextAreaWidget::GetText() const
	{
		return m_drawer.GetText();
	}

	inline void TextAreaWidget::MoveCursor(int offset)
	{
		if (offset >= 0)
			SetCursorPosition(m_cursorPosition += static_cast<std::size_t>(offset));
		else
		{
			std::size_t nOffset = static_cast<std::size_t>(-offset);
			if (nOffset >= m_cursorPosition)
				SetCursorPosition(0);
			else
				SetCursorPosition(m_cursorPosition - nOffset);
		}
	}

	inline void TextAreaWidget::SetCursorPosition(std::size_t cursorPosition)
	{
		m_cursorPosition = std::min(cursorPosition, m_drawer.GetGlyphCount());

		RefreshCursor();
	}
}
