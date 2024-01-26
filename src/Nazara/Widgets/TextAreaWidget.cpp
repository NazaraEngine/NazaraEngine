// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Widgets/TextAreaWidget.hpp>
#include <Nazara/Core/Unicode.hpp>
#include <Nazara/Graphics/Components/GraphicsComponent.hpp>
#include <Nazara/Platform/Clipboard.hpp>
#include <Nazara/Utility/Font.hpp>
#include <Nazara/Utility/Components/NodeComponent.hpp>
#include <Nazara/Widgets/Debug.hpp>

namespace Nz
{
	namespace NAZARA_ANONYMOUS_NAMESPACE
	{
		bool IsSpace(char32_t character)
		{
			switch (character)
			{
				case '\f':
				case '\n':
				case '\r':
				case '\t':
				case '\v':
					return true;

				default:
					return Unicode::GetCategory(character) & Unicode::Category_Separator;
			}
		}
	}

	TextAreaWidget::TextAreaWidget(BaseWidget* parent) :
	AbstractTextAreaWidget(parent)
	{
		SetCharacterSize(GetCharacterSize()); //< Actualize minimum / preferred size

		Layout();
	}

	void TextAreaWidget::AppendText(std::string_view text)
	{
		m_text += text;

		switch (m_echoMode)
		{
			case EchoMode::Normal:
				m_drawer.AppendText(text);
				break;

			case EchoMode::Hidden:
				m_drawer.AppendText(std::string(ComputeCharacterCount(text), '*'));
				break;

			case EchoMode::HiddenExceptLast:
			{
				m_drawer.Clear();
				std::size_t textLength = ComputeCharacterCount(m_text);
				if (textLength >= 2)
				{
					std::size_t lastCharacterPosition = GetCharacterPosition(m_text, textLength - 2);
					if (lastCharacterPosition != std::string::npos)
						m_drawer.AppendText(std::string(textLength - 1, '*'));
				}

				if (textLength >= 1)
					m_drawer.AppendText(m_text.substr(GetCharacterPosition(m_text, textLength - 1))); //< FIXME: getting last character position could be massively optimized

				break;
			}
		}

		UpdateTextSprite();

		OnTextChanged(this, m_text);
	}

	void TextAreaWidget::Clear()
	{
		AbstractTextAreaWidget::Clear();

		m_text.clear();
		OnTextChanged(this, m_text);
	}

	void TextAreaWidget::Erase(std::size_t firstGlyph, std::size_t lastGlyph)
	{
		if (firstGlyph > lastGlyph)
			std::swap(firstGlyph, lastGlyph);

		std::size_t textLength = ComputeCharacterCount(m_text);
		if (firstGlyph > textLength)
			return;

		std::string newText;
		if (firstGlyph > 0)
		{
			std::size_t characterPosition = GetCharacterPosition(m_text, firstGlyph);
			NazaraAssert(characterPosition != std::string::npos, "Invalid character position");

			newText.append(m_text.substr(0, characterPosition));
		}

		if (lastGlyph < textLength)
		{
			std::size_t characterPosition = GetCharacterPosition(m_text, lastGlyph);
			NazaraAssert(characterPosition != std::string::npos, "Invalid character position");

			newText.append(m_text.substr(characterPosition));
		}

		SetText(newText);
	}

	void TextAreaWidget::Write(std::string_view text, std::size_t glyphPosition)
	{
		if (glyphPosition >= m_drawer.GetGlyphCount())
		{
			// It's faster to append than to insert in the middle
			AppendText(text);
			SetCursorPosition(m_drawer.GetGlyphCount());
		}
		else
		{
			m_text.insert(GetCharacterPosition(m_text, glyphPosition), text);
			SetText(m_text);

			SetCursorPosition(glyphPosition + ComputeCharacterCount(text));
		}
	}

	void TextAreaWidget::CopySelectionToClipboard(const Vector2ui& selectionBegin, const Vector2ui& selectionEnd)
	{
		std::size_t glyphCount = ComputeCharacterCount(m_text);
		assert(glyphCount > 0);

		std::size_t startIndex = GetCharacterPosition(m_text, GetGlyphIndex(selectionBegin));
		std::size_t endIndex = GetCharacterPosition(m_text, std::min(GetGlyphIndex(selectionEnd), glyphCount));

		Clipboard::SetString(m_text.substr(startIndex, endIndex - startIndex));
	}

	AbstractTextDrawer& TextAreaWidget::GetTextDrawer()
	{
		return m_drawer;
	}

	const AbstractTextDrawer& TextAreaWidget::GetTextDrawer() const
	{
		return m_drawer;
	}

	void TextAreaWidget::HandleIndentation(bool add)
	{
		if (add)
			Write("\t");
		else
		{
			std::size_t currentGlyph = GetGlyphIndex(m_cursorPositionBegin);

			if (currentGlyph > 0 && m_text[GetCharacterPosition(m_text, currentGlyph - 1U)] == '\t') // Check if previous glyph is a tab
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
			const Vector2ui cursorPositionBegin = m_cursorPositionBegin;
			const Vector2ui cursorPositionEnd = m_cursorPositionEnd;

			if (add)
			{
				Write("\t", {0U, line});
				SetSelection(cursorPositionBegin + (cursorPositionBegin.y == line && cursorPositionBegin.x != 0U ? Vector2ui{ 1U, 0U } : Vector2ui{}),
				             cursorPositionEnd + (cursorPositionEnd.y == line ? Vector2ui{ 1U, 0U } : Vector2ui{}));
			}
			else
			{
				if (m_drawer.GetLineGlyphCount(line) == 0)
					continue;

				std::size_t firstGlyph = GetGlyphIndex({ 0U, line });

				if (m_text[GetCharacterPosition(m_text, firstGlyph)] == '\t')
				{
					Erase(firstGlyph);
					SetSelection(cursorPositionBegin - (cursorPositionBegin.y == line && cursorPositionBegin.x != 0U ? Vector2ui{ 1U, 0U } : Vector2ui{}),
					             cursorPositionEnd - (cursorPositionEnd.y == line && cursorPositionEnd.x != 0U ? Vector2ui{ 1U, 0U } : Vector2ui{}));
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

			std::size_t spaceIndex = m_text.rfind(' ', index - 1);
			std::size_t endlIndex = m_text.rfind('\n', index - 1);

			if ((spaceIndex > endlIndex || endlIndex == std::string::npos) && spaceIndex != std::string::npos)
				SetCursorPosition(spaceIndex + 1);
			else if (endlIndex != std::string::npos)
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
			std::size_t spaceIndex = m_text.find(' ', index);
			std::size_t endlIndex = m_text.find('\n', index);

			if (spaceIndex < endlIndex && spaceIndex != std::string::npos)
			{
				if (ComputeCharacterCount(m_text) > spaceIndex)
					SetCursorPosition(spaceIndex + 1);
				else
					SetCursorPosition({ static_cast<unsigned int>(m_drawer.GetLineGlyphCount(m_cursorPositionEnd.y)), m_cursorPositionEnd.y });
			}
			else if (endlIndex != std::string::npos)
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

	void TextAreaWidget::HandleWordSelection(const Vector2ui& /*position*/)
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		std::size_t index = GetGlyphIndex(m_cursorPositionEnd);

		// FIXME: Handle Unicode properly
		std::size_t startIndex = index;
		for (std::string::reverse_iterator it { m_text.begin() + index }; it != m_text.rend(); ++it)
		{
			if (IsSpace(*it))
				break;

			--startIndex;
		}

		std::size_t endIndex = index;
		for (auto it = m_text.begin() + index; it != m_text.end(); ++it)
		{
			if (IsSpace(*it))
				break;

			++endIndex;
		}

		SetSelection(GetCursorPosition(GetCharacterPosition(m_text, startIndex)), GetCursorPosition(GetCharacterPosition(m_text, endIndex)));
	}

	void TextAreaWidget::PasteFromClipboard(const Vector2ui& targetPosition)
	{
		std::size_t glyphCount = ComputeCharacterCount(m_text);
		std::size_t targetIndex = GetCharacterPosition(m_text, std::min(GetGlyphIndex(targetPosition), glyphCount));

		std::string clipboardString = Clipboard::GetString();
		if (clipboardString.empty())
			return;

		m_text.insert(targetIndex, clipboardString);
		UpdateDisplayText();

		SetCursorPosition(targetIndex + ComputeCharacterCount(clipboardString));
	}

	void TextAreaWidget::UpdateDisplayText()
	{
		switch (m_echoMode)
		{
			case EchoMode::Normal:
				m_drawer.SetText(m_text);
				break;

			case EchoMode::Hidden:
			case EchoMode::HiddenExceptLast:
				m_drawer.SetText(std::string(ComputeCharacterCount(m_text), '*'));
				break;
		}

		UpdateTextSprite();

		SetCursorPosition(m_cursorPositionBegin); //< Refresh cursor position (prevent it from being outside of the text)
	}

	void TextAreaWidget::UpdateMinimumSize()
	{
		std::size_t fontCount = m_drawer.GetFontCount();
		float lineHeight = 0;
		int spaceAdvance = 0;
		for (std::size_t i = 0; i < fontCount; ++i)
		{
			const std::shared_ptr<Font>& font = m_drawer.GetFont(i);

			const Font::SizeInfo& sizeInfo = font->GetSizeInfo(m_drawer.GetCharacterSize());
			lineHeight = std::max(lineHeight, m_drawer.GetLineHeight());
			spaceAdvance = std::max(spaceAdvance, sizeInfo.spaceAdvance);
		}

		Vector2f size = { float(spaceAdvance), lineHeight + 5.f };
		SetMinimumSize(size);
	}
}
