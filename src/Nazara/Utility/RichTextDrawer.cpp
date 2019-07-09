// Copyright (C) 2017 Jérôme Leclercq
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
	//m_outlineColor(Color::Black),
	m_defaultStyle(TextStyle_Regular),
	m_glyphUpdated(true),
	//m_maxLineWidth(std::numeric_limits<float>::infinity()),
	//m_outlineThickness(0.f),
	m_defaultCharacterSize(24)
	{
		SetDefaultFont(Font::GetDefault());
	}

	RichTextDrawer::RichTextDrawer(const RichTextDrawer& drawer) :
	m_defaultColor(drawer.m_defaultColor),
	m_defaultStyle(drawer.m_defaultStyle),
	m_fontIndexes(drawer.m_fontIndexes),
	m_blocks(drawer.m_blocks),
	m_glyphUpdated(false),
	//m_outlineColor(drawer.m_outlineColor),
	//m_maxLineWidth(drawer.m_maxLineWidth),
	//m_outlineThickness(drawer.m_outlineThickness),
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

	auto RichTextDrawer::AppendText(const String& str) -> BlockRef
	{
		NazaraAssert(!str.IsEmpty(), "String cannot be empty");

		std::size_t defaultFontIndex = HandleFontAddition(m_defaultFont);

		auto HasDefaultProperties = [&](const Block& block)
		{
			return block.characterSize == m_defaultCharacterSize &&
			       block.color         == m_defaultColor &&
			       block.fontIndex     == defaultFontIndex &&
			       block.style         == m_defaultStyle;
		};

		// Check if last block has the same property as default, else create a new block
		if (m_blocks.empty() || !HasDefaultProperties(m_blocks.back()))
		{
			m_blocks.emplace_back();
			Block& newBlock = m_blocks.back();
			newBlock.characterSize = m_defaultCharacterSize;
			newBlock.color = m_defaultColor;
			newBlock.fontIndex = defaultFontIndex;
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

	const Recti& RichTextDrawer::GetBounds() const
	{
		if (!m_glyphUpdated)
			UpdateGlyphs();

		return m_bounds;
	}

	Font* RichTextDrawer::GetFont(std::size_t index) const
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

	void RichTextDrawer::MergeBlocks()
	{
		if (m_blocks.size() < 2)
			return;

		auto TestBlockProperties = [](const Block& lhs, const Block& rhs)
		{
			return lhs.characterSize == rhs.characterSize &&
			       lhs.color == rhs.color &&
			       lhs.fontIndex == rhs.fontIndex &&
			       lhs.style == rhs.style;
		};

		auto lastBlockIt = m_blocks.begin();
		for (auto it = lastBlockIt + 1; it != m_blocks.end();)
		{
			if (TestBlockProperties(*lastBlockIt, *it))
			{
				// Append text to previous block and erase
				lastBlockIt->text += it->text;

				ReleaseFont(it->fontIndex);
				it = m_blocks.erase(it);
			}
			else
			{
				lastBlockIt = it;
				++it;
			}
		}
	}

	void RichTextDrawer::RemoveBlock(std::size_t index)
	{
		NazaraAssert(index < m_blocks.size(), "Invalid block index");

		ReleaseFont(m_blocks[index].fontIndex);
		m_blocks.erase(m_blocks.begin() + index);
	}

	RichTextDrawer& RichTextDrawer::operator=(const RichTextDrawer& drawer)
	{
		DisconnectFontSlots();

		m_blocks = drawer.m_blocks;
		m_defaultCharacterSize = drawer.m_defaultCharacterSize;
		m_defaultColor = drawer.m_defaultColor;
		m_defaultFont = drawer.m_defaultFont;
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
		m_blocks = std::move(drawer.m_blocks);
		m_bounds = std::move(m_bounds);
		m_defaultCharacterSize = std::move(drawer.m_defaultCharacterSize);
		m_defaultColor = std::move(drawer.m_defaultColor);
		m_defaultFont = std::move(drawer.m_defaultFont);
		m_defaultStyle = std::move(drawer.m_defaultStyle);
		m_drawPos = std::move(m_drawPos);
		m_fontIndexes = std::move(drawer.m_fontIndexes);
		m_fonts = std::move(drawer.m_fonts);
		m_glyphs = std::move(m_glyphs);
		m_lines = std::move(m_lines);
		m_glyphUpdated = std::move(m_glyphUpdated);
		m_workingBounds = std::move(m_workingBounds);

		drawer.DisconnectFontSlots();
		ConnectFontSlots();

		return *this;
	}

	bool RichTextDrawer::GenerateGlyph(Glyph& glyph, char32_t character, float outlineThickness, bool lineWrap, const Font* font, const Color& color, TextStyleFlags style, unsigned int characterSize, int renderOrder, int* advance) const
	{
		const Font::Glyph& fontGlyph = font->GetGlyph(characterSize, style, outlineThickness, character);
		if (fontGlyph.valid && fontGlyph.fauxOutlineThickness <= 0.f)
		{
			glyph.atlas = font->GetAtlas()->GetLayer(fontGlyph.layerIndex);
			glyph.atlasRect = fontGlyph.atlasRect;
			glyph.color = color;
			glyph.flipped = fontGlyph.flipped;
			glyph.renderOrder = renderOrder;

			glyph.bounds.Set(fontGlyph.aabb);

			//if (lineWrap && ShouldLineWrap(glyph, glyph.bounds.width))
			//	AppendNewLine(m_lastSeparatorGlyph, m_lastSeparatorPosition);

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

	void RichTextDrawer::GenerateGlyphs(const Font* font, const Color& color, TextStyleFlags style, unsigned int characterSize, const Color& outlineColor, float outlineThickness, const String& text) const
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

		char32_t previousCharacter = 0;

		const Font::SizeInfo& sizeInfo = font->GetSizeInfo(characterSize);

		float heightDifference = sizeInfo.lineHeight - m_lines.back().bounds.height;
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
		/*if (firstFont.font)
			m_lines.emplace_back(Line{ Rectf(0.f, 0.f, 0.f, float(font->GetSizeInfo(firstBlock.characterSize).lineHeight)), 0 });
		else
			m_lines.emplace_back(Line{ Rectf::Zero(), 0 });*/

		m_glyphs.reserve(m_glyphs.size() + characters.size() * ((outlineThickness > 0.f) ? 2 : 1));
		for (char32_t character : characters)
		{
			if (previousCharacter != 0)
				m_drawPos.x += font->GetKerning(characterSize, previousCharacter, character);

			previousCharacter = character;

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
				if (!GenerateGlyph(glyph, character, 0.f, true, font, color, style, characterSize, 0, &advance))
					continue; // Glyph failed to load, just skip it (can't do much)

				if (outlineThickness > 0.f)
				{
					Glyph outlineGlyph;
					if (GenerateGlyph(outlineGlyph, character, outlineThickness, false, font, outlineColor, style, characterSize, -1, nullptr))
					{
						m_glyphs.push_back(outlineGlyph);
					}
				}
			}
			else
			{
				float glyphAdvance = advance;

				//if (ShouldLineWrap(glyph, glyphAdvance))
				//	AppendNewLine(m_lastSeparatorGlyph, m_lastSeparatorPosition);

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
					AppendNewLine(font, characterSize);
					break;
				}

				default:
					m_drawPos.x += advance;
					break;
			}

			/*if (whitespace)
			{
				m_lastSeparatorGlyph = m_glyphs.size();
				m_lastSeparatorPosition = m_drawPos.x;
			}*/

			m_glyphs.push_back(glyph);
		}

		m_workingBounds.ExtendTo(m_lines.back().bounds);

		m_bounds.Set(Rectf(std::floor(m_workingBounds.x), std::floor(m_workingBounds.y), std::ceil(m_workingBounds.width), std::ceil(m_workingBounds.height)));

		m_glyphUpdated = true;
	}

	void RichTextDrawer::OnFontAtlasLayerChanged(const Font* font, AbstractImage* oldLayer, AbstractImage* newLayer)
	{
		NazaraUnused(font);

#ifdef NAZARA_DEBUG
		auto it = std::find_if(m_fonts.begin(), m_fonts.end(), [font](const auto& fontData) { return fontData.font == font; });
		if (it == m_fonts.end())
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

	void RichTextDrawer::OnFontInvalidated(const Font* font)
	{
		NazaraUnused(font);

#ifdef NAZARA_DEBUG
		auto it = std::find_if(m_fonts.begin(), m_fonts.end(), [font](const auto& fontData) { return fontData.font == font; });
		if (it == m_fonts.end())
		{
			NazaraInternalError("Not listening to " + String::Pointer(font));
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
		auto it = std::find_if(m_fonts.begin(), m_fonts.end(), [font](const auto& fontData) { return fontData.font == font; });
		if (it == m_fonts.end())
		{
			NazaraInternalError("Not listening to " + String::Pointer(font));
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
				m_lines.emplace_back(Line{ Rectf(0.f, 0.f, 0.f, float(firstFont.font->GetSizeInfo(firstBlock.characterSize).lineHeight)), 0 });
			else
				m_lines.emplace_back(Line{ Rectf::Zero(), 0 });

			m_drawPos.Set(0, firstBlock.characterSize);

			for (const Block& block : m_blocks)
			{
				assert(block.fontIndex < m_fonts.size());
				const auto& fontData = m_fonts[block.fontIndex];

				GenerateGlyphs(fontData.font, block.color, block.style, block.characterSize, block.color, 0.f, block.text);
			}
		}
	}
}
