// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/RichTextDrawer.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	inline void RichTextDrawer::Clear()
	{
		m_fontIndexes.clear();
		m_blocks.clear();
		m_fonts.clear();
		m_glyphs.clear();
		ClearGlyphs();
	}

	inline unsigned int RichTextDrawer::GetBlockCharacterSize(std::size_t index) const
	{
		NazaraAssert(index < m_blocks.size(), "Invalid block index");
		return m_blocks[index].characterSize;
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

	inline const FontRef& RichTextDrawer::GetBlockFont(std::size_t index) const
	{
		NazaraAssert(index < m_blocks.size(), "Invalid block index");
		std::size_t fontIndex = m_blocks[index].fontIndex;
		assert(fontIndex < m_fonts.size());
		return m_fonts[fontIndex].font;
	}

	inline TextStyleFlags RichTextDrawer::GetBlockStyle(std::size_t index) const
	{
		NazaraAssert(index < m_blocks.size(), "Invalid block index");
		return m_blocks[index].style;
	}

	inline const String& RichTextDrawer::GetBlockText(std::size_t index) const
	{
		NazaraAssert(index < m_blocks.size(), "Invalid block index");
		return m_blocks[index].text;
	}

	inline unsigned int RichTextDrawer::GetDefaultCharacterSize() const
	{
		return m_defaultCharacterSize;
	}

	inline const Color& RichTextDrawer::GetDefaultColor() const
	{
		return m_defaultColor;
	}

	inline const FontRef& RichTextDrawer::GetDefaultFont() const
	{
		return m_defaultFont;
	}

	inline TextStyleFlags RichTextDrawer::GetDefaultStyle() const
	{
		return m_defaultStyle;
	}

	inline void RichTextDrawer::AppendNewLine(const Font* font, unsigned int characterSize) const
	{
		// Ensure we're appending from last line
		Line& lastLine = m_lines.back();

		const Font::SizeInfo& sizeInfo = font->GetSizeInfo(characterSize);

		unsigned int previousDrawPos = m_drawPos.x;

		// Reset cursor
		m_drawPos.x = 0;
		m_drawPos.y += sizeInfo.lineHeight;

		m_workingBounds.ExtendTo(lastLine.bounds);
		m_lines.emplace_back(Line{ Rectf(0.f, float(sizeInfo.lineHeight * m_lines.size()), 0.f, float(sizeInfo.lineHeight)), m_glyphs.size() + 1 });
	}

	inline void RichTextDrawer::ClearGlyphs() const
	{
		m_bounds.MakeZero();
		m_lines.clear();
		m_glyphs.clear();
		m_glyphUpdated = true;
		m_workingBounds.MakeZero(); //< Compute bounds as float to speedup bounds computation (as casting between floats and integers is costly)
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

	inline std::size_t RichTextDrawer::HandleFontAddition(const FontRef& font)
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
			for (auto it = m_fontIndexes.begin(); it != m_fontIndexes.end(); ++it)
			{
				if (it->second > fontIndex)
					it->second--;
			}

			m_fonts.erase(m_fonts.begin() + fontIndex);
		}
	}

	inline void RichTextDrawer::SetBlockCharacterSize(std::size_t index, unsigned int characterSize)
	{
		NazaraAssert(index < m_blocks.size(), "Invalid block index");
		m_blocks[index].characterSize = characterSize;

		InvalidateGlyphs();
	}

	inline void RichTextDrawer::SetBlockColor(std::size_t index, const Color& color)
	{
		NazaraAssert(index < m_blocks.size(), "Invalid block index");
		m_blocks[index].color = color;

		InvalidateGlyphs();
	}

	inline void RichTextDrawer::SetBlockFont(std::size_t index, FontRef font)
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
	}

	inline void RichTextDrawer::SetBlockStyle(std::size_t index, TextStyleFlags style)
	{
		NazaraAssert(index < m_blocks.size(), "Invalid block index");
		m_blocks[index].style = style;

		InvalidateGlyphs();
	}

	inline void RichTextDrawer::SetBlockText(std::size_t index, const String& str)
	{
		NazaraAssert(index < m_blocks.size(), "Invalid block index");
		m_blocks[index].text = str;

		InvalidateGlyphs();
	}

	inline void RichTextDrawer::SetDefaultCharacterSize(unsigned int characterSize)
	{
		m_defaultCharacterSize = characterSize;
	}

	inline void RichTextDrawer::SetDefaultColor(const Color& color)
	{
		m_defaultColor = color;
	}

	inline void RichTextDrawer::SetDefaultFont(const FontRef& font)
	{
		m_defaultFont = font;
	}

	inline void RichTextDrawer::SetDefaultStyle(TextStyleFlags style)
	{
		m_defaultStyle = style;
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
	* \see GetCharacterSize, GetColor, GetStyle, GetText, SetFont
	*/
	inline const FontRef& RichTextDrawer::BlockRef::GetFont() const
	{
		return m_drawer.GetBlockFont(m_blockIndex);
	}

	/*!
	* Returns the style flags used for the characters of the referenced block
	* \return The referenced block style flags (see TextStyleFlags)
	*
	* \see GetCharacterSize, GetColor, GetFont, GetText, SetStyle
	*/
	inline TextStyleFlags RichTextDrawer::BlockRef::GetStyle() const
	{
		return m_drawer.GetBlockStyle(m_blockIndex);
	}

	/*!
	* Returns the text of the referenced block
	* \return The referenced block text
	*
	* \see GetCharacterSize, GetColor, GetFont, GetStyle, SetText
	*/
	inline const String& RichTextDrawer::BlockRef::GetText() const
	{
		return m_drawer.GetBlockText(m_blockIndex);
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
	* \see GetColor, SetCharacterSize, SetFont, SetStyle, SetText
	*/
	inline void RichTextDrawer::BlockRef::SetColor(Color color)
	{
		m_drawer.SetBlockColor(m_blockIndex, color);
	}

	/*!
	* Changes the font of the referenced block characters
	* \remark This invalidates the drawer and will force a (complete or partial, depending on the block index) glyph regeneration to occur.
	*
	* \see GetCharacterSize, SetCharacterSize, SetColor, SetStyle, SetText
	*/
	inline void RichTextDrawer::BlockRef::SetFont(FontRef font)
	{
		m_drawer.SetBlockFont(m_blockIndex, std::move(font));
	}

	/*!
	* Changes the style flags of the referenced block characters
	* \remark This invalidates the drawer and will force a (complete or partial, depending on the block index) glyph regeneration to occur.
	*
	* \see GetStyle, SetCharacterSize, SetColor, SetFont, SetText
	*/
	inline void RichTextDrawer::BlockRef::SetStyle(TextStyleFlags style)
	{
		m_drawer.SetBlockStyle(m_blockIndex, style);
	}

	/*!
	* Changes the text of the referenced block
	* \remark This invalidates the drawer and will force a (complete or partial, depending on the block index) glyph regeneration to occur.
	*
	* \see GetText, SetCharacterSize, SetColor, SetFont, SetStyle
	*/
	inline void RichTextDrawer::BlockRef::SetText(const String& text)
	{
		m_drawer.SetBlockText(m_blockIndex, text);
	}
}

#include <Nazara/Utility/DebugOff.hpp>
