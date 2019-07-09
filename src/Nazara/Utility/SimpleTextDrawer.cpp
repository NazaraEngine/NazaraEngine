// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/SimpleTextDrawer.hpp>
#include <limits>
#include <memory>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	SimpleTextDrawer::SimpleTextDrawer() :
	m_color(Color::White),
	m_outlineColor(Color::Black),
	m_style(TextStyle_Regular),
	m_colorUpdated(true),
	m_glyphUpdated(true),
	m_maxLineWidth(std::numeric_limits<float>::infinity()),
	m_outlineThickness(0.f),
	m_characterSize(24)
	{
		SetFont(Font::GetDefault());
	}

	SimpleTextDrawer::SimpleTextDrawer(const SimpleTextDrawer& drawer) :
	m_color(drawer.m_color),
	m_text(drawer.m_text),
	m_style(drawer.m_style),
	m_colorUpdated(false),
	m_glyphUpdated(false),
	m_outlineColor(drawer.m_outlineColor),
	m_maxLineWidth(drawer.m_maxLineWidth),
	m_outlineThickness(drawer.m_outlineThickness),
	m_characterSize(drawer.m_characterSize)
	{
		SetFont(drawer.m_font);
	}

	SimpleTextDrawer::SimpleTextDrawer(SimpleTextDrawer&& drawer)
	{
		operator=(std::move(drawer));
	}

	SimpleTextDrawer::~SimpleTextDrawer() = default;

	void SimpleTextDrawer::AppendText(const String& str)
	{
		m_text.Append(str);
		if (m_glyphUpdated)
			GenerateGlyphs(str);
	}

	void SimpleTextDrawer::Clear()
	{
		m_text.Clear(true);
		ClearGlyphs();
	}

	const Recti& SimpleTextDrawer::GetBounds() const
	{
		if (!m_glyphUpdated)
			UpdateGlyphs();

		return m_bounds;
	}

	unsigned int SimpleTextDrawer::GetCharacterSize() const
	{
		return m_characterSize;
	}

	const Color& SimpleTextDrawer::GetColor() const
	{
		return m_color;
	}

	Font* SimpleTextDrawer::GetFont() const
	{
		return m_font;
	}

	Font* SimpleTextDrawer::GetFont(std::size_t index) const
	{
		NazaraAssert(index == 0, "Font index out of range");
		NazaraUnused(index);

		return m_font;
	}

	std::size_t SimpleTextDrawer::GetFontCount() const
	{
		return 1;
	}

	const AbstractTextDrawer::Glyph& SimpleTextDrawer::GetGlyph(std::size_t index) const
	{
		if (!m_glyphUpdated)
			UpdateGlyphs();
		else if (!m_colorUpdated)
			UpdateGlyphColor();

		return m_glyphs[index];
	}

	std::size_t SimpleTextDrawer::GetGlyphCount() const
	{
		if (!m_glyphUpdated)
			UpdateGlyphs();

		return m_glyphs.size();
	}

	const AbstractTextDrawer::Line& SimpleTextDrawer::GetLine(std::size_t index) const
	{
		if (!m_glyphUpdated)
			UpdateGlyphs();

		NazaraAssert(index < m_lines.size(), "Line index out of range");
		return m_lines[index];
	}

	std::size_t SimpleTextDrawer::GetLineCount() const
	{
		if (!m_glyphUpdated)
			UpdateGlyphs();

		return m_lines.size();
	}

	float SimpleTextDrawer::GetMaxLineWidth() const
	{
		return m_maxLineWidth;
	}

	const Color& SimpleTextDrawer::GetOutlineColor() const
	{
		return m_outlineColor;
	}

	float SimpleTextDrawer::GetOutlineThickness() const
	{
		return m_outlineThickness;
	}

	TextStyleFlags SimpleTextDrawer::GetStyle() const
	{
		return m_style;
	}

	const String& SimpleTextDrawer::GetText() const
	{
		return m_text;
	}

	void SimpleTextDrawer::SetCharacterSize(unsigned int characterSize)
	{
		m_characterSize = characterSize;

		m_glyphUpdated = false;
	}

	void SimpleTextDrawer::SetColor(const Color& color)
	{
		m_color = color;

		m_colorUpdated = false;
	}

	void SimpleTextDrawer::SetFont(Font* font)
	{
		if (m_font != font)
		{
			m_font = font;

			if (m_font)
				ConnectFontSlots();
			else
				DisconnectFontSlots();

			m_glyphUpdated = false;
		}
	}

	void SimpleTextDrawer::SetMaxLineWidth(float lineWidth)
	{
		NazaraAssert(m_maxLineWidth > 0.f, "Max line width must be positive");

		m_maxLineWidth = lineWidth;

		m_glyphUpdated = false;
	}

	void SimpleTextDrawer::SetOutlineColor(const Color& color)
	{
		m_outlineColor = color;

		m_colorUpdated = false;
	}

	void SimpleTextDrawer::SetOutlineThickness(float thickness)
	{
		NazaraAssert(thickness >= 0.f, "Thickness must be zero or positive");

		m_outlineThickness = thickness;

		m_glyphUpdated = false;
	}

	void SimpleTextDrawer::SetStyle(TextStyleFlags style)
	{
		m_style = style;

		m_glyphUpdated = false;
	}

	void SimpleTextDrawer::SetText(const String& str)
	{
		m_text = str;

		m_glyphUpdated = false;
	}

	SimpleTextDrawer& SimpleTextDrawer::operator=(const SimpleTextDrawer& drawer)
	{
		m_characterSize = drawer.m_characterSize;
		m_color = drawer.m_color;
		m_outlineColor = drawer.m_outlineColor;
		m_outlineThickness = drawer.m_outlineThickness;
		m_style = drawer.m_style;
		m_text = drawer.m_text;

		m_colorUpdated = false;
		m_glyphUpdated = false;
		SetFont(drawer.m_font);

		return *this;
	}

	SimpleTextDrawer& SimpleTextDrawer::operator=(SimpleTextDrawer&& drawer)
	{
		DisconnectFontSlots();

		m_bounds = std::move(drawer.m_bounds);
		m_colorUpdated = std::move(drawer.m_colorUpdated);
		m_characterSize = std::move(drawer.m_characterSize);
		m_color = std::move(drawer.m_color);
		m_glyphs = std::move(drawer.m_glyphs);
		m_glyphUpdated = std::move(drawer.m_glyphUpdated);
		m_font = std::move(drawer.m_font);
		m_maxLineWidth = drawer.m_maxLineWidth;
		m_outlineColor = std::move(drawer.m_outlineColor);
		m_outlineThickness = std::move(drawer.m_outlineThickness);
		m_style = std::move(drawer.m_style);
		m_text = std::move(drawer.m_text);

		// Update slot pointers (TODO: Improve the way of doing this)
		ConnectFontSlots();

		return *this;
	}

	SimpleTextDrawer SimpleTextDrawer::Draw(const String& str, unsigned int characterSize, TextStyleFlags style, const Color& color)
	{
		SimpleTextDrawer drawer;
		drawer.SetCharacterSize(characterSize);
		drawer.SetColor(color);
		drawer.SetStyle(style);
		drawer.SetText(str);

		return drawer;
	}

	SimpleTextDrawer SimpleTextDrawer::Draw(const String& str, unsigned int characterSize, TextStyleFlags style, const Color& color, float outlineThickness, const Color& outlineColor)
	{
		SimpleTextDrawer drawer;
		drawer.SetCharacterSize(characterSize);
		drawer.SetColor(color);
		drawer.SetOutlineColor(outlineColor);
		drawer.SetOutlineThickness(outlineThickness);
		drawer.SetStyle(style);
		drawer.SetText(str);

		return drawer;
	}

	SimpleTextDrawer SimpleTextDrawer::Draw(Font* font, const String& str, unsigned int characterSize, TextStyleFlags style, const Color& color)
	{
		SimpleTextDrawer drawer;
		drawer.SetCharacterSize(characterSize);
		drawer.SetColor(color);
		drawer.SetFont(font);
		drawer.SetStyle(style);
		drawer.SetText(str);

		return drawer;
	}

	SimpleTextDrawer SimpleTextDrawer::Draw(Font* font, const String& str, unsigned int characterSize, TextStyleFlags style, const Color& color, float outlineThickness, const Color& outlineColor)
	{
		SimpleTextDrawer drawer;
		drawer.SetCharacterSize(characterSize);
		drawer.SetColor(color);
		drawer.SetFont(font);
		drawer.SetOutlineColor(outlineColor);
		drawer.SetOutlineThickness(outlineThickness);
		drawer.SetStyle(style);
		drawer.SetText(str);

		return drawer;
	}

	void SimpleTextDrawer::AppendNewLine() const
	{
		AppendNewLine(InvalidGlyph, 0.f);
	}

	void SimpleTextDrawer::AppendNewLine(std::size_t glyphIndex, unsigned int glyphPosition) const
	{
		// Ensure we're appending from last line
		Line& lastLine = m_lines.back();

		const Font::SizeInfo& sizeInfo = m_font->GetSizeInfo(m_characterSize);

		unsigned int previousDrawPos = m_drawPos.x;

		// Reset cursor
		m_drawPos.x = 0;
		m_drawPos.y += sizeInfo.lineHeight;
		m_lastSeparatorGlyph = InvalidGlyph;

		m_workingBounds.ExtendTo(lastLine.bounds);
		m_lines.emplace_back(Line{ Rectf(0.f, float(sizeInfo.lineHeight * m_lines.size()), 0.f, float(sizeInfo.lineHeight)), m_glyphs.size() + 1 });

		if (glyphIndex != InvalidGlyph && glyphIndex > lastLine.glyphIndex)
		{
			Line& newLine = m_lines.back();
			newLine.glyphIndex = glyphIndex;

			for (std::size_t i = glyphIndex; i < m_glyphs.size(); ++i)
			{
				Glyph& glyph = m_glyphs[i];
				glyph.bounds.x -= glyphPosition;
				glyph.bounds.y += sizeInfo.lineHeight;

				for (auto& corner : glyph.corners)
				{
					corner.x -= glyphPosition;
					corner.y += sizeInfo.lineHeight;
				}

				newLine.bounds.ExtendTo(glyph.bounds);
			}

			assert(previousDrawPos >= glyphPosition);
			m_drawPos.x += previousDrawPos - glyphPosition;

			lastLine.bounds.width -= lastLine.bounds.GetMaximum().x - m_lastSeparatorPosition;

			// Regenerate working bounds
			m_workingBounds.MakeZero();
			for (std::size_t i = 0; i < m_lines.size(); ++i)
				m_workingBounds.ExtendTo(m_lines[i].bounds);
		}
	}

	void SimpleTextDrawer::ClearGlyphs() const
	{
		m_bounds.MakeZero();
		m_colorUpdated = true;
		m_drawPos.Set(0, m_characterSize); //< Our draw "cursor"
		m_lastSeparatorGlyph = InvalidGlyph;
		m_lines.clear();
		m_glyphs.clear();
		m_glyphUpdated = true;
		m_previousCharacter = 0;
		m_workingBounds.MakeZero(); //< Compute bounds as float to speedup bounds computation (as casting between floats and integers is costly)

		if (m_font)
			m_lines.emplace_back(Line{Rectf(0.f, 0.f, 0.f, float(m_font->GetSizeInfo(m_characterSize).lineHeight)), 0});
		else
			m_lines.emplace_back(Line{Rectf::Zero(), 0});
	}

	void SimpleTextDrawer::ConnectFontSlots()
	{
		m_atlasChangedSlot.Connect(m_font->OnFontAtlasChanged, this, &SimpleTextDrawer::OnFontInvalidated);
		m_atlasLayerChangedSlot.Connect(m_font->OnFontAtlasLayerChanged, this, &SimpleTextDrawer::OnFontAtlasLayerChanged);
		m_fontReleaseSlot.Connect(m_font->OnFontRelease, this, &SimpleTextDrawer::OnFontRelease);
		m_glyphCacheClearedSlot.Connect(m_font->OnFontGlyphCacheCleared, this, &SimpleTextDrawer::OnFontInvalidated);
	}

	void SimpleTextDrawer::DisconnectFontSlots()
	{
		m_atlasChangedSlot.Disconnect();
		m_atlasLayerChangedSlot.Disconnect();
		m_fontReleaseSlot.Disconnect();
		m_glyphCacheClearedSlot.Disconnect();
	}

	bool SimpleTextDrawer::GenerateGlyph(Glyph& glyph, char32_t character, float outlineThickness, bool lineWrap, Nz::Color color, int renderOrder, int* advance) const
	{
		const Font::Glyph& fontGlyph = m_font->GetGlyph(m_characterSize, m_style, outlineThickness, character);
		if (fontGlyph.valid && fontGlyph.fauxOutlineThickness <= 0.f)
		{
			glyph.atlas = m_font->GetAtlas()->GetLayer(fontGlyph.layerIndex);
			glyph.atlasRect = fontGlyph.atlasRect;
			glyph.color = color;
			glyph.flipped = fontGlyph.flipped;
			glyph.renderOrder = renderOrder;

			glyph.bounds.Set(fontGlyph.aabb);

			if (lineWrap && ShouldLineWrap(glyph, glyph.bounds.width))
				AppendNewLine(m_lastSeparatorGlyph, m_lastSeparatorPosition);

			glyph.bounds.x += m_drawPos.x;
			glyph.bounds.y += m_drawPos.y;

			// Faux bold and faux outline thickness are not supported

			// We "lean" the glyph to simulate italics style
			float italic = (fontGlyph.requireFauxItalic) ? 0.208f : 0.f;
			float italicTop = italic * glyph.bounds.y;
			float italicBottom = italic * glyph.bounds.GetMaximum().y;

			glyph.corners[0].Set(glyph.bounds.x - italicTop - outlineThickness, glyph.bounds.y - outlineThickness);
			glyph.corners[1].Set(glyph.bounds.x + glyph.bounds.width - italicTop - outlineThickness, glyph.bounds.y - outlineThickness);
			glyph.corners[2].Set(glyph.bounds.x - italicBottom - outlineThickness, glyph.bounds.y + glyph.bounds.height - outlineThickness);
			glyph.corners[3].Set(glyph.bounds.x + glyph.bounds.width - italicBottom - outlineThickness, glyph.bounds.y + glyph.bounds.height - outlineThickness);

			if (advance)
				*advance = fontGlyph.advance;

			return true;
		}
		else
			return false;
	};

	void SimpleTextDrawer::GenerateGlyphs(const String& text) const
	{
		if (text.IsEmpty())
			return;

		///TODO: Allow iteration on Unicode characters without allocating any buffer
		std::u32string characters = text.GetUtf32String();
		if (characters.empty())
		{
			NazaraError("Invalid character set");
			return;
		}

		const Font::SizeInfo& sizeInfo = m_font->GetSizeInfo(m_characterSize);

		m_glyphs.reserve(m_glyphs.size() + characters.size() * ((m_outlineThickness > 0.f) ? 2 : 1));
		for (char32_t character : characters)
		{
			if (m_previousCharacter != 0)
				m_drawPos.x += m_font->GetKerning(m_characterSize, m_previousCharacter, character);

			m_previousCharacter = character;

			bool whitespace = true;
			int advance = 0;
			switch (character)
			{
				case ' ':
				case '\n':
					advance = sizeInfo.spaceAdvance;
					break;

				case '\t':
					advance = sizeInfo.spaceAdvance * 4;
					break;

				default:
					whitespace = false;
					break;
			}

			Glyph glyph;
			if (!whitespace)
			{
				if (!GenerateGlyph(glyph, character, 0.f, true, m_color, 0, &advance))
					continue; // Glyph failed to load, just skip it (can't do much)

				if (m_outlineThickness > 0.f)
				{
					Glyph outlineGlyph;
					if (GenerateGlyph(outlineGlyph, character, m_outlineThickness, false, m_outlineColor, -1, nullptr))
					{
						m_glyphs.push_back(outlineGlyph);
					}
				}
			}
			else
			{
				float glyphAdvance = advance;

				if (ShouldLineWrap(glyph, glyphAdvance))
					AppendNewLine(m_lastSeparatorGlyph, m_lastSeparatorPosition);

				glyph.atlas = nullptr;
				glyph.bounds.Set(float(m_drawPos.x), m_lines.back().bounds.y, glyphAdvance, float(sizeInfo.lineHeight));

				glyph.corners[0].Set(glyph.bounds.GetCorner(RectCorner_LeftTop));
				glyph.corners[1].Set(glyph.bounds.GetCorner(RectCorner_RightTop));
				glyph.corners[2].Set(glyph.bounds.GetCorner(RectCorner_LeftBottom));
				glyph.corners[3].Set(glyph.bounds.GetCorner(RectCorner_RightBottom));
			}

			m_lines.back().bounds.ExtendTo(glyph.bounds);

			switch (character)
			{
				case '\n':
				{
					AppendNewLine();
					break;
				}

				default:
					m_drawPos.x += advance;
					break;
			}

			if (whitespace)
			{
				m_lastSeparatorGlyph = m_glyphs.size();
				m_lastSeparatorPosition = m_drawPos.x;
			}

			m_glyphs.push_back(glyph);
		}

		m_workingBounds.ExtendTo(m_lines.back().bounds);

		m_bounds.Set(Rectf(std::floor(m_workingBounds.x), std::floor(m_workingBounds.y), std::ceil(m_workingBounds.width), std::ceil(m_workingBounds.height)));

		m_colorUpdated = true;
		m_glyphUpdated = true;
	}

	void SimpleTextDrawer::OnFontAtlasLayerChanged(const Font* font, AbstractImage* oldLayer, AbstractImage* newLayer)
	{
		NazaraUnused(font);

		#ifdef NAZARA_DEBUG
		if (m_font != font)
		{
			NazaraInternalError("Not listening to " + String::Pointer(font));
			return;
		}
		#endif

		// Update atlas layer pointer
		// Note: This can happen while updating
		for (Glyph& glyph : m_glyphs)
		{
			if (glyph.atlas == oldLayer)
				glyph.atlas = newLayer;
		}
	}

	void SimpleTextDrawer::OnFontInvalidated(const Font* font)
	{
		NazaraUnused(font);

		#ifdef NAZARA_DEBUG
		if (m_font != font)
		{
			NazaraInternalError("Not listening to " + String::Pointer(font));
			return;
		}
		#endif

		m_glyphUpdated = false;
	}

	void SimpleTextDrawer::OnFontRelease(const Font* font)
	{
		NazaraUnused(font);
		NazaraUnused(font);

		#ifdef NAZARA_DEBUG
		if (m_font != font)
		{
			NazaraInternalError("Not listening to " + String::Pointer(font));
			return;
		}
		#endif

		SetFont(nullptr);
	}

	bool SimpleTextDrawer::ShouldLineWrap(Glyph& glyph, float size, bool checkFirstGlyph) const
	{
		if (checkFirstGlyph && m_lines.back().glyphIndex > m_glyphs.size())
			return false;

		return m_lines.back().bounds.GetMaximum().x + size > m_maxLineWidth;
	}

	void SimpleTextDrawer::UpdateGlyphColor() const
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

	void SimpleTextDrawer::UpdateGlyphs() const
	{
		NazaraAssert(m_font && m_font->IsValid(), "Invalid font");

		ClearGlyphs();
		GenerateGlyphs(m_text);
	}
}
