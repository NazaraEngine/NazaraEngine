// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Widgets/RichTextAreaWidget.hpp>

namespace Nz
{
	void RichTextAreaWidget::AppendText(std::string_view text)
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

				newText.append(blockText.substr(0, characterPosition));
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

	void RichTextAreaWidget::SetMaximumTextLength(std::size_t maximumLength)
	{
		AbstractTextAreaWidget::SetMaximumTextLength(maximumLength);

		if (m_maximumTextLength > 0 && m_drawer.HasBlocks())
		{
			std::size_t blockIndex = m_drawer.FindBlock(m_maximumTextLength);

			auto blockRef = m_drawer.GetBlock(blockIndex);
			const std::string& blockText = blockRef.GetText();

			assert(m_maximumTextLength >= blockRef.GetFirstGlyphIndex());
			std::size_t maxBlockSize = m_maximumTextLength - blockRef.GetFirstGlyphIndex();
			std::size_t textLength = ComputeCharacterCount(blockText);
			if (textLength > maxBlockSize)
			{
				blockRef.SetText(std::string(TrimRightCount(blockText, textLength - maxBlockSize, Nz::UnicodeAware{})));

				// And then remove all blocks after the limit (in reverse order because of index shifting)
				std::size_t lastBlockIndex = m_drawer.GetBlockCount();
				assert(lastBlockIndex > 0);
				for (std::size_t i = lastBlockIndex - 1; i > blockIndex; --i)
					m_drawer.RemoveBlock(i);

				UpdateDisplayText();
			}
		}
	}

	void RichTextAreaWidget::Write(std::string_view text, std::size_t glyphPosition)
	{
		if (m_drawer.HasBlocks())
		{
			if (m_maximumTextLength > 0)
			{
				auto lastBlockRef = m_drawer.GetBlock(m_drawer.GetBlockCount() - 1);
				std::size_t currentLength = lastBlockRef.GetFirstGlyphIndex() + ComputeCharacterCount(lastBlockRef.GetText());
				if (m_maximumTextLength <= currentLength)
					return;

				text = Substring(text, 0, m_maximumTextLength - currentLength, UnicodeAware{});
			}

			auto blockRef = m_drawer.GetBlock(m_drawer.FindBlock((glyphPosition > 0) ? glyphPosition - 1 : glyphPosition));
			std::size_t firstGlyph = blockRef.GetFirstGlyphIndex();
			assert(glyphPosition >= firstGlyph);

			std::string blockText = blockRef.GetText();
			std::size_t characterPosition = GetCharacterPosition(blockText, glyphPosition - firstGlyph);
			blockText.insert(characterPosition, text);

			blockRef.SetText(blockText);
		}
		else
		{
			if (m_maximumTextLength > 0)
				text = Substring(text, 0, m_maximumTextLength, UnicodeAware{});

			m_drawer.AppendText(text);
		}

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
