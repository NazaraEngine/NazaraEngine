// Copyright (C) 2017 Jérôme Leclercq
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

	inline void TextAreaWidget::EnableMultiline(bool enable)
	{
		m_multiLineEnabled = enable;
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

	inline const Nz::Color& TextAreaWidget::GetTextColor() const
	{
		return m_drawer.GetColor();
	}

	inline bool Ndk::TextAreaWidget::IsMultilineEnabled() const
	{
		return m_multiLineEnabled;
	}

	inline bool TextAreaWidget::IsReadOnly() const
	{
		return m_readOnly;
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
		OnTextAreaCursorMove(this, &cursorPosition);

		m_cursorPosition = std::min(cursorPosition, m_drawer.GetGlyphCount());

		RefreshCursor();
	}

	inline void TextAreaWidget::SetReadOnly(bool readOnly)
	{
		m_readOnly = readOnly;

		m_cursorEntity->Enable(!m_readOnly);
	}

	inline void TextAreaWidget::SetText(const Nz::String& text)
	{
		m_drawer.SetText(text);

		m_textSprite->Update(m_drawer);

		SetCursorPosition(m_cursorPosition); //< Refresh cursor position (prevent it from being outside of the text)
	}

	inline void TextAreaWidget::SetTextColor(const Nz::Color& text)
	{
		m_drawer.SetColor(text);

		m_textSprite->Update(m_drawer);
	}
}
