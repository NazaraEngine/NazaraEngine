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

	inline const Nz::Vector2ui& TextAreaWidget::GetCursorPosition() const
	{
		return m_cursorPosition;
	}

	inline std::size_t Ndk::TextAreaWidget::GetGlyphUnderCursor() const
	{
		return m_cursorGlyph;
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
			SetCursorPosition(m_cursorGlyph + static_cast<std::size_t>(offset));
		else
		{
			std::size_t nOffset = static_cast<std::size_t>(-offset);
			if (nOffset >= m_cursorGlyph)
				SetCursorPosition(0);
			else
				SetCursorPosition(m_cursorGlyph - nOffset);
		}
	}

	inline void TextAreaWidget::MoveCursor(const Nz::Vector2i& offset)
	{
		auto BoundOffset = [] (unsigned int cursorPosition, int offset) -> unsigned int
		{
			if (offset >= 0)
				return cursorPosition + offset;
			else
			{
				unsigned int nOffset = static_cast<unsigned int>(-offset);
				if (nOffset >= cursorPosition)
					return 0;
				else
					return cursorPosition - nOffset;
			}
		};

		Nz::Vector2ui cursorPosition = m_cursorPosition;
		cursorPosition.x = BoundOffset(static_cast<unsigned int>(cursorPosition.x), offset.x);
		cursorPosition.y = BoundOffset(static_cast<unsigned int>(cursorPosition.y), offset.y);

		SetCursorPosition(cursorPosition);
	}

	inline void TextAreaWidget::SetCursorPosition(std::size_t glyphIndex)
	{
		OnTextAreaCursorMove(this, &glyphIndex);

		m_cursorGlyph = std::min(glyphIndex, m_drawer.GetGlyphCount());
		
		std::size_t lineCount = m_drawer.GetLineCount();
		std::size_t line = 0U;
		for (std::size_t i = line + 1; i < lineCount; ++i)
		{
			if (m_drawer.GetLine(i).glyphIndex > m_cursorGlyph)
				break;

			line = i;
		}

		const auto& lineInfo = m_drawer.GetLine(line);

		m_cursorPosition.y = line;
		m_cursorPosition.x = m_cursorGlyph - lineInfo.glyphIndex;

		RefreshCursor();
	}

	inline void TextAreaWidget::SetCursorPosition(Nz::Vector2ui cursorPosition)
	{
		std::size_t lineCount = m_drawer.GetLineCount();
		if (cursorPosition.y >= lineCount)
			cursorPosition.y = static_cast<unsigned int>(lineCount - 1);

		m_cursorPosition = cursorPosition;

		const auto& lineInfo = m_drawer.GetLine(cursorPosition.y);
		if (cursorPosition.y + 1 < lineCount)
		{
			const auto& nextLineInfo = m_drawer.GetLine(cursorPosition.y + 1);
			cursorPosition.x = std::min(cursorPosition.x, static_cast<unsigned int>(nextLineInfo.glyphIndex - lineInfo.glyphIndex - 1));
		}

		std::size_t glyphIndex = lineInfo.glyphIndex + cursorPosition.x;

		OnTextAreaCursorMove(this, &glyphIndex);

		m_cursorGlyph = std::min(glyphIndex, m_drawer.GetGlyphCount());

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
