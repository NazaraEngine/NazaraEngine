// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	inline std::size_t RichTextDrawer::FindBlock(std::size_t glyphIndex) const
	{
		auto it = m_blocks.begin();
		for (; it != m_blocks.end(); ++it)
		{
			if (it->glyphIndex > glyphIndex)
				break;
		}

		assert(it != m_blocks.begin());
		return std::distance(m_blocks.begin(), it) - 1;
		/*
		// Binary search
		std::size_t count = m_blocks.size();
		std::size_t step;

		std::size_t i = InvalidBlockIndex;
		std::size_t first = 0;
		std::size_t last = count;
		while (count > 0)
		{
			i = first;
			step = count / 2;

			i += step;

			if (m_blocks[i].glyphIndex < glyphIndex)
			{
				first = ++i;
				count -= step + 1;
			}
			else
				count = step;
		}

		return i;*/
	}

	inline auto RichTextDrawer::GetBlock(std::size_t index) -> BlockRef
	{
		NazaraAssert(index < m_blocks.size(), "Invalid block index");
		return BlockRef(*this, index);
	}

	inline unsigned int RichTextDrawer::GetBlockCharacterSize(std::size_t index) const
	{
		NazaraAssert(index < m_blocks.size(), "Invalid block index");
		return m_blocks[index].characterSize;
	}

	inline float RichTextDrawer::GetBlockCharacterSpacingOffset(std::size_t index) const
	{
		NazaraAssert(index < m_blocks.size(), "Invalid block index");
		return m_blocks[index].characterSpacingOffset;
	}

	inline const Color& RichTextDrawer::GetBlockColor(std::size_t index) const
	{
		NazaraAssert(index < m_blocks.size(), "Invalid block index");
		return m_blocks[index].color;
	}

	inline std::size_t RichTextDrawer::GetBlockCount() const
	{
		return m_blocks.size();
	}

	inline std::size_t RichTextDrawer::GetBlockFirstGlyphIndex(std::size_t index) const
	{
		NazaraAssert(index < m_blocks.size(), "Invalid block index");
		return m_blocks[index].glyphIndex;
	}

	inline const std::shared_ptr<Font>& RichTextDrawer::GetBlockFont(std::size_t index) const
	{
		NazaraAssert(index < m_blocks.size(), "Invalid block index");
		std::size_t fontIndex = m_blocks[index].fontIndex;
		assert(fontIndex < m_fonts.size());
		return m_fonts[fontIndex].font;
	}

	inline float RichTextDrawer::GetBlockLineSpacingOffset(std::size_t index) const
	{
		NazaraAssert(index < m_blocks.size(), "Invalid block index");
		return m_blocks[index].lineSpacingOffset;
	}

	inline const Color& RichTextDrawer::GetBlockOutlineColor(std::size_t index) const
	{
		NazaraAssert(index < m_blocks.size(), "Invalid block index");
		return m_blocks[index].outlineColor;
	}

	inline float RichTextDrawer::GetBlockOutlineThickness(std::size_t index) const
	{
		NazaraAssert(index < m_blocks.size(), "Invalid block index");
		return m_blocks[index].outlineThickness;
	}

	inline TextStyleFlags RichTextDrawer::GetBlockStyle(std::size_t index) const
	{
		NazaraAssert(index < m_blocks.size(), "Invalid block index");
		return m_blocks[index].style;
	}

	inline const std::string& RichTextDrawer::GetBlockText(std::size_t index) const
	{
		NazaraAssert(index < m_blocks.size(), "Invalid block index");
		return m_blocks[index].text;
	}

	inline unsigned int RichTextDrawer::GetCharacterSize() const
	{
		return m_currentCharacterSize;
	}

	inline float RichTextDrawer::GetCharacterSpacingOffset() const
	{
		return m_currentCharacterSpacingOffset;
	}

	inline float RichTextDrawer::GetLineSpacingOffset() const
	{
		return m_currentLineSpacingOffset;
	}

	inline void RichTextDrawer::AppendNewLine(const Font& font, unsigned int characterSize, float lineSpacingOffset) const
	{
		AppendNewLine(font, characterSize, lineSpacingOffset, InvalidGlyph, 0);
	}

	inline void RichTextDrawer::ClearGlyphs() const
	{
		m_bounds = Rectf::Zero(); //< Compute bounds as float to speedup bounds computation (as casting between floats and integers is costly)
		m_lastSeparatorGlyph = InvalidGlyph;
		m_lines.clear();
		m_glyphs.clear();
		m_glyphUpdated = true;
	}

	inline void RichTextDrawer::ConnectFontSlots()
	{
		for (auto& fontData : m_fonts)
		{
			fontData.atlasChangedSlot.Connect(fontData.font->OnFontAtlasChanged, this, &RichTextDrawer::OnFontInvalidated);
			fontData.atlasLayerChangedSlot.Connect(fontData.font->OnFontAtlasLayerChanged, this, &RichTextDrawer::OnFontAtlasLayerChanged);
			fontData.fontReleaseSlot.Connect(fontData.font->OnFontDestroy, this, &RichTextDrawer::OnFontRelease);
			fontData.glyphCacheClearedSlot.Connect(fontData.font->OnFontGlyphCacheCleared, this, &RichTextDrawer::OnFontInvalidated);
		}
	}

	inline void RichTextDrawer::DisconnectFontSlots()
	{
		for (auto& fontData : m_fonts)
		{
			fontData.atlasChangedSlot.Disconnect();
			fontData.atlasLayerChangedSlot.Disconnect();
			fontData.fontReleaseSlot.Disconnect();
			fontData.glyphCacheClearedSlot.Disconnect();
		}
	}

	inline float RichTextDrawer::GetLineHeight(const Block& block) const
	{
		assert(block.fontIndex < m_fonts.size());
		const FontData& fontData = m_fonts[block.fontIndex];

		return GetLineHeight(block.lineSpacingOffset, fontData.font->GetSizeInfo(block.characterSize));
	}

	inline float RichTextDrawer::GetLineHeight(float lineSpacingOffset, const Font::SizeInfo& sizeInfo) const
	{
		return float(sizeInfo.lineHeight) + lineSpacingOffset;
	}

	inline std::size_t RichTextDrawer::HandleFontAddition(const std::shared_ptr<Font>& font)
	{
		auto it = m_fontIndexes.find(font);
		if (it == m_fontIndexes.end())
		{
			std::size_t fontIndex = m_fonts.size();
			m_fonts.emplace_back();
			auto& fontData = m_fonts.back();
			fontData.font = font;
			fontData.atlasChangedSlot.Connect(font->OnFontAtlasChanged, this, &RichTextDrawer::OnFontInvalidated);
			fontData.atlasLayerChangedSlot.Connect(font->OnFontAtlasLayerChanged, this, &RichTextDrawer::OnFontAtlasLayerChanged);
			fontData.fontReleaseSlot.Connect(font->OnFontDestroy, this, &RichTextDrawer::OnFontRelease);
			fontData.glyphCacheClearedSlot.Connect(font->OnFontGlyphCacheCleared, this, &RichTextDrawer::OnFontInvalidated);

			it = m_fontIndexes.emplace(font, fontIndex).first;
		}

		return it->second;
	}

	inline void RichTextDrawer::ReleaseFont(std::size_t fontIndex)
	{
		assert(fontIndex < m_fonts.size());

		FontData& fontData = m_fonts[fontIndex];
		assert(fontData.useCount > 0);

		if (--fontData.useCount == 0)
		{
			// Shift font indexes
			m_fontIndexes.erase(fontData.font);
			for (auto& fontIndexe : m_fontIndexes)
			{
				if (fontIndexe.second > fontIndex)
					fontIndexe.second--;
			}

			m_fonts.erase(m_fonts.begin() + fontIndex);
		}
	}

	inline bool RichTextDrawer::ShouldLineWrap(float size) const
	{
		if (m_lines.back().glyphIndex > m_glyphs.size())
			return false;

		return m_lines.back().bounds.GetMaximum().x + size > m_maxLineWidth;
	}

	inline const Color& RichTextDrawer::GetTextColor() const
	{
		return m_currentColor;
	}

	inline const std::shared_ptr<Font>& RichTextDrawer::GetTextFont() const
	{
		return m_currentFont;
	}

	inline const Color& RichTextDrawer::GetTextOutlineColor() const
	{
		return m_currentOutlineColor;
	}

	inline float RichTextDrawer::GetTextOutlineThickness() const
	{
		return m_currentOutlineThickness;
	}

	inline TextStyleFlags RichTextDrawer::GetTextStyle() const
	{
		return m_currentStyle;
	}

	inline bool RichTextDrawer::HasBlocks() const
	{
		return !m_blocks.empty();
	}

	inline void RichTextDrawer::SetBlockCharacterSize(std::size_t index, unsigned int characterSize)
	{
		NazaraAssert(index < m_blocks.size(), "Invalid block index");
		m_blocks[index].characterSize = characterSize;

		InvalidateGlyphs();
	}

	inline void RichTextDrawer::SetBlockCharacterSpacingOffset(std::size_t index, float offset)
	{
		NazaraAssert(index < m_blocks.size(), "Invalid block index");
		m_blocks[index].characterSpacingOffset = offset;

		InvalidateGlyphs();
	}

	inline void RichTextDrawer::SetBlockColor(std::size_t index, const Color& color)
	{
		NazaraAssert(index < m_blocks.size(), "Invalid block index");
		m_blocks[index].color = color;

		InvalidateGlyphs();
	}

	inline void RichTextDrawer::SetBlockFont(std::size_t index, std::shared_ptr<Font> font)
	{
		NazaraAssert(index < m_blocks.size(), "Invalid block index");
		std::size_t fontIndex = HandleFontAddition(font);
		std::size_t oldFontIndex = m_blocks[index].fontIndex;

		if (oldFontIndex != fontIndex)
		{
			ReleaseFont(oldFontIndex);

			m_fonts[fontIndex].useCount++;
			m_blocks[index].fontIndex = fontIndex;
		}

		InvalidateGlyphs();
	}

	inline void RichTextDrawer::SetBlockLineSpacingOffset(std::size_t index, float offset)
	{
		NazaraAssert(index < m_blocks.size(), "Invalid block index");
		m_blocks[index].lineSpacingOffset = offset;

		InvalidateGlyphs();
	}

	inline void RichTextDrawer::SetBlockOutlineColor(std::size_t index, const Color& color)
	{
		NazaraAssert(index < m_blocks.size(), "Invalid block index");
		m_blocks[index].outlineColor = color;

		InvalidateGlyphs();
	}

	inline void RichTextDrawer::SetBlockOutlineThickness(std::size_t index, float thickness)
	{
		NazaraAssert(index < m_blocks.size(), "Invalid block index");
		m_blocks[index].outlineThickness = thickness;

		InvalidateGlyphs();
	}

	inline void RichTextDrawer::SetBlockStyle(std::size_t index, TextStyleFlags style)
	{
		NazaraAssert(index < m_blocks.size(), "Invalid block index");
		m_blocks[index].style = style;

		InvalidateGlyphs();
	}

	inline void RichTextDrawer::SetBlockText(std::size_t index, std::string str)
	{
		NazaraAssert(index < m_blocks.size(), "Invalid block index");

		std::size_t previousLength = m_blocks[index].text.size(); //< FIXME: Count Unicode glyphs

		m_blocks[index].text = std::move(str);

		std::size_t newLength = m_blocks[index].text.size(); //< FIXME: Count Unicode glyphs
		if (newLength != previousLength)
		{
			std::size_t delta = newLength - previousLength; //< Underflow allowed
			for (std::size_t i = index + 1; i < m_blocks.size(); ++i)
				m_blocks[i].glyphIndex += delta;
		}

		InvalidateGlyphs();
	}

	inline void RichTextDrawer::SetCharacterSize(unsigned int characterSize)
	{
		m_currentCharacterSize = characterSize;
	}

	inline void RichTextDrawer::SetCharacterSpacingOffset(float offset)
	{
		m_currentCharacterSpacingOffset = offset;
	}

	inline void RichTextDrawer::SetLineSpacingOffset(float offset)
	{
		m_currentLineSpacingOffset = offset;
	}

	inline void RichTextDrawer::SetTextColor(const Color& color)
	{
		m_currentColor = color;
	}

	inline void RichTextDrawer::SetTextFont(const std::shared_ptr<Font>& font)
	{
		m_currentFont = font;
	}

	inline void RichTextDrawer::SetTextOutlineColor(const Color& color)
	{
		m_currentOutlineColor = color;
	}

	inline void RichTextDrawer::SetTextOutlineThickness(float thickness)
	{
		m_currentOutlineThickness = thickness;
	}

	inline void RichTextDrawer::SetTextStyle(TextStyleFlags style)
	{
		m_currentStyle = style;
	}

	inline void RichTextDrawer::InvalidateGlyphs()
	{
		m_glyphUpdated = false;
	}

	/*!
	* \class Nz::RichTextDrawer::BlockRef
	* \brief Helper class representing a block inside a RichTextDrawer, allowing easier access.
	* 
	* \warning This class is meant for temporary use, moving or destroying the RichTextDrawer or one of its blocks invalidates all BlockRef
	*/

	inline RichTextDrawer::BlockRef::BlockRef(RichTextDrawer& drawer, std::size_t index) :
	m_blockIndex(index),
	m_drawer(drawer)
	{
	}

	/*!
	* Returns the character spacing offset used for the characters of the referenced block
	* \return The referenced block character size
	*
	* \see GetColor, GetFont, GetStyle, GetText, SetCharacterSize
	*/
	inline float RichTextDrawer::BlockRef::GetCharacterSpacingOffset() const
	{
		return m_drawer.GetBlockCharacterSpacingOffset(m_blockIndex);
	}

	/*!
	* Returns the character size used for the characters of the referenced block
	* \return The referenced block character size
	*
	* \see GetColor, GetFont, GetStyle, GetText, SetCharacterSize
	*/
	inline unsigned int RichTextDrawer::BlockRef::GetCharacterSize() const
	{
		return m_drawer.GetBlockCharacterSize(m_blockIndex);
	}

	/*!
	* Returns the color used for the characters of the referenced block
	* \return The referenced block color
	*
	* \see GetCharacterSize, GetFont, GetStyle, GetText, SetColor
	*/
	inline Color RichTextDrawer::BlockRef::GetColor() const
	{
		return m_drawer.GetBlockColor(m_blockIndex);
	}

	/*!
	* Returns the font used for the characters of the referenced block
	* \return A reference on the referenced block font
	*
	* \see GetCharacterSize, GetTextColor, GetStyle, GetText, SetFont
	*/
	inline const std::shared_ptr<Font>& RichTextDrawer::BlockRef::GetFont() const
	{
		return m_drawer.GetBlockFont(m_blockIndex);
	}

	/*!
	* Returns the line spacing offset used for the characters of the referenced block
	* \return The referenced block character size
	*
	* \see GetTextColor, GetTextFont, GetStyle, GetText, SetCharacterSize
	*/
	inline float RichTextDrawer::BlockRef::GetLineSpacingOffset() const
	{
		return m_drawer.GetBlockLineSpacingOffset(m_blockIndex);
	}

	/*!
	* Returns the outline color used for the characters of the referenced block
	* \return The referenced block outline color
	*
	* \see GetCharacterSize, GetTextColor, GetStyle, GetText, SetFont
	*/
	inline Color RichTextDrawer::BlockRef::GetOutlineColor() const
	{
		return m_drawer.GetBlockOutlineColor(m_blockIndex);
	}

	/*!
	* Returns the outline thickness used for the characters of the referenced block
	* \return The referenced block outline thickness
	*
	* \see GetCharacterSize, GetTextColor, GetStyle, GetText, SetFont
	*/
	inline float RichTextDrawer::BlockRef::GetOutlineThickness() const
	{
		return m_drawer.GetBlockOutlineThickness(m_blockIndex);
	}

	/*!
	* Returns the style flags used for the characters of the referenced block
	* \return The referenced block style flags (see TextStyleFlags)
	*
	* \see GetCharacterSize, GetTextColor, GetTextFont, GetText, SetStyle
	*/
	inline TextStyleFlags RichTextDrawer::BlockRef::GetStyle() const
	{
		return m_drawer.GetBlockStyle(m_blockIndex);
	}

	/*!
	* Returns the first glyph index at which starts the referenced block
	* \return The first glyph index concerned by this block
	*
	* \see GetText
	*/
	inline std::size_t RichTextDrawer::BlockRef::GetFirstGlyphIndex() const
	{
		return m_drawer.GetBlockFirstGlyphIndex(m_blockIndex);
	}

	/*!
	* Returns the text of the referenced block
	* \return The referenced block text
	*
	* \see GetCharacterSize, GetTextColor, GetTextFont, GetStyle, SetText
	*/
	inline const std::string& RichTextDrawer::BlockRef::GetText() const
	{
		return m_drawer.GetBlockText(m_blockIndex);
	}

	/*!
	* Changes the character spacing offset of the referenced block characters
	* \remark This invalidates the drawer and will force a (complete or partial, depending on the block index) glyph regeneration to occur.
	*
	* \see GetCharacterSpacingOffset, SetColor, SetFont, SetStyle, SetText
	*/
	inline void RichTextDrawer::BlockRef::SetCharacterSpacingOffset(float offset)
	{
		m_drawer.SetBlockCharacterSpacingOffset(m_blockIndex, offset);
	}

	/*!
	* Changes the character size of the referenced block characters
	* \remark This invalidates the drawer and will force a (complete or partial, depending on the block index) glyph regeneration to occur.
	*
	* \see GetCharacterSize, SetColor, SetFont, SetStyle, SetText
	*/
	inline void RichTextDrawer::BlockRef::SetCharacterSize(unsigned int size)
	{
		m_drawer.SetBlockCharacterSize(m_blockIndex, size);
	}

	/*!
	* Changes the color of the referenced block characters 
	* \remark This is the only property that can be changed without forcing a glyph regeneration
	*
	* \see GetTextColor, SetCharacterSize, SetFont, SetStyle, SetText
	*/
	inline void RichTextDrawer::BlockRef::SetColor(Color color)
	{
		m_drawer.SetBlockColor(m_blockIndex, color);
	}

	/*!
	* Changes the font of the referenced block characters
	* \remark This invalidates the drawer and will force a (complete or partial, depending on the block index) glyph regeneration to occur.
	*
	* \see GetCharacterSize, SetCharacterSize, SetTextColor, SetStyle, SetText
	*/
	inline void RichTextDrawer::BlockRef::SetFont(std::shared_ptr<Font> font)
	{
		m_drawer.SetBlockFont(m_blockIndex, std::move(font));
	}

	/*!
	* Changes the line spacing offset of the referenced block characters
	* \remark This invalidates the drawer and will force a (complete or partial, depending on the block index) glyph regeneration to occur.
	*
	* \see GetLineSpacingOffset, SetTextColor, SetTextFont, SetStyle, SetText
	*/
	inline void RichTextDrawer::BlockRef::SetLineSpacingOffset(float offset)
	{
		m_drawer.SetBlockLineSpacingOffset(m_blockIndex, offset);
	}

	/*!
	* Changes the outline color of the referenced block characters
	* \remark This invalidates the drawer and will force a (complete or partial, depending on the block index) glyph regeneration to occur.
	*
	* \see GetCharacterSize, SetCharacterSize, SetTextColor, SetStyle, SetText
	*/
	inline void RichTextDrawer::BlockRef::SetOutlineColor(Color color)
	{
		m_drawer.SetBlockOutlineColor(m_blockIndex, std::move(color));
	}

	/*!
	* Changes the outline thickness of the referenced block characters
	* \remark This invalidates the drawer and will force a (complete or partial, depending on the block index) glyph regeneration to occur.
	*
	* \see GetCharacterSize, SetCharacterSize, SetTextColor, SetStyle, SetText
	*/
	inline void RichTextDrawer::BlockRef::SetOutlineThickness(float thickness)
	{
		m_drawer.SetBlockOutlineThickness(m_blockIndex, thickness);
	}

	/*!
	* Changes the style flags of the referenced block characters
	* \remark This invalidates the drawer and will force a (complete or partial, depending on the block index) glyph regeneration to occur.
	*
	* \see GetStyle, SetCharacterSize, SetTextColor, SetTextFont, SetText
	*/
	inline void RichTextDrawer::BlockRef::SetStyle(TextStyleFlags style)
	{
		m_drawer.SetBlockStyle(m_blockIndex, style);
	}

	/*!
	* Changes the text of the referenced block
	* \remark This invalidates the drawer and will force a (complete or partial, depending on the block index) glyph regeneration to occur.
	*
	* \see GetText, SetCharacterSize, SetTextColor, SetTextFont, SetTextStyle
	*/
	inline void RichTextDrawer::BlockRef::SetText(std::string text)
	{
		m_drawer.SetBlockText(m_blockIndex, std::move(text));
	}
}

#include <Nazara/Utility/DebugOff.hpp>
