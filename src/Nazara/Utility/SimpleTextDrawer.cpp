// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/SimpleTextDrawer.hpp>
#include <limits>
#include <memory>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	void SimpleTextDrawer::Clear()
	{
		m_text.clear();
		ClearGlyphs();
	}

	const Rectf& SimpleTextDrawer::GetBounds() const
	{
		if (!m_glyphUpdated)
			UpdateGlyphs();

		return m_bounds;
	}

	const std::shared_ptr<Font>& SimpleTextDrawer::GetFont([[maybe_unused]] std::size_t index) const
	{
		NazaraAssert(index == 0, "font index out of range");
		return GetTextFont();
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

	void SimpleTextDrawer::AppendNewLine(std::size_t glyphIndex, float glyphPosition) const
	{
		// Ensure we're appending from last line
		Line& lastLine = m_lines.back();

		float previousDrawPos = m_drawPos.x;

		float lineHeight = GetLineHeight();

		// Reset cursor
		m_drawPos.x = 0.f;
		m_drawPos.y += lineHeight;
		m_lastSeparatorGlyph = InvalidGlyph;

		m_bounds.ExtendTo(lastLine.bounds);
		m_lines.emplace_back(Line{ Rectf(0.f, lineHeight * m_lines.size(), 0.f, lineHeight), m_glyphs.size() + 1 });

		if (glyphIndex != InvalidGlyph && glyphIndex > lastLine.glyphIndex)
		{
			Line& newLine = m_lines.back();
			newLine.glyphIndex = glyphIndex;

			for (std::size_t i = glyphIndex; i < m_glyphs.size(); ++i)
			{
				Glyph& glyph = m_glyphs[i];
				glyph.bounds.x -= glyphPosition;
				glyph.bounds.y += lineHeight;

				for (auto& corner : glyph.corners)
				{
					corner.x -= glyphPosition;
					corner.y += lineHeight;
				}

				newLine.bounds.ExtendTo(glyph.bounds);
			}

			assert(previousDrawPos >= glyphPosition);
			m_drawPos.x += previousDrawPos - glyphPosition;

			lastLine.bounds.width -= lastLine.bounds.GetMaximum().x - glyphPosition;

			// Regenerate bounds
			m_bounds = Rectf::Zero();
			for (auto& line : m_lines)
				m_bounds.ExtendTo(line.bounds);
		}
	}

	void SimpleTextDrawer::ClearGlyphs() const
	{
		m_bounds = Rectf::Zero();
		m_colorUpdated = true;
		m_drawPos = Vector2f(0.f, SafeCast<float>(m_characterSize)); //< Our draw "cursor"
		m_lastSeparatorGlyph = InvalidGlyph;
		m_lines.clear();
		m_glyphs.clear();
		m_glyphUpdated = true;
		m_previousCharacter = 0;

		if (m_font)
			m_lines.emplace_back(Line{Rectf(0.f, 0.f, 0.f, GetLineHeight()), 0});
		else
			m_lines.emplace_back(Line{Rectf::Zero(), 0});
	}

	bool SimpleTextDrawer::GenerateGlyph(Glyph& glyph, char32_t character, float outlineThickness, bool lineWrap, Color color, int renderOrder, int* advance) const
	{
		const Font::Glyph& fontGlyph = m_font->GetGlyph(m_characterSize, m_style, outlineThickness, character);
		if (fontGlyph.valid && fontGlyph.fauxOutlineThickness <= 0.f)
		{
			glyph.atlas = m_font->GetAtlas()->GetLayer(fontGlyph.layerIndex);
			glyph.atlasRect = fontGlyph.atlasRect;
			glyph.color = color;
			glyph.flipped = fontGlyph.flipped;
			glyph.renderOrder = renderOrder;

			glyph.bounds = Rectf(fontGlyph.aabb);

			if (lineWrap && ShouldLineWrap(glyph.bounds.width))
				AppendNewLine(m_lastSeparatorGlyph, m_lastSeparatorPosition);

			glyph.bounds.x += m_drawPos.x;
			glyph.bounds.y += m_drawPos.y;

			// Faux bold and faux outline thickness are not supported

			// We "lean" the glyph to simulate italics style
			float italic = (fontGlyph.requireFauxItalic) ? 0.208f : 0.f;
			float italicTop = italic * glyph.bounds.y;
			float italicBottom = italic * glyph.bounds.GetMaximum().y;

			glyph.corners[0] = Vector2f(glyph.bounds.x - italicTop - outlineThickness, glyph.bounds.y - outlineThickness);
			glyph.corners[1] = Vector2f(glyph.bounds.x + glyph.bounds.width - italicTop - outlineThickness, glyph.bounds.y - outlineThickness);
			glyph.corners[2] = Vector2f(glyph.bounds.x - italicBottom - outlineThickness, glyph.bounds.y + glyph.bounds.height - outlineThickness);
			glyph.corners[3] = Vector2f(glyph.bounds.x + glyph.bounds.width - italicBottom - outlineThickness, glyph.bounds.y + glyph.bounds.height - outlineThickness);

			if (advance)
				*advance = fontGlyph.advance;

			return true;
		}
		else
			return false;
	};

	void SimpleTextDrawer::GenerateGlyphs(std::string_view text) const
	{
		if (text.empty())
			return;

		const Font::SizeInfo& sizeInfo = m_font->GetSizeInfo(m_characterSize);

		IterateOnCodepoints(text, [&](std::u32string_view characters)
		{
			for (char32_t character : characters)
			{
				if (m_previousCharacter != 0)
					m_drawPos.x += m_font->GetKerning(m_characterSize, m_previousCharacter, character);

				m_previousCharacter = character;

				bool whitespace = true;
				float advance = m_characterSpacingOffset;
				switch (character)
				{
					case ' ':
					case '\n':
						advance += float(sizeInfo.spaceAdvance);
						break;

					case '\t':
						advance += float(sizeInfo.spaceAdvance) * 4.f;
						break;

					default:
						whitespace = false;
						break;
				}

				Glyph glyph;
				if (!whitespace)
				{
					int glyphRenderOrder = (m_outlineThickness > 0.f) ? 1 : 0;

					int iAdvance;
					if (!GenerateGlyph(glyph, character, 0.f, true, m_color, glyphRenderOrder, &iAdvance))
						continue; // Glyph failed to load, just skip it (can't do much)

					advance += float(iAdvance);

					if (m_outlineThickness > 0.f)
					{
						Glyph outlineGlyph;
						if (GenerateGlyph(outlineGlyph, character, m_outlineThickness, false, m_outlineColor, glyphRenderOrder - 1, nullptr))
							m_glyphs.push_back(outlineGlyph);
					}
				}
				else
				{
					if (ShouldLineWrap(advance))
						AppendNewLine(m_lastSeparatorGlyph, m_lastSeparatorPosition);

					glyph.atlas = nullptr;
					glyph.bounds = Rectf(m_drawPos.x, m_lines.back().bounds.y, advance, GetLineHeight(sizeInfo));

					glyph.corners[0] = glyph.bounds.GetCorner(RectCorner::LeftTop);
					glyph.corners[1] = glyph.bounds.GetCorner(RectCorner::RightTop);
					glyph.corners[2] = glyph.bounds.GetCorner(RectCorner::LeftBottom);
					glyph.corners[3] = glyph.bounds.GetCorner(RectCorner::RightBottom);
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

			return true; //< continue iteration
		});

		m_bounds.ExtendTo(m_lines.back().bounds);

		m_colorUpdated = true;
		m_glyphUpdated = true;
	}

	void SimpleTextDrawer::OnFontAtlasLayerChanged(const Font* font, AbstractImage* oldLayer, AbstractImage* newLayer)
	{
		NazaraUnused(font);

		#ifdef NAZARA_DEBUG
		if (m_font.get() != font)
		{
			NazaraInternalError("Not listening to " + PointerToString(font));
			return;
		}
		#endif

		// Update atlas layer pointer
		// Note: This can happen while updating glyphs
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
		if (m_font.get() != font)
		{
			NazaraInternalError("Not listening to " + PointerToString(font));
			return;
		}
		#endif

		InvalidateGlyphs();
	}

	void SimpleTextDrawer::OnFontRelease(const Font* font)
	{
		NazaraUnused(font);
		NazaraUnused(font);

		#ifdef NAZARA_DEBUG
		if (m_font.get() != font)
		{
			NazaraInternalError("Not listening to " + PointerToString(font));
			return;
		}
		#endif

		SetTextFont(nullptr);
	}
}
