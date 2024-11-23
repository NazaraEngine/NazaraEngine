// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Text renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/TextRenderer/SimpleTextDrawer.hpp>
#include <limits>
#include <memory>

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
		NazaraAssertMsg(index == 0, "font index out of range");
		return GetTextFont();
	}

	std::size_t SimpleTextDrawer::GetFontCount() const
	{
		return 1;
	}

	const AbstractTextDrawer::Glyph* SimpleTextDrawer::GetGlyphs() const
	{
		if (!m_glyphUpdated)
			UpdateGlyphs();

		return m_glyphs.data();
	}

	std::size_t SimpleTextDrawer::GetGlyphCount() const
	{
		if (!m_glyphUpdated)
			UpdateGlyphs();

		return m_glyphs.size();
	}

	const AbstractTextDrawer::Line* SimpleTextDrawer::GetLines() const
	{
		if (!m_glyphUpdated)
			UpdateGlyphs();

		return m_lines.data();
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

	const AbstractTextDrawer::Sprite* SimpleTextDrawer::GetSprites() const
	{
		if (!m_glyphUpdated)
			UpdateGlyphs();
		else if (!m_colorUpdated)
			UpdateSpriteColor();

		return m_sprites.data();
	}

	std::size_t SimpleTextDrawer::GetSpriteCount() const
	{
		if (!m_glyphUpdated)
			UpdateGlyphs();

		return m_sprites.size();
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
		m_lines.emplace_back(Line{ m_glyphs.size() + 1, Rectf(0.f, lastLine.bounds.y + lastLine.bounds.height, 0.f, lineHeight), true });

		// Move characters since last whitespace to the new line 
		if (glyphIndex != InvalidGlyph && glyphIndex > lastLine.glyphIndex)
		{
			Line& newLine = m_lines.back();
			newLine.glyphIndex = glyphIndex;

			for (std::size_t i = glyphIndex; i < m_glyphs.size(); ++i)
			{
				Glyph& glyph = m_glyphs[i];
				glyph.bounds.x -= glyphPosition;
				glyph.bounds.y += lineHeight;

				for (std::size_t spriteIndex : { glyph.spriteIndex, glyph.outlineSpriteIndex })
				{
					if (spriteIndex == InvalidIndex)
						continue;

					for (auto& corner : m_sprites[spriteIndex].corners)
					{
						corner.x -= glyphPosition;
						corner.y += lineHeight;
					}
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
		m_sprites.clear();
		m_glyphUpdated = true;
		m_previousCharacter = 0;

		if (m_font)
			m_lines.emplace_back(Line{ 0, Rectf(0.f, 0.f, 0.f, GetLineHeight()), true });
		else
			m_lines.emplace_back(Line{ 0, Rectf::Zero(), true });
	}

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
						// we give line feed advance so they can be selected
						advance += float(sizeInfo.spaceAdvance);
						break;

					case '\t':
						advance += float(sizeInfo.spaceAdvance) * 4.f;
						break;

					default:
						whitespace = false;
						break;
				}

				int glyphRenderOrder = (m_outlineThickness != 0.f && !m_style.Test(TextStyle::OutlineOnly)) ? 1 : 0;

				Glyph glyph;
				if (!whitespace)
				{
					Sprite sprite;
					int iAdvance;
					if (!GenerateSprite(glyph.bounds, sprite, character, (m_style.Test(TextStyle::OutlineOnly)) ? m_outlineThickness : 0.f, true, (m_style.Test(TextStyle::OutlineOnly)) ? m_outlineColor : m_color, glyphRenderOrder, &iAdvance))
						continue; // Glyph failed to load, just skip it (can't do much)

					glyph.spriteIndex = m_sprites.size();
					m_sprites.push_back(sprite);

					if (m_outlineThickness != 0.f && !m_style.Test(TextStyle::OutlineOnly))
					{
						Sprite outlineSprite;
						if (GenerateSprite(glyph.bounds, outlineSprite, character, m_outlineThickness, false, m_outlineColor, glyphRenderOrder - 1, nullptr))
						{
							glyph.outlineSpriteIndex = m_sprites.size();
							m_sprites.push_back(outlineSprite);
						}
					}

					advance += float(iAdvance);
				}
				else
				{
					if (ShouldLineWrap(advance) && character != '\n')
						AppendNewLine(m_lastSeparatorGlyph, m_lastSeparatorPosition);

					glyph.bounds = Rectf(m_drawPos.x, m_lines.back().bounds.y, advance, GetLineHeight(sizeInfo));
					glyph.outlineSpriteIndex = InvalidIndex;
					glyph.spriteIndex = InvalidIndex;
				}

				m_lines.back().bounds.ExtendTo(glyph.bounds);

				if (character == '\n')
				{
					m_lines.back().allowsOvershoot = false;
					AppendNewLine();
				}
				else
					m_drawPos.x += advance;

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

	bool SimpleTextDrawer::GenerateSprite(Rectf& bounds, Sprite& sprite, char32_t character, float outlineThickness, bool lineWrap, Color color, int renderOrder, int* advance) const
	{
		const Font::Glyph& fontGlyph = m_font->GetGlyph(m_characterSize, m_style, outlineThickness, character);
		if (fontGlyph.valid && fontGlyph.fauxOutlineThickness <= 0.f)
		{
			sprite.atlas = m_font->GetAtlas()->GetLayer(fontGlyph.layerIndex);
			sprite.atlasRect = fontGlyph.atlasRect;
			sprite.color = color;
			sprite.flipped = fontGlyph.flipped;
			sprite.renderOrder = renderOrder;

			bounds = Rectf(fontGlyph.aabb);

			if (lineWrap && ShouldLineWrap(bounds.width))
				AppendNewLine(m_lastSeparatorGlyph, m_lastSeparatorPosition);

			bounds.x += m_drawPos.x;
			bounds.y += m_drawPos.y;

			// Faux bold and faux outline thickness are not supported

			// We "lean" the glyph to simulate italics style
			float italic = (fontGlyph.requireFauxItalic) ? 0.208f : 0.f;
			float italicTop = italic * bounds.y;
			float italicBottom = italic * bounds.GetMaximum().y;

			sprite.corners[0] = Vector2f(bounds.x - italicTop - outlineThickness, bounds.y - outlineThickness);
			sprite.corners[1] = Vector2f(bounds.x + bounds.width - italicTop - outlineThickness, bounds.y - outlineThickness);
			sprite.corners[2] = Vector2f(bounds.x - italicBottom - outlineThickness, bounds.y + bounds.height - outlineThickness);
			sprite.corners[3] = Vector2f(bounds.x + bounds.width - italicBottom - outlineThickness, bounds.y + bounds.height - outlineThickness);

			if (advance)
				*advance = fontGlyph.advance;

			return true;
		}
		else
			return false;
	};

	void SimpleTextDrawer::OnFontAtlasLayerChanged(const Font* font, AbstractImage* oldLayer, AbstractImage* newLayer)
	{
		NazaraUnused(font);

#ifdef NAZARA_DEBUG
		if (m_font.get() != font)
		{
			NazaraInternalError("Not listening to {0}", PointerToString(font));
			return;
		}
#endif

		// Update atlas layer pointer
		// Note: This can happen while updating sprites
		for (Sprite& sprite : m_sprites)
		{
			if (sprite.atlas == oldLayer)
				sprite.atlas = newLayer;
		}
	}

	void SimpleTextDrawer::OnFontInvalidated(const Font* font)
	{
		NazaraUnused(font);

		#ifdef NAZARA_DEBUG
		if (m_font.get() != font)
		{
			NazaraInternalError("not listening to {0}", PointerToString(font));
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
			NazaraInternalError("not listening to {0}", PointerToString(font));
			return;
		}
		#endif

		SetTextFont(nullptr);
	}
}
