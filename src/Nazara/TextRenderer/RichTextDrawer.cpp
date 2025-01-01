// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Text renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/TextRenderer/RichTextDrawer.hpp>
#include <limits>
#include <memory>

namespace Nz
{
	RichTextDrawer::RichTextDrawer() :
	m_currentColor(Color::White()),
	m_currentOutlineColor(Color::Black()),
	m_currentStyle(TextStyle_Regular),
	m_glyphUpdated(false),
	m_currentCharacterSpacingOffset(0.f),
	m_currentLineSpacingOffset(0.f),
	m_currentOutlineThickness(0.f),
	m_maxLineWidth(std::numeric_limits<float>::infinity()),
	m_currentCharacterSize(24)
	{
		SetTextFont(Font::GetDefault());
	}

	RichTextDrawer::RichTextDrawer(const RichTextDrawer& drawer) :
	m_currentColor(drawer.m_currentColor),
	m_currentOutlineColor(drawer.m_currentOutlineColor),
	m_currentStyle(drawer.m_currentStyle),
	m_fontIndexes(drawer.m_fontIndexes),
	m_blocks(drawer.m_blocks),
	m_glyphUpdated(false),
	m_currentCharacterSpacingOffset(drawer.m_currentCharacterSpacingOffset),
	m_currentLineSpacingOffset(drawer.m_currentLineSpacingOffset),
	m_currentOutlineThickness(drawer.m_currentOutlineThickness),
	m_maxLineWidth(drawer.m_maxLineWidth),
	m_currentCharacterSize(drawer.m_currentCharacterSize)
	{
		m_fonts.resize(drawer.m_fonts.size());
		for (std::size_t i = 0; i < m_fonts.size(); ++i)
		{
			m_fonts[i].font = drawer.m_fonts[i].font;
			m_fonts[i].useCount = drawer.m_fonts[i].useCount;
		}

		SetTextFont(drawer.m_currentFont);

		ConnectFontSlots();
	}

	RichTextDrawer::RichTextDrawer(RichTextDrawer&& drawer) noexcept
	{
		operator=(std::move(drawer));
	}

	RichTextDrawer::~RichTextDrawer() = default;

	auto RichTextDrawer::AppendText(std::string_view str, bool forceNewBlock) -> BlockRef
	{
		NazaraAssertMsg(!str.empty(), "String cannot be empty");

		std::size_t currentFontIndex = HandleFontAddition(m_currentFont);

		auto DoPropertiesMatch = [&](const Block& block)
		{
			return block.characterSize          == m_currentCharacterSize &&
			       block.color                  == m_currentColor &&
			       block.fontIndex              == currentFontIndex &&
			       block.characterSpacingOffset == m_currentCharacterSpacingOffset &&
			       block.lineSpacingOffset      == m_currentLineSpacingOffset &&
			       block.outlineColor           == m_currentOutlineColor &&
			       block.outlineThickness       == m_currentOutlineThickness &&
			       block.style                  == m_currentStyle;
		};

		// Check if last block has the same properties as previous block, else create a new block
		if (forceNewBlock || m_blocks.empty() || !DoPropertiesMatch(m_blocks.back()))
		{
			std::size_t glyphIndex;
			if (!m_blocks.empty())
			{
				Block& lastBlock = m_blocks.back();
				glyphIndex = lastBlock.glyphIndex + lastBlock.text.size();
			}
			else
				glyphIndex = 0;

			m_blocks.emplace_back();
			Block& newBlock = m_blocks.back();
			newBlock.characterSize = m_currentCharacterSize;
			newBlock.characterSpacingOffset = m_currentCharacterSpacingOffset;
			newBlock.color = m_currentColor;
			newBlock.fontIndex = currentFontIndex;
			newBlock.glyphIndex = glyphIndex;
			newBlock.lineSpacingOffset = m_currentLineSpacingOffset;
			newBlock.outlineColor = m_currentOutlineColor;
			newBlock.outlineThickness = m_currentOutlineThickness;
			newBlock.style = m_currentStyle;
			newBlock.text = str;

			assert(newBlock.fontIndex < m_fonts.size());
			m_fonts[newBlock.fontIndex].useCount++;
		}
		else
			m_blocks.back().text += str;

		InvalidateGlyphs();

		return BlockRef(*this, m_blocks.size() - 1);
	}

	void RichTextDrawer::Clear()
	{
		m_fontIndexes.clear();
		m_blocks.clear();
		m_fonts.clear();
		ClearGlyphs();
	}

	const Rectf& RichTextDrawer::GetBounds() const
	{
		if (!m_glyphUpdated)
			UpdateGlyphs();

		return m_bounds;
	}

	const std::shared_ptr<Font>& RichTextDrawer::GetFont(std::size_t index) const
	{
		NazaraAssertMsg(index < m_fonts.size(), "Font index out of range");

		return m_fonts[index].font;
	}

	std::size_t RichTextDrawer::GetFontCount() const
	{
		return m_fonts.size();
	}

	const AbstractTextDrawer::Glyph* RichTextDrawer::GetGlyphs() const
	{
		if (!m_glyphUpdated)
			UpdateGlyphs();

		return m_glyphs.data();
	}

	std::size_t RichTextDrawer::GetGlyphCount() const
	{
		if (!m_glyphUpdated)
			UpdateGlyphs();

		return m_glyphs.size();
	}

	const AbstractTextDrawer::Line* RichTextDrawer::GetLines() const
	{
		if (!m_glyphUpdated)
			UpdateGlyphs();

		return m_lines.data();
	}

	std::size_t RichTextDrawer::GetLineCount() const
	{
		if (!m_glyphUpdated)
			UpdateGlyphs();

		return m_lines.size();
	}

	float RichTextDrawer::GetMaxLineWidth() const
	{
		return m_maxLineWidth;
	}

	const AbstractTextDrawer::Sprite* RichTextDrawer::GetSprites() const
	{
		if (!m_glyphUpdated)
			UpdateGlyphs();

		return m_sprites.data();
	}

	std::size_t RichTextDrawer::GetSpriteCount() const
	{
		if (!m_glyphUpdated)
			UpdateGlyphs();

		return m_sprites.size();
	}

	void RichTextDrawer::MergeBlocks()
	{
		auto TestBlockProperties = [](const Block& lhs, const Block& rhs)
		{
			return lhs.characterSize          == rhs.characterSize &&
			       lhs.color                  == rhs.color &&
			       lhs.fontIndex              == rhs.fontIndex &&
			       lhs.characterSpacingOffset == rhs.characterSpacingOffset &&
			       lhs.lineSpacingOffset      == rhs.lineSpacingOffset &&
			       lhs.outlineColor           == rhs.outlineColor &&
			       lhs.outlineThickness       == rhs.outlineThickness &&
			       lhs.style                  == rhs.style;
		};

		std::size_t previousBlockIndex = 0;
		for (std::size_t i = 1; i < m_blocks.size(); ++i)
		{
			if (TestBlockProperties(m_blocks[previousBlockIndex], m_blocks[i]))
			{
				m_blocks[previousBlockIndex].text += m_blocks[i].text;

				RemoveBlock(i);
				--i;
			}
			else
				previousBlockIndex = i;
		}
	}

	void RichTextDrawer::RemoveBlock(std::size_t index)
	{
		NazaraAssertMsg(index < m_blocks.size(), "Invalid block index");

		std::size_t textLength = m_blocks[index].text.size();

		ReleaseFont(m_blocks[index].fontIndex);
		m_blocks.erase(m_blocks.begin() + index);

		for (std::size_t i = index; i < m_blocks.size(); ++i)
		{
			assert(m_blocks[i].glyphIndex > textLength);
			m_blocks[i].glyphIndex -= textLength;
		}

		InvalidateGlyphs();
	}

	void RichTextDrawer::SetMaxLineWidth(float lineWidth)
	{
		m_maxLineWidth = lineWidth;

		InvalidateGlyphs();
	}

	RichTextDrawer& RichTextDrawer::operator=(const RichTextDrawer& drawer)
	{
		DisconnectFontSlots();

		m_blocks = drawer.m_blocks;
		m_currentCharacterSize = drawer.m_currentCharacterSize;
		m_currentCharacterSpacingOffset = drawer.m_currentCharacterSpacingOffset;
		m_currentColor = drawer.m_currentColor;
		m_currentFont = drawer.m_currentFont;
		m_currentLineSpacingOffset = drawer.m_currentLineSpacingOffset;
		m_currentOutlineColor = drawer.m_currentOutlineColor;
		m_currentOutlineThickness = drawer.m_currentOutlineThickness;
		m_currentStyle = drawer.m_currentStyle;
		m_fontIndexes = drawer.m_fontIndexes;

		m_fonts.resize(drawer.m_fonts.size());
		for (std::size_t i = 0; i < m_fonts.size(); ++i)
		{
			m_fonts[i].font = drawer.m_fonts[i].font;
			m_fonts[i].useCount = drawer.m_fonts[i].useCount;
		}

		ConnectFontSlots();
		InvalidateGlyphs();

		return *this;
	}

	RichTextDrawer& RichTextDrawer::operator=(RichTextDrawer&& drawer) noexcept
	{
		DisconnectFontSlots();

		m_blocks = std::move(drawer.m_blocks);
		m_bounds = std::move(drawer.m_bounds);
		m_currentCharacterSize = std::move(drawer.m_currentCharacterSize);
		m_currentCharacterSpacingOffset = std::move(drawer.m_currentCharacterSpacingOffset);
		m_currentColor = std::move(drawer.m_currentColor);
		m_currentFont = std::move(drawer.m_currentFont);
		m_currentLineSpacingOffset = std::move(drawer.m_currentLineSpacingOffset);
		m_currentOutlineColor = std::move(drawer.m_currentOutlineColor);
		m_currentOutlineThickness = std::move(drawer.m_currentOutlineThickness);
		m_currentStyle = std::move(drawer.m_currentStyle);
		m_drawPos = std::move(drawer.m_drawPos);
		m_fontIndexes = std::move(drawer.m_fontIndexes);
		m_fonts = std::move(drawer.m_fonts);
		m_glyphs = std::move(drawer.m_glyphs);
		m_lines = std::move(drawer.m_lines);
		m_glyphUpdated = std::move(drawer.m_glyphUpdated);

		drawer.DisconnectFontSlots();
		ConnectFontSlots();

		return *this;
	}

	void RichTextDrawer::AppendNewLine(const Font& font, unsigned int characterSize, float lineSpacingOffset, std::size_t glyphIndex, float glyphPosition) const
	{
		// Ensure we're appending from last line
		Line& lastLine = m_lines.back();

		const Font::SizeInfo& sizeInfo = font.GetSizeInfo(characterSize);

		float previousDrawPos = m_drawPos.x;

		float lineHeight = GetLineHeight(lineSpacingOffset, sizeInfo);

		// Reset cursor
		m_drawPos.x = 0;
		m_drawPos.y += lineHeight;
		m_lastSeparatorGlyph = InvalidGlyph;

		m_bounds.ExtendTo(lastLine.bounds);
		m_lines.emplace_back(Line{ m_glyphs.size() + 1, Rectf(0.f, lastLine.bounds.y + lastLine.bounds.height, 0.f, lineHeight), true });

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

	void RichTextDrawer::GenerateGlyphs(const Font& font, const Color& color, TextStyleFlags style, unsigned int characterSize, const Color& outlineColor, float outlineThickness, float characterSpacingOffset, float lineSpacingOffset, std::string_view text) const
	{
		if (text.empty())
			return;

		char32_t previousCharacter = 0;

		const Font::SizeInfo& sizeInfo = font.GetSizeInfo(characterSize);
		float lineHeight = GetLineHeight(lineSpacingOffset, sizeInfo);

		float heightDifference = lineHeight - m_lines.back().bounds.height;
		if (heightDifference > 0.f)
		{
			for (std::size_t glyphIndex = m_lines.back().glyphIndex; glyphIndex < m_glyphs.size(); ++glyphIndex)
			{
				Glyph& glyph = m_glyphs[glyphIndex];
				glyph.bounds.y += heightDifference;

				for (std::size_t spriteIndex : { glyph.spriteIndex, glyph.outlineSpriteIndex })
				{
					if (spriteIndex == InvalidIndex)
						continue;

					for (auto& corner : m_sprites[spriteIndex].corners)
						corner.y += heightDifference;
				}
			}

			m_drawPos.y += heightDifference;
			m_lines.back().bounds.height += heightDifference;
		}

		IterateOnCodepoints(text, [&](std::u32string_view characters)
		{
			for (char32_t character : characters)
			{
				if (previousCharacter != 0)
					m_drawPos.x += font.GetKerning(characterSize, previousCharacter, character);

				previousCharacter = character;

				bool whitespace = true;
				float advance = characterSpacingOffset;
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

				int glyphRenderOrder = (outlineThickness != 0.f && !style.Test(TextStyle::OutlineOnly)) ? 1 : 0;

				Glyph glyph;
				if (!whitespace)
				{
					Sprite sprite;
					int iAdvance;
					if (!GenerateSprite(glyph.bounds, sprite, character, (style.Test(TextStyle::OutlineOnly)) ? outlineThickness : 0.f, true, font, (style.Test(TextStyle::OutlineOnly)) ? outlineColor : color, style, lineSpacingOffset, characterSize, glyphRenderOrder, &iAdvance))
						continue; // Glyph failed to load, just skip it (can't do much)

					glyph.spriteIndex = m_sprites.size();
					m_sprites.push_back(sprite);

					if (outlineThickness != 0.f && !style.Test(TextStyle::OutlineOnly))
					{
						Sprite outlineSprite;
						if (GenerateSprite(glyph.bounds, outlineSprite, character, outlineThickness, false, font, outlineColor, style, lineSpacingOffset, characterSize, glyphRenderOrder - 1, nullptr))
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
						AppendNewLine(font, characterSize, lineSpacingOffset, m_lastSeparatorGlyph, m_lastSeparatorPosition);

					glyph.bounds = Rectf(m_drawPos.x, m_lines.back().bounds.y, advance, lineHeight);
				}

				m_lines.back().bounds.ExtendTo(glyph.bounds);

				if (character == '\n')
				{
					m_lines.back().allowsOvershoot = false;
					AppendNewLine(font, characterSize, lineSpacingOffset);
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

		m_glyphUpdated = true;
	}

	bool RichTextDrawer::GenerateSprite(Rectf& bounds, Sprite& sprite, char32_t character, float outlineThickness, bool lineWrap, const Font& font, const Color& color, TextStyleFlags style, float lineSpacingOffset, unsigned int characterSize, int renderOrder, int* advance) const
	{
		const Font::Glyph& fontGlyph = font.GetGlyph(characterSize, style, outlineThickness, character);
		if (fontGlyph.valid && fontGlyph.fauxOutlineThickness <= 0.f)
		{
			sprite.atlas = font.GetAtlas()->GetLayer(fontGlyph.layerIndex);
			sprite.atlasRect = fontGlyph.atlasRect;
			sprite.color = color;
			sprite.flipped = fontGlyph.flipped;
			sprite.renderOrder = renderOrder;

			bounds = Rectf(fontGlyph.aabb);

			if (lineWrap && ShouldLineWrap(bounds.width))
				AppendNewLine(font, characterSize, lineSpacingOffset, m_lastSeparatorGlyph, m_lastSeparatorPosition);

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

	void RichTextDrawer::OnFontAtlasLayerChanged(const Font* font, AbstractImage* oldLayer, AbstractImage* newLayer)
	{
		NazaraUnused(font);

#ifdef NAZARA_DEBUG
		auto it = std::find_if(m_fonts.begin(), m_fonts.end(), [font](const auto& fontData) { return fontData.font.get() == font; });
		if (it == m_fonts.end())
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

	void RichTextDrawer::OnFontInvalidated(const Font* font)
	{
		NazaraUnused(font);

#ifdef NAZARA_DEBUG
		auto it = std::find_if(m_fonts.begin(), m_fonts.end(), [font](const auto& fontData) { return fontData.font.get() == font; });
		if (it == m_fonts.end())
		{
			NazaraInternalError("Not listening to {0}", PointerToString(font));
			return;
		}
#endif

		m_glyphUpdated = false;
	}

	void RichTextDrawer::OnFontRelease(const Font* font)
	{
		NazaraUnused(font);
		NazaraUnused(font);

#ifdef NAZARA_DEBUG
		auto it = std::find_if(m_fonts.begin(), m_fonts.end(), [font](const auto& fontData) { return fontData.font.get() == font; });
		if (it == m_fonts.end())
		{
			NazaraInternalError("Not listening to {0}", PointerToString(font));
			return;
		}
#endif

		//SetTextFont(nullptr);
	}

	void RichTextDrawer::UpdateGlyphs() const
	{
		ClearGlyphs();

		if (!m_blocks.empty())
		{
			const Block& firstBlock = m_blocks.front();

			assert(firstBlock.fontIndex < m_fonts.size());
			const auto& firstFont = m_fonts[firstBlock.fontIndex];

			if (firstFont.font)
				m_lines.emplace_back(Line{ 0, Rectf(0.f, 0.f, 0.f, GetLineHeight(firstBlock)) });
			else
				m_lines.emplace_back(Line{ 0, Rectf::Zero(), true });

			m_drawPos = Vector2f(0.f, SafeCast<float>(firstBlock.characterSize));

			for (const Block& block : m_blocks)
			{
				assert(block.fontIndex < m_fonts.size());
				const auto& fontData = m_fonts[block.fontIndex];

				GenerateGlyphs(*fontData.font, block.color, block.style, block.characterSize, block.outlineColor, block.outlineThickness, block.lineSpacingOffset, block.characterSpacingOffset, block.text);
			}
		}
		else
			m_lines.emplace_back(Line{ 0, Rectf::Zero(), true }); //< Ensure there's always a line
	}
}
