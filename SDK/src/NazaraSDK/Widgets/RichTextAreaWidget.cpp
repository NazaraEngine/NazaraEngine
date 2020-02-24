// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NazaraSDK/Widgets/RichTextAreaWidget.hpp>

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
		std::size_t lastBlock = m_drawer.FindBlock((lastGlyph > 0) ? lastGlyph - 1 : lastGlyph);
		if (firstBlock == lastBlock)
		{
			const Nz::String& blockText = m_drawer.GetBlockText(firstBlock);
			std::size_t blockFirstGlyph = m_drawer.GetBlockFirstGlyphIndex(firstBlock);

			Nz::String newText;
			if (firstGlyph > blockFirstGlyph)
			{
				std::size_t characterPosition = blockText.GetCharacterPosition(firstGlyph - blockFirstGlyph);
				NazaraAssert(characterPosition != Nz::String::npos, "Invalid character position");

				newText.Append(blockText.SubString(0, characterPosition - 1));
			}

			if (lastGlyph < textLength)
				newText.Append(blockText.SubString(blockText.GetCharacterPosition(lastGlyph - blockFirstGlyph)));

			if (!newText.IsEmpty())
				m_drawer.SetBlockText(firstBlock, std::move(newText));
			else
				m_drawer.RemoveBlock(firstBlock);
		}
		else
		{
			const Nz::String& lastBlockText = m_drawer.GetBlockText(lastBlock);
			std::size_t lastBlockGlyphIndex = m_drawer.GetBlockFirstGlyphIndex(lastBlock);

			// First, update/delete last block
			std::size_t lastCharPos = lastBlockText.GetCharacterPosition(lastGlyph - lastBlockGlyphIndex);
			if (lastCharPos != Nz::String::npos)
			{
				Nz::String newText = lastBlockText.SubString(lastCharPos);
				if (!newText.IsEmpty())
					m_drawer.SetBlockText(lastBlock, std::move(newText));
				else
					m_drawer.RemoveBlock(lastBlock);
			}

			// And then remove all middle blocks, remove in reverse order because of index shifting
			assert(lastBlock > 0);
			for (std::size_t i = lastBlock - 1; i > firstBlock; --i)
				m_drawer.RemoveBlock(i);

			const Nz::String& firstBlockText = m_drawer.GetBlockText(firstBlock);
			std::size_t firstBlockGlyphIndex = m_drawer.GetBlockFirstGlyphIndex(firstBlock);

			// And finally update/delete first block
			if (firstGlyph > firstBlockGlyphIndex)
			{
				std::size_t firstCharPos = firstBlockText.GetCharacterPosition(firstGlyph - firstBlockGlyphIndex - 1);
				if (firstCharPos != Nz::String::npos)
				{
					Nz::String newText = firstBlockText.SubString(0, firstCharPos);
					if (!newText.IsEmpty())
						m_drawer.SetBlockText(firstBlock, std::move(newText));
					else
						m_drawer.RemoveBlock(firstBlock);
				}
			}
			else
				m_drawer.RemoveBlock(firstBlock);
		}

		UpdateDisplayText();
	}

	void RichTextAreaWidget::Write(const Nz::String& text, std::size_t glyphPosition)
	{
		if (m_drawer.HasBlocks())
		{
			auto block = m_drawer.GetBlock(m_drawer.FindBlock((glyphPosition > 0) ? glyphPosition - 1 : glyphPosition));
			std::size_t firstGlyph = block.GetFirstGlyphIndex();
			assert(glyphPosition >= firstGlyph);

			Nz::String blockText = block.GetText();
			std::size_t characterPosition = blockText.GetCharacterPosition(glyphPosition - firstGlyph);
			blockText.Insert(characterPosition, text);

			block.SetText(blockText);
		}
		else
			m_drawer.AppendText(text);

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
