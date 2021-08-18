// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/RichTextDrawer.hpp>
#include <limits>
#include <memory>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	RichTextDrawer::RichTextDrawer() :
	m_defaultColor(Color::White),
	m_defaultOutlineColor(Color::Black),
	m_defaultStyle(TextStyle_Regular),
	m_glyphUpdated(false),
	m_defaultCharacterSpacingOffset(0.f),
	m_defaultLineSpacingOffset(0.f),
	m_defaultOutlineThickness(0.f),
	m_maxLineWidth(std::numeric_limits<float>::infinity()),
	m_defaultCharacterSize(24)
	{
		SetDefaultFont(Font::GetDefault());
	}

	RichTextDrawer::RichTextDrawer(const RichTextDrawer& drawer) :
	m_defaultColor(drawer.m_defaultColor),
	m_defaultOutlineColor(drawer.m_defaultOutlineColor),
	m_defaultStyle(drawer.m_defaultStyle),
	m_fontIndexes(drawer.m_fontIndexes),
	m_blocks(drawer.m_blocks),
	m_glyphUpdated(false),
	m_defaultCharacterSpacingOffset(drawer.m_defaultCharacterSpacingOffset),
	m_defaultLineSpacingOffset(drawer.m_defaultLineSpacingOffset),
	m_defaultOutlineThickness(drawer.m_defaultOutlineThickness),
	m_maxLineWidth(drawer.m_maxLineWidth),
	m_defaultCharacterSize(drawer.m_defaultCharacterSize)
	{
		m_fonts.resize(drawer.m_fonts.size());
		for (std::size_t i = 0; i < m_fonts.size(); ++i)
		{
			m_fonts[i].font = drawer.m_fonts[i].font;
			m_fonts[i].useCount = drawer.m_fonts[i].useCount;
		}

		SetDefaultFont(drawer.m_defaultFont);

		ConnectFontSlots();
	}

	RichTextDrawer::RichTextDrawer(RichTextDrawer&& drawer)
	{
		operator=(std::move(drawer));
	}

	RichTextDrawer::~RichTextDrawer() = default;

	auto RichTextDrawer::AppendText(const std::string& str, bool forceNewBlock) -> BlockRef
	{
		NazaraAssert(!str.empty(), "String cannot be empty");

		std::size_t defaultFontIndex = HandleFontAddition(m_defaultFont);

		auto HasDefaultProperties = [&](const Block& block)
		{
			return block.characterSize          == m_defaultCharacterSize &&
			       block.color                  == m_defaultColor &&
			       block.fontIndex              == defaultFontIndex &&
			       block.characterSpacingOffset == m_defaultCharacterSpacingOffset &&
			       block.lineSpacingOffset      == m_defaultLineSpacingOffset &&
			       block.outlineColor           == m_defaultOutlineColor &&
			       block.outlineThickness       == m_defaultOutlineThickness &&
			       block.style                  == m_defaultStyle;
		};

		// Check if last block has the same property as default, else create a new block
		if (forceNewBlock || m_blocks.empty() || !HasDefaultProperties(m_blocks.back()))
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
			newBlock.characterSize = m_defaultCharacterSize;
			newBlock.characterSpacingOffset = m_defaultCharacterSpacingOffset;
			newBlock.color = m_defaultColor;
			newBlock.fontIndex = defaultFontIndex;
			newBlock.glyphIndex = glyphIndex;
			newBlock.lineSpacingOffset = m_defaultLineSpacingOffset;
			newBlock.outlineColor = m_defaultOutlineColor;
			newBlock.outlineThickness = m_defaultOutlineThickness;
			newBlock.style = m_defaultStyle;
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
		m_glyphs.clear();
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
		NazaraAssert(index < m_fonts.size(), "Font index out of range");

		return m_fonts[index].font;
	}

	std::size_t RichTextDrawer::GetFontCount() const
	{
		return m_fonts.size();
	}

	const AbstractTextDrawer::Glyph& RichTextDrawer::GetGlyph(std::size_t index) const
	{
		if (!m_glyphUpdated)
			UpdateGlyphs();

		return m_glyphs[index];
	}

	std::size_t RichTextDrawer::GetGlyphCount() const
	{
		if (!m_glyphUpdated)
			UpdateGlyphs();

		return m_glyphs.size();
	}

	const AbstractTextDrawer::Line& RichTextDrawer::GetLine(std::size_t index) const
	{
		if (!m_glyphUpdated)
			UpdateGlyphs();

		NazaraAssert(index < m_lines.size(), "Line index out of range");
		return m_lines[index];
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
		NazaraAssert(index < m_blocks.size(), "Invalid block index");

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
		m_defaultCharacterSize = drawer.m_defaultCharacterSize;
		m_defaultCharacterSpacingOffset = drawer.m_defaultCharacterSpacingOffset;
		m_defaultColor = drawer.m_defaultColor;
		m_defaultFont = drawer.m_defaultFont;
		m_defaultLineSpacingOffset = drawer.m_defaultLineSpacingOffset;
		m_defaultOutlineColor = drawer.m_defaultOutlineColor;
		m_defaultOutlineThickness = drawer.m_defaultOutlineThickness;
		m_defaultStyle = drawer.m_defaultStyle;
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

	RichTextDrawer& RichTextDrawer::operator=(RichTextDrawer&& drawer)
	{
		DisconnectFontSlots();

		m_blocks = std::move(drawer.m_blocks);
		m_bounds = std::move(drawer.m_bounds);
		m_defaultCharacterSize = std::move(drawer.m_defaultCharacterSize);
		m_defaultCharacterSpacingOffset = std::move(drawer.m_defaultCharacterSpacingOffset);
		m_defaultColor = std::move(drawer.m_defaultColor);
		m_defaultFont = std::move(drawer.m_defaultFont);
		m_defaultLineSpacingOffset = std::move(drawer.m_defaultLineSpacingOffset);
		m_defaultOutlineColor = std::move(drawer.m_defaultOutlineColor);
		m_defaultOutlineThickness = std::move(drawer.m_defaultOutlineThickness);
		m_defaultStyle = std::move(drawer.m_defaultStyle);
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
			m_bounds.MakeZero();
			for (auto& line : m_lines)
				m_bounds.ExtendTo(line.bounds);
		}
	}

	bool RichTextDrawer::GenerateGlyph(Glyph& glyph, char32_t character, float outlineThickness, bool lineWrap, const Font& font, const Color& color, TextStyleFlags style, float lineSpacingOffset, unsigned int characterSize, int renderOrder, int* advance) const
	{
		const Font::Glyph& fontGlyph = font.GetGlyph(characterSize, style, outlineThickness, character);
		if (fontGlyph.valid && fontGlyph.fauxOutlineThickness <= 0.f)
		{
			glyph.atlas = font.GetAtlas()->GetLayer(fontGlyph.layerIndex);
			glyph.atlasRect = fontGlyph.atlasRect;
			glyph.color = color;
			glyph.flipped = fontGlyph.flipped;
			glyph.renderOrder = renderOrder;

			glyph.bounds.Set(fontGlyph.aabb);

			if (lineWrap && ShouldLineWrap(glyph.bounds.width))
				AppendNewLine(font, characterSize, lineSpacingOffset, m_lastSeparatorGlyph, m_lastSeparatorPosition);

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

	void RichTextDrawer::GenerateGlyphs(const Font& font, const Color& color, TextStyleFlags style, unsigned int characterSize, const Color& outlineColor, float characterSpacingOffset, float lineSpacingOffset, float outlineThickness, const std::string& text) const
	{
		if (text.empty())
			return;

		///TODO: Allow iteration on Unicode characters without allocating any buffer
		std::u32string characters = ToUtf32String(text);
		if (characters.empty())
		{
			NazaraError("Invalid character set");
			return;
		}

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

				for (auto& corner : glyph.corners)
					corner.y += heightDifference;
			}

			m_drawPos.y += heightDifference;
			m_lines.back().bounds.height += heightDifference;
		}

		m_glyphs.reserve(m_glyphs.size() + characters.size() * ((outlineThickness > 0.f) ? 2 : 1));
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

			Glyph glyph;
			if (!whitespace)
			{
				int iAdvance;
				if (!GenerateGlyph(glyph, character, 0.f, true, font, color, style, lineSpacingOffset, characterSize, 0, &iAdvance))
					continue; // Glyph failed to load, just skip it (can't do much)

				advance += float(iAdvance);

				if (outlineThickness > 0.f)
				{
					Glyph outlineGlyph;
					if (GenerateGlyph(outlineGlyph, character, outlineThickness, false, font, outlineColor, style, lineSpacingOffset, characterSize, -1, nullptr))
						m_glyphs.push_back(outlineGlyph);
				}
			}
			else
			{
				if (ShouldLineWrap(advance))
					AppendNewLine(font, characterSize, lineSpacingOffset, m_lastSeparatorGlyph, m_lastSeparatorPosition);

				glyph.atlas = nullptr;
				glyph.bounds.Set(m_drawPos.x, m_lines.back().bounds.y, advance, lineHeight);

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
					AppendNewLine(font, characterSize, lineSpacingOffset);
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

		m_bounds.ExtendTo(m_lines.back().bounds);

		m_glyphUpdated = true;
	}

	void RichTextDrawer::OnFontAtlasLayerChanged(const Font* font, AbstractImage* oldLayer, AbstractImage* newLayer)
	{
		NazaraUnused(font);

#ifdef NAZARA_DEBUG
		auto it = std::find_if(m_fonts.begin(), m_fonts.end(), [font](const auto& fontData) { return fontData.font.get() == font; });
		if (it == m_fonts.end())
		{
			NazaraInternalError("Not listening to " + PointerToString(font));
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

	void RichTextDrawer::OnFontInvalidated(const Font* font)
	{
		NazaraUnused(font);

#ifdef NAZARA_DEBUG
		auto it = std::find_if(m_fonts.begin(), m_fonts.end(), [font](const auto& fontData) { return fontData.font.get() == font; });
		if (it == m_fonts.end())
		{
			NazaraInternalError("Not listening to " + PointerToString(font));
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
			NazaraInternalError("Not listening to " + PointerToString(font));
			return;
		}
#endif

		//SetFont(nullptr);
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
				m_lines.emplace_back(Line{ Rectf(0.f, 0.f, 0.f, GetLineHeight(firstBlock)), 0 });
			else
				m_lines.emplace_back(Line{ Rectf::Zero(), 0 });

			m_drawPos.Set(0, float(firstBlock.characterSize));

			for (const Block& block : m_blocks)
			{
				assert(block.fontIndex < m_fonts.size());
				const auto& fontData = m_fonts[block.fontIndex];

				GenerateGlyphs(*fontData.font, block.color, block.style, block.characterSize, block.outlineColor, block.outlineThickness, block.characterSpacingOffset, block.lineSpacingOffset, block.text);
			}
		}
		else
			m_lines.emplace_back(Line{ Rectf::Zero(), 0 }); //< Ensure there's always a line
	}
}
