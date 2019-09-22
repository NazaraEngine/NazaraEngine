// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NDK/Widgets/RichTextAreaWidget.hpp>

namespace Ndk
{
	RichTextAreaWidget::RichTextAreaWidget(BaseWidget* parent) :
	AbstractTextAreaWidget(parent)
	{
		Layout();
	}
	
	void RichTextAreaWidget::AppendText(const Nz::String& text)
	{
		//m_text += text;
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
				/*m_drawer.Clear();
				std::size_t textLength = m_text.GetLength();
				if (textLength >= 2)
				{
					std::size_t lastCharacterPosition = m_text.GetCharacterPosition(textLength - 2);
					if (lastCharacterPosition != Nz::String::npos)
						m_drawer.AppendText(Nz::String(textLength - 1, '*'));
				}

				if (textLength >= 1)
					m_drawer.AppendText(m_text.SubString(m_text.GetCharacterPosition(textLength - 1)));*/

				break;
			}
		}

		UpdateTextSprite();

		//OnTextChanged(this, m_text);
	}

	void RichTextAreaWidget::Clear()
	{
		AbstractTextAreaWidget::Clear();
	}

	void RichTextAreaWidget::Erase(std::size_t firstGlyph, std::size_t lastGlyph)
	{
		if (firstGlyph > lastGlyph)
			std::swap(firstGlyph, lastGlyph);

		std::size_t textLength = m_drawer.GetGlyphCount();
		if (firstGlyph > textLength)
			return;

		std::size_t firstBlock = m_drawer.FindBlock(firstGlyph);
		std::size_t lastBlock = m_drawer.FindBlock(lastGlyph);
		if (firstBlock == lastBlock)
		{
			const Nz::String& blockText = m_drawer.GetBlockText(firstBlock);
			std::size_t blockFirstGlyph = m_drawer.GetBlockFirstGlyphIndex(firstBlock);

			Nz::String newText;
			if (firstGlyph > blockFirstGlyph)
			{
				std::size_t characterPosition = blockText.GetCharacterPosition(firstGlyph - blockFirstGlyph - 1);
				NazaraAssert(characterPosition != Nz::String::npos, "Invalid character position");

				newText.Append(blockText.SubString(0, characterPosition));
			}

			if (lastGlyph < textLength)
			{
				std::size_t characterPosition = blockText.GetCharacterPosition(lastGlyph - blockFirstGlyph);
				NazaraAssert(characterPosition != Nz::String::npos, "Invalid character position");

				newText.Append(blockText.SubString(characterPosition));
			}

			m_drawer.SetBlockText(firstBlock, newText);
		}
		else
		{
			// More complicated algorithm, yay
		}

		UpdateDisplayText();
	}

	void RichTextAreaWidget::Write(const Nz::String& text, std::size_t glyphPosition)
	{
		auto block = m_drawer.GetBlock(m_drawer.FindBlock(glyphPosition));
		std::size_t firstGlyph = block.GetFirstGlyphIndex();
		assert(glyphPosition >= firstGlyph);

		Nz::String blockText = block.GetText();
		std::size_t characterPosition = blockText.GetCharacterPosition(glyphPosition - firstGlyph);
		blockText.Insert(characterPosition, text);

		block.SetText(blockText);
		SetCursorPosition(glyphPosition + text.GetLength());

		UpdateDisplayText();
	}

	Nz::AbstractTextDrawer& RichTextAreaWidget::GetTextDrawer()
	{
		return m_drawer;
	}

	const Nz::AbstractTextDrawer& RichTextAreaWidget::GetTextDrawer() const
	{
		return m_drawer;
	}

	void RichTextAreaWidget::HandleIndentation(bool add)
	{
	}

	void RichTextAreaWidget::HandleSelectionIndentation(bool add)
	{
	}

	void RichTextAreaWidget::HandleWordCursorMove(bool left)
	{
	}

	void RichTextAreaWidget::UpdateDisplayText()
	{
		/*m_drawer.Clear();
		switch (m_echoMode)
		{
			case EchoMode_Normal:
				m_drawer.AppendText(m_text);
				break;

			case EchoMode_Password:
			case EchoMode_PasswordExceptLast:
				m_drawer.AppendText(Nz::String(m_text.GetLength(), '*'));
				break;
		}*/

		UpdateTextSprite();

		SetCursorPosition(m_cursorPositionBegin); //< Refresh cursor position (prevent it from being outside of the text)
	}
}
