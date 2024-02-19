// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Text renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	inline SimpleTextDrawer::SimpleTextDrawer() :
	m_color(Color::White()),
	m_outlineColor(Color::Black()),
	m_style(TextStyle_Regular),
	m_colorUpdated(true),
	m_glyphUpdated(true),
	m_characterSpacingOffset(0.f),
	m_lineSpacingOffset(0.f),
	m_maxLineWidth(std::numeric_limits<float>::infinity()),
	m_outlineThickness(0.f),
	m_characterSize(24)
	{
		SetTextFont(Font::GetDefault());
	}

	inline SimpleTextDrawer::SimpleTextDrawer(const SimpleTextDrawer& drawer) :
	m_text(drawer.m_text),
	m_color(drawer.m_color),
	m_outlineColor(drawer.m_outlineColor),
	m_style(drawer.m_style),
	m_colorUpdated(false),
	m_glyphUpdated(false),
	m_characterSpacingOffset(drawer.m_characterSpacingOffset),
	m_lineSpacingOffset(drawer.m_lineSpacingOffset),
	m_maxLineWidth(drawer.m_maxLineWidth),
	m_outlineThickness(drawer.m_outlineThickness),
	m_characterSize(drawer.m_characterSize)
	{
		SetTextFont(drawer.m_font);
	}

	inline SimpleTextDrawer::SimpleTextDrawer(SimpleTextDrawer&& drawer) noexcept
	{
		operator=(std::move(drawer));
	}

	inline void SimpleTextDrawer::AppendText(std::string_view str)
	{
		m_text.append(str);
		if (m_glyphUpdated)
			GenerateGlyphs(str);
	}

	inline float SimpleTextDrawer::GetCharacterSpacingOffset() const
	{
		return m_characterSpacingOffset;
	}

	inline unsigned int SimpleTextDrawer::GetCharacterSize() const
	{
		return m_characterSize;
	}

	inline float SimpleTextDrawer::GetLineHeight() const
	{
		NazaraAssert(m_font, "SimpleTextDrawer has no font");
		return GetLineHeight(m_font->GetSizeInfo(m_characterSize));
	}

	inline float SimpleTextDrawer::GetLineSpacingOffset() const
	{
		return m_lineSpacingOffset;
	}

	inline const std::string& SimpleTextDrawer::GetText() const
	{
		return m_text;
	}

	inline const Color& SimpleTextDrawer::GetTextColor() const
	{
		return m_color;
	}

	inline const std::shared_ptr<Font>& SimpleTextDrawer::GetTextFont() const
	{
		return m_font;
	}

	inline const Color& SimpleTextDrawer::GetTextOutlineColor() const
	{
		return m_outlineColor;
	}

	inline float SimpleTextDrawer::GetTextOutlineThickness() const
	{
		return m_outlineThickness;
	}

	inline TextStyleFlags SimpleTextDrawer::GetTextStyle() const
	{
		return m_style;
	}

	inline void SimpleTextDrawer::SetCharacterSpacingOffset(float offset)
	{
		if (m_characterSpacingOffset != offset)
		{
			m_characterSpacingOffset = offset;

			InvalidateGlyphs();
		}
	}

	inline void SimpleTextDrawer::SetCharacterSize(unsigned int characterSize)
	{
		if (m_characterSize != characterSize)
		{
			m_characterSize = characterSize;

			InvalidateGlyphs();
		}
	}

	inline void SimpleTextDrawer::SetLineSpacingOffset(float offset)
	{
		if (m_lineSpacingOffset != offset)
		{
			m_lineSpacingOffset = offset;

			InvalidateGlyphs();
		}
	}

	inline void SimpleTextDrawer::SetMaxLineWidth(float lineWidth)
	{
		if (m_maxLineWidth != lineWidth)
		{
			NazaraAssert(lineWidth > 0.f, "Max line width must be positive");

			m_maxLineWidth = lineWidth;

			InvalidateGlyphs();
		}
	}

	inline void SimpleTextDrawer::SetText(std::string str)
	{
		if (m_text != str)
		{
			m_text = std::move(str);

			InvalidateGlyphs();
		}
	}

	inline void SimpleTextDrawer::SetTextColor(const Color& color)
	{
		if (m_color != color)
		{
			m_color = color;

			InvalidateColor();
		}
	}

	inline void SimpleTextDrawer::SetTextFont(std::shared_ptr<Font> font)
	{
		if (m_font != font)
		{
			m_font = std::move(font);

			if (m_font)
				ConnectFontSlots();
			else
				DisconnectFontSlots();

			InvalidateGlyphs();
		}
	}

	inline void SimpleTextDrawer::SetTextOutlineColor(const Color& color)
	{
		if (m_outlineColor != color)
		{
			m_outlineColor = color;

			InvalidateColor();
		}
	}

	inline void SimpleTextDrawer::SetTextOutlineThickness(float thickness)
	{
		if (m_outlineThickness != thickness)
		{
			NazaraAssert(thickness >= 0.f, "Thickness must be zero or positive");

			m_outlineThickness = thickness;

			InvalidateGlyphs();
		}
	}

	inline void SimpleTextDrawer::SetTextStyle(TextStyleFlags style)
	{
		if (m_style != style)
		{
			m_style = style;

			InvalidateGlyphs();
		}
	}

	inline SimpleTextDrawer& SimpleTextDrawer::operator=(const SimpleTextDrawer& drawer)
	{
		m_characterSize = drawer.m_characterSize;
		m_characterSpacingOffset = drawer.m_characterSpacingOffset;
		m_color = drawer.m_color;
		m_lineSpacingOffset = drawer.m_lineSpacingOffset;
		m_maxLineWidth = drawer.m_maxLineWidth;
		m_outlineColor = drawer.m_outlineColor;
		m_outlineThickness = drawer.m_outlineThickness;
		m_style = drawer.m_style;
		m_text = drawer.m_text;

		SetTextFont(drawer.m_font);
		InvalidateGlyphs();

		return *this;
	}

	inline SimpleTextDrawer& SimpleTextDrawer::operator=(SimpleTextDrawer&& drawer) noexcept
	{
		DisconnectFontSlots();

		m_bounds = std::move(drawer.m_bounds);
		m_colorUpdated = std::move(drawer.m_colorUpdated);
		m_characterSize = std::move(drawer.m_characterSize);
		m_characterSpacingOffset = drawer.m_characterSpacingOffset;
		m_color = std::move(drawer.m_color);
		m_glyphs = std::move(drawer.m_glyphs);
		m_glyphUpdated = std::move(drawer.m_glyphUpdated);
		m_font = std::move(drawer.m_font);
		m_lineSpacingOffset = drawer.m_lineSpacingOffset;
		m_maxLineWidth = drawer.m_maxLineWidth;
		m_outlineColor = std::move(drawer.m_outlineColor);
		m_outlineThickness = std::move(drawer.m_outlineThickness);
		m_style = std::move(drawer.m_style);
		m_text = std::move(drawer.m_text);

		// Update slot pointers (TODO: Improve the way of doing this)
		if (m_font)
		{
			drawer.DisconnectFontSlots();
			ConnectFontSlots();
		}

		return *this;
	}

	inline SimpleTextDrawer SimpleTextDrawer::Draw(std::string str, unsigned int characterSize, TextStyleFlags style, const Color& color)
	{
		SimpleTextDrawer drawer;
		drawer.SetCharacterSize(characterSize);
		drawer.SetTextColor(color);
		drawer.SetTextStyle(style);
		drawer.SetText(std::move(str));

		return drawer;
	}

	inline SimpleTextDrawer SimpleTextDrawer::Draw(std::string str, unsigned int characterSize, TextStyleFlags style, const Color& color, float outlineThickness, const Color& outlineColor)
	{
		SimpleTextDrawer drawer;
		drawer.SetCharacterSize(characterSize);
		drawer.SetTextColor(color);
		drawer.SetTextOutlineColor(outlineColor);
		drawer.SetTextOutlineThickness(outlineThickness);
		drawer.SetTextStyle(style);
		drawer.SetText(std::move(str));

		return drawer;
	}

	inline SimpleTextDrawer SimpleTextDrawer::Draw(const std::shared_ptr<Font>& font, std::string str, unsigned int characterSize, TextStyleFlags style, const Color& color)
	{
		SimpleTextDrawer drawer;
		drawer.SetCharacterSize(characterSize);
		drawer.SetTextColor(color);
		drawer.SetTextFont(font);
		drawer.SetTextStyle(style);
		drawer.SetText(std::move(str));

		return drawer;
	}

	inline SimpleTextDrawer SimpleTextDrawer::Draw(const std::shared_ptr<Font>& font, std::string str, unsigned int characterSize, TextStyleFlags style, const Color& color, float outlineThickness, const Color& outlineColor)
	{
		SimpleTextDrawer drawer;
		drawer.SetCharacterSize(characterSize);
		drawer.SetTextColor(color);
		drawer.SetTextFont(font);
		drawer.SetTextOutlineColor(outlineColor);
		drawer.SetTextOutlineThickness(outlineThickness);
		drawer.SetTextStyle(style);
		drawer.SetText(std::move(str));

		return drawer;
	}

	inline void SimpleTextDrawer::AppendNewLine() const
	{
		AppendNewLine(InvalidGlyph, 0);
	}

	inline void SimpleTextDrawer::ConnectFontSlots()
	{
		m_atlasChangedSlot.Connect(m_font->OnFontAtlasChanged, this, &SimpleTextDrawer::OnFontInvalidated);
		m_atlasLayerChangedSlot.Connect(m_font->OnFontAtlasLayerChanged, this, &SimpleTextDrawer::OnFontAtlasLayerChanged);
		m_fontReleaseSlot.Connect(m_font->OnFontRelease, this, &SimpleTextDrawer::OnFontRelease);
		m_glyphCacheClearedSlot.Connect(m_font->OnFontGlyphCacheCleared, this, &SimpleTextDrawer::OnFontInvalidated);
	}

	inline void SimpleTextDrawer::DisconnectFontSlots()
	{
		m_atlasChangedSlot.Disconnect();
		m_atlasLayerChangedSlot.Disconnect();
		m_fontReleaseSlot.Disconnect();
		m_glyphCacheClearedSlot.Disconnect();
	}

	inline float SimpleTextDrawer::GetLineHeight(const Font::SizeInfo& sizeInfo) const
	{
		return float(sizeInfo.lineHeight) + m_lineSpacingOffset;
	}

	inline void SimpleTextDrawer::InvalidateColor()
	{
		m_colorUpdated = false;
	}

	inline void SimpleTextDrawer::InvalidateGlyphs()
	{
		m_glyphUpdated = false;
	}

	inline bool SimpleTextDrawer::ShouldLineWrap(float size) const
	{
		if (m_lines.back().glyphIndex > m_glyphs.size())
			return false;

		return m_lines.back().bounds.GetMaximum().x + size > m_maxLineWidth;
	}

	inline void SimpleTextDrawer::UpdateGlyphColor() const
	{
		if (m_outlineThickness > 0.f)
		{
			for (std::size_t glyphIndex = 0; glyphIndex < m_glyphs.size(); ++glyphIndex)
			{
				Glyph& glyph = m_glyphs[glyphIndex];
				if (glyphIndex % 2 == 0)
					glyph.color = m_outlineColor;
				else
					glyph.color = m_color;
			}
		}
		else
		{
			for (Glyph& glyph : m_glyphs)
				glyph.color = m_color;
		}

		m_colorUpdated = true;
	}

	inline void SimpleTextDrawer::UpdateGlyphs() const
	{
		NazaraAssert(m_font && m_font->IsValid(), "Invalid font");

		ClearGlyphs();
		GenerateGlyphs(m_text);
	}
}

