// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
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
	inline UInt32 RichTextDrawer::BlockRef::GetStyle() const
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
	inline void RichTextDrawer::BlockRef::SetStyle(UInt32 style)
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
