// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NDK/Widgets/TextAreaWidget.hpp>
#include <Nazara/Core/Unicode.hpp>
#include <Nazara/Utility/Font.hpp>
#include <NDK/Components/GraphicsComponent.hpp>
#include <NDK/Components/NodeComponent.hpp>

namespace Ndk
{
	TextAreaWidget::TextAreaWidget(BaseWidget* parent) :
	AbstractTextAreaWidget(parent)
	{
		SetCharacterSize(GetCharacterSize()); //< Actualize minimum / preferred size

		Layout();
	}
	
	void TextAreaWidget::AppendText(const Nz::String& text)
	{
		m_text += text;
		switch (m_echoMode)
		{
			case EchoMode_Normal:
				m_drawer.AppendText(text);
				break;

			case EchoMode_Password:
				m_drawer.AppendText(Nz::String(text.GetLength(), '*'));
				break;

			case EchoMode_PasswordExceptLast:
			{
				m_drawer.Clear();
				std::size_t textLength = m_text.GetLength();
				if (textLength >= 2)
				{
					std::size_t lastCharacterPosition = m_text.GetCharacterPosition(textLength - 2);
					if (lastCharacterPosition != Nz::String::npos)
						m_drawer.AppendText(Nz::String(textLength - 1, '*'));
				}

				if (textLength >= 1)
					m_drawer.AppendText(m_text.SubString(m_text.GetCharacterPosition(textLength - 1)));

				break;
			}
		}

		UpdateTextSprite();

		OnTextChanged(this, m_text);
	}

	void TextAreaWidget::Clear()
	{
		AbstractTextAreaWidget::Clear();

		m_text.Clear();
		OnTextChanged(this, m_text);
	}
	
	void TextAreaWidget::Erase(std::size_t firstGlyph, std::size_t lastGlyph)
	{
		if (firstGlyph > lastGlyph)
			std::swap(firstGlyph, lastGlyph);

		std::size_t textLength = m_text.GetLength();
		if (firstGlyph > textLength)
			return;

		Nz::String newText;
		if (firstGlyph > 0)
		{
			std::size_t characterPosition = m_text.GetCharacterPosition(firstGlyph - 1);
			NazaraAssert(characterPosition != Nz::String::npos, "Invalid character position");

			newText.Append(m_text.SubString(0, characterPosition));
		}

		if (lastGlyph < textLength)
		{
			std::size_t characterPosition = m_text.GetCharacterPosition(lastGlyph);
			NazaraAssert(characterPosition != Nz::String::npos, "Invalid character position");

			newText.Append(m_text.SubString(characterPosition));
		}

		SetText(newText);
	}

	void TextAreaWidget::SetCharacterSize(unsigned int characterSize)
	{
		m_drawer.SetCharacterSize(characterSize);

		std::size_t fontCount = m_drawer.GetFontCount();
		unsigned int lineHeight = 0;
		int spaceAdvance = 0;
		for (std::size_t i = 0; i < fontCount; ++i)
		{
			Nz::Font* font = m_drawer.GetFont(i);

			const Nz::Font::SizeInfo& sizeInfo = font->GetSizeInfo(characterSize);
			lineHeight = std::max(lineHeight, sizeInfo.lineHeight);
			spaceAdvance = std::max(spaceAdvance, sizeInfo.spaceAdvance);
		}

		Nz::Vector2f size = { float(spaceAdvance), float(lineHeight) + 5.f };
		SetMinimumSize(size);
	}

	void TextAreaWidget::Write(const Nz::String& text, std::size_t glyphPosition)
	{
		if (glyphPosition >= m_drawer.GetGlyphCount())
		{
			// It's faster to append than to insert in the middle
			AppendText(text);
			SetCursorPosition(m_drawer.GetGlyphCount());
		}
		else
		{
			m_text.Insert(m_text.GetCharacterPosition(glyphPosition), text);
			SetText(m_text);

			SetCursorPosition(glyphPosition + text.GetLength());
		}
	}

	Nz::AbstractTextDrawer& TextAreaWidget::GetTextDrawer()
	{
		return m_drawer;
	}

	const Nz::AbstractTextDrawer& TextAreaWidget::GetTextDrawer() const
	{
		return m_drawer;
	}

	void TextAreaWidget::HandleIndentation(bool add)
	{
		if (add)
			Write(Nz::String('\t'));
		else
		{
			std::size_t currentGlyph = GetGlyphIndex(m_cursorPositionBegin);

			if (currentGlyph > 0 && m_text[m_text.GetCharacterPosition(currentGlyph - 1U)] == '\t') // Check if previous glyph is a tab
			{
				Erase(currentGlyph - 1U);

				if (m_cursorPositionBegin.x < static_cast<unsigned int>(m_drawer.GetLineGlyphCount(m_cursorPositionBegin.y)))
					MoveCursor(-1);
			}
		}
	}

	void TextAreaWidget::HandleSelectionIndentation(bool add)
	{
		for (unsigned line = m_cursorPositionBegin.y; line <= m_cursorPositionEnd.y; ++line)
		{
			const Nz::Vector2ui cursorPositionBegin = m_cursorPositionBegin;
			const Nz::Vector2ui cursorPositionEnd = m_cursorPositionEnd;

			if (add)
			{
				Write(Nz::String('\t'), { 0U, line });
				SetSelection(cursorPositionBegin + (cursorPositionBegin.y == line && cursorPositionBegin.x != 0U ? Nz::Vector2ui{ 1U, 0U } : Nz::Vector2ui{}),
				             cursorPositionEnd + (cursorPositionEnd.y == line ? Nz::Vector2ui{ 1U, 0U } : Nz::Vector2ui{}));
			}
			else
			{
				if (m_drawer.GetLineGlyphCount(line) == 0)
					continue;

				std::size_t firstGlyph = GetGlyphIndex({ 0U, line });

				if (m_text[m_text.GetCharacterPosition(firstGlyph)] == '\t')
				{
					Erase(firstGlyph);
					SetSelection(cursorPositionBegin - (cursorPositionBegin.y == line && cursorPositionBegin.x != 0U ? Nz::Vector2ui{ 1U, 0U } : Nz::Vector2ui{}),
					             cursorPositionEnd - (cursorPositionEnd.y == line && cursorPositionEnd.x != 0U ? Nz::Vector2ui{ 1U, 0U } : Nz::Vector2ui{}));
				}
			}
		}
	}

	void TextAreaWidget::HandleWordCursorMove(bool left)
	{
		if (left)
		{
			std::size_t index = GetGlyphIndex(m_cursorPositionBegin);
			if (index == 0)
				return;

			std::size_t spaceIndex = m_text.FindLast(' ', index - 2);
			std::size_t endlIndex = m_text.FindLast('\n', index - 1);

			if ((spaceIndex > endlIndex || endlIndex == Nz::String::npos) && spaceIndex != Nz::String::npos)
				SetCursorPosition(spaceIndex + 1);
			else if (endlIndex != Nz::String::npos)
			{
				if (index == endlIndex + 1)
					SetCursorPosition(endlIndex);
				else
					SetCursorPosition(endlIndex + 1);
			}
			else
				SetCursorPosition({ 0U, m_cursorPositionBegin.y });
		}
		else
		{
			std::size_t index = GetGlyphIndex(m_cursorPositionEnd);
			std::size_t spaceIndex = m_text.Find(' ', index);
			std::size_t endlIndex = m_text.Find('\n', index);

			if (spaceIndex < endlIndex && spaceIndex != Nz::String::npos)
			{
				if (m_text.GetSize() > spaceIndex)
					SetCursorPosition(spaceIndex + 1);
				else
					SetCursorPosition({ static_cast<unsigned int>(m_drawer.GetLineGlyphCount(m_cursorPositionEnd.y)), m_cursorPositionEnd.y });
			}
			else if (endlIndex != Nz::String::npos)
			{
				if (index == endlIndex)
					SetCursorPosition(endlIndex + 1);
				else
					SetCursorPosition(endlIndex);
			}
			else
				SetCursorPosition({ static_cast<unsigned int>(m_drawer.GetLineGlyphCount(m_cursorPositionEnd.y)), m_cursorPositionEnd.y });
		}
	}

	void TextAreaWidget::UpdateDisplayText()
	{
		switch (m_echoMode)
		{
			case EchoMode_Normal:
				m_drawer.SetText(m_text);
				break;

			case EchoMode_Password:
			case EchoMode_PasswordExceptLast:
				m_drawer.SetText(Nz::String(m_text.GetLength(), '*'));
				break;
		}

		UpdateTextSprite();

		SetCursorPosition(m_cursorPositionBegin); //< Refresh cursor position (prevent it from being outside of the text)
	}
}
