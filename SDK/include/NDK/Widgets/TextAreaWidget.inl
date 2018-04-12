// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NDK/Widgets/TextAreaWidget.hpp>

namespace Ndk
{
	inline void TextAreaWidget::Clear()
	{
		m_cursorPosition.MakeZero();
		m_drawer.Clear();
		m_text.Clear();
		m_textSprite->Update(m_drawer);

		RefreshCursor();
		OnTextChanged(this, m_text);
	}

	inline void TextAreaWidget::EnableMultiline(bool enable)
	{
		m_multiLineEnabled = enable;
	}

	inline unsigned int TextAreaWidget::GetCharacterSize() const
	{
		return m_drawer.GetCharacterSize();
	}

	inline const Nz::Vector2ui& TextAreaWidget::GetCursorPosition() const
	{
		return m_cursorPosition;
	}

	inline const Nz::String& TextAreaWidget::GetDisplayText() const
	{
		return m_drawer.GetText();
	}

	inline EchoMode TextAreaWidget::GetEchoMode() const
	{
		return m_echoMode;
	}

	inline std::size_t TextAreaWidget::GetGlyphIndex() const
	{
		return GetGlyphIndex(m_cursorPosition);
	}

	inline std::size_t TextAreaWidget::GetGlyphIndex(const Nz::Vector2ui& cursorPosition) const
	{
		std::size_t glyphIndex = m_drawer.GetLine(cursorPosition.y).glyphIndex + cursorPosition.x;
		if (m_drawer.GetLineCount() > cursorPosition.y + 1)
			glyphIndex = std::min(glyphIndex, m_drawer.GetLine(cursorPosition.y + 1).glyphIndex - 1);
		else
			glyphIndex = std::min(glyphIndex, m_drawer.GetGlyphCount());

		return glyphIndex;
	}

	inline const Nz::String& TextAreaWidget::GetText() const
	{
		return m_text;
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
		std::size_t cursorGlyph = GetGlyphIndex(m_cursorPosition);
		if (offset >= 0)
			SetCursorPosition(cursorGlyph + static_cast<std::size_t>(offset));
		else
		{
			std::size_t nOffset = static_cast<std::size_t>(-offset);
			if (nOffset >= cursorGlyph)
				SetCursorPosition(0);
			else
				SetCursorPosition(cursorGlyph - nOffset);
		}
	}

	inline void TextAreaWidget::MoveCursor(const Nz::Vector2i& offset)
	{
		auto ClampOffset = [] (unsigned int cursorPosition, int cursorOffset) -> unsigned int
		{
			if (cursorOffset >= 0)
				return cursorPosition + cursorOffset;
			else
			{
				unsigned int nOffset = static_cast<unsigned int>(-cursorOffset);
				if (nOffset >= cursorPosition)
					return 0;
				else
					return cursorPosition - nOffset;
			}
		};

		Nz::Vector2ui cursorPosition = m_cursorPosition;
		cursorPosition.x = ClampOffset(static_cast<unsigned int>(cursorPosition.x), offset.x);
		cursorPosition.y = ClampOffset(static_cast<unsigned int>(cursorPosition.y), offset.y);

		SetCursorPosition(cursorPosition);
	}

	inline void TextAreaWidget::SetCharacterSize(unsigned int characterSize)
	{
		m_drawer.SetCharacterSize(characterSize);

		UpdateDisplayText();
	}

	inline void TextAreaWidget::SetCursorPosition(std::size_t glyphIndex)
	{
		OnTextAreaCursorMove(this, &glyphIndex);

		glyphIndex = std::min(glyphIndex, m_drawer.GetGlyphCount());

		std::size_t lineCount = m_drawer.GetLineCount();
		std::size_t line = 0U;
		for (std::size_t i = line + 1; i < lineCount; ++i)
		{
			if (m_drawer.GetLine(i).glyphIndex > glyphIndex)
				break;

			line = i;
		}

		const auto& lineInfo = m_drawer.GetLine(line);

		m_cursorPosition.y = static_cast<unsigned int>(line);
		m_cursorPosition.x = static_cast<unsigned int>(glyphIndex - lineInfo.glyphIndex);

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

		RefreshCursor();
	}

	inline void TextAreaWidget::SetEchoMode(EchoMode echoMode)
	{
		m_echoMode = echoMode;

		UpdateDisplayText();
	}

	inline void TextAreaWidget::SetReadOnly(bool readOnly)
	{
		m_readOnly = readOnly;
		m_cursorEntity->Enable(!m_readOnly && HasFocus());
	}

	inline void TextAreaWidget::SetText(const Nz::String& text)
	{
		m_text = text;
		OnTextChanged(this, m_text);

		UpdateDisplayText();
	}

	inline void TextAreaWidget::SetTextColor(const Nz::Color& text)
	{
		m_drawer.SetColor(text);

		m_textSprite->Update(m_drawer);
	}
}
