// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Widgets/RichTextAreaWidget.hpp>
#include <Nazara/Widgets/Debug.hpp>

namespace Nz
{
	RichTextAreaWidget::RichTextAreaWidget(BaseWidget* parent) :
	AbstractTextAreaWidget(parent)
	{
		Layout();
	}
	
	void RichTextAreaWidget::AppendText(const std::string& text)
	{
		//m_text += text;
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
				// TODO
				/*m_drawer.Clear();
				std::size_t textLength = m_text.GetLength();
				if (textLength >= 2)
				{
					std::size_t lastCharacterPosition = m_text.GetCharacterPosition(textLength - 2);
					if (lastCharacterPosition != std::string::npos)
						m_drawer.AppendText(std::string(textLength - 1, '*'));
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
			const std::string& blockText = m_drawer.GetBlockText(firstBlock);
			std::size_t blockFirstGlyph = m_drawer.GetBlockFirstGlyphIndex(firstBlock);

			std::string newText;
			if (firstGlyph > blockFirstGlyph)
			{
				std::size_t characterPosition = GetCharacterPosition(blockText, firstGlyph - blockFirstGlyph);
				NazaraAssert(characterPosition != std::string::npos, "Invalid character position");

				newText.append(blockText.substr(0, characterPosition - 1));
			}

			if (lastGlyph < textLength)
				newText.append(blockText.substr(GetCharacterPosition(blockText, lastGlyph - blockFirstGlyph)));

			if (!newText.empty())
				m_drawer.SetBlockText(firstBlock, std::move(newText));
			else
				m_drawer.RemoveBlock(firstBlock);
		}
		else
		{
			const std::string& lastBlockText = m_drawer.GetBlockText(lastBlock);
			std::size_t lastBlockGlyphIndex = m_drawer.GetBlockFirstGlyphIndex(lastBlock);

			// First, update/delete last block
			std::size_t lastCharPos = GetCharacterPosition(lastBlockText, lastGlyph - lastBlockGlyphIndex);
			if (lastCharPos != std::string::npos)
			{
				std::string newText = lastBlockText.substr(lastCharPos);
				if (!newText.empty())
					m_drawer.SetBlockText(lastBlock, std::move(newText));
				else
					m_drawer.RemoveBlock(lastBlock);
			}

			// And then remove all middle blocks, remove in reverse order because of index shifting
			assert(lastBlock > 0);
			for (std::size_t i = lastBlock - 1; i > firstBlock; --i)
				m_drawer.RemoveBlock(i);

			const std::string& firstBlockText = m_drawer.GetBlockText(firstBlock);
			std::size_t firstBlockGlyphIndex = m_drawer.GetBlockFirstGlyphIndex(firstBlock);

			// And finally update/delete first block
			if (firstGlyph > firstBlockGlyphIndex)
			{
				std::size_t firstCharPos = GetCharacterPosition(firstBlockText, firstGlyph - firstBlockGlyphIndex - 1);
				if (firstCharPos != std::string::npos)
				{
					std::string newText = firstBlockText.substr(0, firstCharPos);
					if (!newText.empty())
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

	void RichTextAreaWidget::Write(const std::string& text, std::size_t glyphPosition)
	{
		if (m_drawer.HasBlocks())
		{
			auto block = m_drawer.GetBlock(m_drawer.FindBlock((glyphPosition > 0) ? glyphPosition - 1 : glyphPosition));
			std::size_t firstGlyph = block.GetFirstGlyphIndex();
			assert(glyphPosition >= firstGlyph);

			std::string blockText = block.GetText();
			std::size_t characterPosition = GetCharacterPosition(blockText, glyphPosition - firstGlyph);
			blockText.insert(characterPosition, text);

			block.SetText(blockText);
		}
		else
			m_drawer.AppendText(text);

		SetCursorPosition(glyphPosition + ComputeCharacterCount(text));

		UpdateDisplayText();
	}

	void RichTextAreaWidget::CopySelectionToClipboard(const Vector2ui& selectionBegin, const Vector2ui& selectionEnd)
	{
		// TODO
	}

	AbstractTextDrawer& RichTextAreaWidget::GetTextDrawer()
	{
		return m_drawer;
	}

	const AbstractTextDrawer& RichTextAreaWidget::GetTextDrawer() const
	{
		return m_drawer;
	}

	void RichTextAreaWidget::HandleIndentation(bool add)
	{
		// TODO
	}

	void RichTextAreaWidget::HandleSelectionIndentation(bool add)
	{
		// TODO
	}

	void RichTextAreaWidget::HandleWordCursorMove(bool left)
	{
		// TODO
	}

	void RichTextAreaWidget::HandleWordSelection(const Vector2ui& position)
	{
		// TODO
	}

	void RichTextAreaWidget::PasteFromClipboard(const Vector2ui& targetPosition)
	{
		// TODO
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
				m_drawer.AppendText(std::string(m_text.GetLength(), '*'));
				break;
		}*/

		UpdateTextSprite();

		SetCursorPosition(m_cursorPositionBegin); //< Refresh cursor position (prevent it from being outside of the text)
	}
}
