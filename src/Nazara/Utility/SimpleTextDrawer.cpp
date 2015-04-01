// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/SimpleTextDrawer.hpp>
#include <memory>
#include <Nazara/Utility/Debug.hpp>

NzSimpleTextDrawer::NzSimpleTextDrawer() :
m_color(NzColor::White),
m_fontListener(this),
m_style(nzTextStyle_Regular)
{
	SetFont(NzFont::GetDefault());
}

const NzRectui& NzSimpleTextDrawer::GetBounds() const
{
	if (!m_glyphUpdated)
		UpdateGlyphs();

	return m_bounds;
}

unsigned int NzSimpleTextDrawer::GetCharacterSize() const
{
	return m_characterSize;
}

const NzColor& NzSimpleTextDrawer::GetColor() const
{
	return m_color;
}

NzFont* NzSimpleTextDrawer::GetFont() const
{
	return m_font;
}

NzFont* NzSimpleTextDrawer::GetFont(unsigned int index) const
{
	#if NAZARA_UTILITY_SAFE
	if (index > 0)
	{
		NazaraError("Font index out of range (" + NzString::Number(index) + " >= 1)");
		return nullptr;
	}
	#endif

	return m_font;
}

unsigned int NzSimpleTextDrawer::GetFontCount() const
{
	return 1;
}

const NzAbstractTextDrawer::Glyph& NzSimpleTextDrawer::GetGlyph(unsigned int index) const
{
	if (!m_glyphUpdated)
		UpdateGlyphs();

	return m_glyphs[index];
}

unsigned int NzSimpleTextDrawer::GetGlyphCount() const
{
	if (!m_glyphUpdated)
		UpdateGlyphs();

	return m_glyphs.size();
}

nzUInt32 NzSimpleTextDrawer::GetStyle() const
{
	return m_style;
}

const NzString& NzSimpleTextDrawer::GetText() const
{
	return m_text;
}

void NzSimpleTextDrawer::SetCharacterSize(unsigned int characterSize)
{
	m_characterSize = characterSize;

	m_glyphUpdated = false;
}

void NzSimpleTextDrawer::SetColor(const NzColor& color)
{
	m_color = color;

	m_glyphUpdated = false;
}

void NzSimpleTextDrawer::SetFont(NzFont* font)
{
	m_font = font;
	m_fontListener = font;

	m_glyphUpdated = false;
}

void NzSimpleTextDrawer::SetStyle(nzUInt32 style)
{
	m_style = style;

	m_glyphUpdated = false;
}

void NzSimpleTextDrawer::SetText(const NzString& str)
{
	m_text = str;

	m_glyphUpdated = false;
}

NzSimpleTextDrawer NzSimpleTextDrawer::Draw(const NzString& str, unsigned int characterSize, nzUInt32 style, const NzColor& color)
{
	NzSimpleTextDrawer drawer;
	drawer.SetCharacterSize(characterSize);
	drawer.SetColor(color);
	drawer.SetStyle(style);
	drawer.SetText(str);

	return drawer;
}

NzSimpleTextDrawer NzSimpleTextDrawer::Draw(NzFont* font, const NzString& str, unsigned int characterSize, nzUInt32 style, const NzColor& color)
{
	NzSimpleTextDrawer drawer;
	drawer.SetCharacterSize(characterSize);
	drawer.SetColor(color);
	drawer.SetFont(font);
	drawer.SetStyle(style);
	drawer.SetText(str);

	return drawer;
}

bool NzSimpleTextDrawer::OnObjectModified(const NzRefCounted* object, int index, unsigned int code)
{
	NazaraUnused(object);
	NazaraUnused(index);

	#ifdef NAZARA_DEBUG
	if (m_font != object)
	{
		NazaraInternalError("Not listening to " + NzString::Pointer(object));
		return false;
	}
	#endif

	if (code == NzFont::ModificationCode_AtlasChanged ||
	    code == NzFont::ModificationCode_AtlasLayerChanged ||
	    code == NzFont::ModificationCode_GlyphCacheCleared)
	{
		m_glyphUpdated = false;
	}

	return true;
}

void NzSimpleTextDrawer::OnObjectReleased(const NzRefCounted* object, int index)
{
	NazaraUnused(object);
	NazaraUnused(index);

	#ifdef NAZARA_DEBUG
	if (m_font != object)
	{
		NazaraInternalError("Not listening to " + NzString::Pointer(object));
		return;
	}
	#endif

	SetFont(nullptr);
}

void NzSimpleTextDrawer::UpdateGlyphs() const
{
	m_bounds.MakeZero();
	m_glyphs.clear();
	m_glyphUpdated = true;

	#if NAZARA_UTILITY_SAFE
	if (!m_font || !m_font->IsValid())
	{
		NazaraError("Invalid font");
		return;
	}
	#endif

	if (m_text.IsEmpty())
		return;

	///TODO: Itération UTF-8 => UTF-32 sans allocation de buffer (Exposer utf8cpp ?)
	std::u32string characters = m_text.GetUtf32String();
	if (characters.empty())
	{
		NazaraError("Invalid character set");
		return;
	}

	const NzFont::SizeInfo& sizeInfo = m_font->GetSizeInfo(m_characterSize);

	// "Curseur" de dessin
	NzVector2ui drawPos(0, m_characterSize);

	// On calcule les bornes en flottants pour accélérer les calculs (il est coûteux de changer de type trop souvent)
	bool firstGlyph = true;
	NzRectf textBounds = NzRectf::Zero();
	nzUInt32 previousCharacter = 0;

	m_glyphs.reserve(characters.size());
	for (char32_t character : characters)
	{
		if (previousCharacter != 0)
			drawPos.x += m_font->GetKerning(m_characterSize, previousCharacter, character);

		previousCharacter = character;

		bool whitespace = true;
		switch (character)
		{
			case ' ':
				drawPos.x += sizeInfo.spaceAdvance;
				break;

			case '\n':
				drawPos.x = 0;
				drawPos.y += sizeInfo.lineHeight;
				break;

			case '\t':
				drawPos.x += sizeInfo.spaceAdvance*4;
				break;

			default:
				whitespace = false;
				break;
		}

		if (whitespace)
			continue; // Inutile d'avoir un glyphe pour un espace blanc

		const NzFont::Glyph& fontGlyph = m_font->GetGlyph(m_characterSize, m_style, character);
		if (!fontGlyph.valid)
			continue; // Le glyphe n'a pas été correctement chargé, que pouvons-nous faire d'autre que le passer

		Glyph glyph;
		glyph.atlas = m_font->GetAtlas()->GetLayer(fontGlyph.layerIndex);
		glyph.atlasRect = fontGlyph.atlasRect;
		glyph.color = m_color;
		glyph.flipped = fontGlyph.flipped;

		int advance = fontGlyph.advance;

		NzRectf bounds(fontGlyph.aabb);
		bounds.x += drawPos.x;
		bounds.y += drawPos.y;

		if (fontGlyph.requireFauxBold)
		{
			// On va agrandir le glyphe pour simuler le gras (idée moisie, mais idée quand même)
			NzVector2f center = bounds.GetCenter();

			bounds.width *= 1.1f;
			bounds.height *= 1.1f;

			// On le replace à la bonne hauteur
			NzVector2f offset(bounds.GetCenter() - center);
			bounds.y -= offset.y;

			// On ajuste l'espacement
			advance += advance/10;
		}

		// On "penche" le glyphe pour obtenir un semblant d'italique
		float italic = (fontGlyph.requireFauxItalic) ? 0.208f : 0.f;
		float italicTop = italic * bounds.y;
		float italicBottom = italic * bounds.GetMaximum().y;

		glyph.corners[0].Set(bounds.x - italicTop, bounds.y);
		glyph.corners[1].Set(bounds.x + bounds.width - italicTop, bounds.y);
		glyph.corners[2].Set(bounds.x - italicBottom, bounds.y + bounds.height);
		glyph.corners[3].Set(bounds.x + bounds.width - italicBottom, bounds.y + bounds.height);

		if (firstGlyph)
		{
			textBounds.Set(glyph.corners[0]);
			firstGlyph = false;
		}

		for (unsigned int i = 0; i < 4; ++i)
			textBounds.ExtendTo(glyph.corners[i]);

		drawPos.x += advance;
		m_glyphs.push_back(glyph);
	}

	m_bounds.Set(NzRectf(std::floor(textBounds.x), std::floor(textBounds.y), std::ceil(textBounds.width), std::ceil(textBounds.height)));
}
