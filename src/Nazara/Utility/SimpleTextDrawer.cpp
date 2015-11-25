// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/SimpleTextDrawer.hpp>
#include <memory>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	SimpleTextDrawer::SimpleTextDrawer() :
	m_color(Color::White),
	m_style(TextStyle_Regular),
	m_glyphUpdated(false)
	{
		SetFont(Font::GetDefault());
	}

	SimpleTextDrawer::SimpleTextDrawer(const SimpleTextDrawer& drawer) :
	m_color(drawer.m_color),
	m_text(drawer.m_text),
	m_style(drawer.m_style),
	m_glyphUpdated(false),
	m_characterSize(drawer.m_characterSize)
	{
		SetFont(drawer.m_font);
	}

	SimpleTextDrawer::SimpleTextDrawer(SimpleTextDrawer&& drawer)
	{
		operator=(std::move(drawer));
	}


	SimpleTextDrawer::~SimpleTextDrawer() = default;

	const Rectui& SimpleTextDrawer::GetBounds() const
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

	Font* SimpleTextDrawer::GetFont(unsigned int index) const
	{
		#if NAZARA_UTILITY_SAFE
		if (index > 0)
		{
			NazaraError("Font index out of range (" + String::Number(index) + " >= 1)");
			return nullptr;
		}
		#endif

		return m_font;
	}

	unsigned int SimpleTextDrawer::GetFontCount() const
	{
		return 1;
	}

	const AbstractTextDrawer::Glyph& SimpleTextDrawer::GetGlyph(unsigned int index) const
	{
		if (!m_glyphUpdated)
			UpdateGlyphs();

		return m_glyphs[index];
	}

	unsigned int SimpleTextDrawer::GetGlyphCount() const
	{
		if (!m_glyphUpdated)
			UpdateGlyphs();

		return m_glyphs.size();
	}

	UInt32 SimpleTextDrawer::GetStyle() const
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

		m_glyphUpdated = false;
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

	void SimpleTextDrawer::SetStyle(UInt32 style)
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
		m_style = drawer.m_style;
		m_text = drawer.m_text;

		m_glyphUpdated = false;
		SetFont(drawer.m_font);

		return *this;
	}

	SimpleTextDrawer& SimpleTextDrawer::operator=(SimpleTextDrawer&& drawer)
	{
		DisconnectFontSlots();

		m_bounds = std::move(drawer.m_bounds);
		m_characterSize = std::move(drawer.m_characterSize);
		m_color = std::move(drawer.m_color);
		m_glyphs = std::move(drawer.m_glyphs);
		m_glyphUpdated = std::move(drawer.m_glyphUpdated);
		m_font = std::move(drawer.m_font);
		m_style = std::move(drawer.m_style);
		m_text = std::move(drawer.m_text);

		// Update slot pointers (TODO: Improve the way of doing this)
		ConnectFontSlots();

		return *this;
	}

	SimpleTextDrawer SimpleTextDrawer::Draw(const String& str, unsigned int characterSize, UInt32 style, const Color& color)
	{
		SimpleTextDrawer drawer;
		drawer.SetCharacterSize(characterSize);
		drawer.SetColor(color);
		drawer.SetStyle(style);
		drawer.SetText(str);

		return drawer;
	}

	SimpleTextDrawer SimpleTextDrawer::Draw(Font* font, const String& str, unsigned int characterSize, UInt32 style, const Color& color)
	{
		SimpleTextDrawer drawer;
		drawer.SetCharacterSize(characterSize);
		drawer.SetColor(color);
		drawer.SetFont(font);
		drawer.SetStyle(style);
		drawer.SetText(str);

		return drawer;
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
		// Note: This can happend while updating
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

	void SimpleTextDrawer::UpdateGlyphs() const
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

		const Font::SizeInfo& sizeInfo = m_font->GetSizeInfo(m_characterSize);

		// "Curseur" de dessin
		Vector2ui drawPos(0, m_characterSize);

		// On calcule les bornes en flottants pour accélérer les calculs (il est coûteux de changer de type trop souvent)
		bool firstGlyph = true;
		Rectf textBounds = Rectf::Zero();
		UInt32 previousCharacter = 0;

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

			const Font::Glyph& fontGlyph = m_font->GetGlyph(m_characterSize, m_style, character);
			if (!fontGlyph.valid)
				continue; // Le glyphe n'a pas été correctement chargé, que pouvons-nous faire d'autre que le passer

			Glyph glyph;
			glyph.atlas = m_font->GetAtlas()->GetLayer(fontGlyph.layerIndex);
			glyph.atlasRect = fontGlyph.atlasRect;
			glyph.color = m_color;
			glyph.flipped = fontGlyph.flipped;

			int advance = fontGlyph.advance;

			Rectf bounds(fontGlyph.aabb);
			bounds.x += drawPos.x;
			bounds.y += drawPos.y;

			if (fontGlyph.requireFauxBold)
			{
				// On va agrandir le glyphe pour simuler le gras (idée moisie, mais idée quand même)
				Vector2f center = bounds.GetCenter();

				bounds.width *= 1.1f;
				bounds.height *= 1.1f;

				// On le replace à la bonne hauteur
				Vector2f offset(bounds.GetCenter() - center);
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

		m_bounds.Set(Rectf(std::floor(textBounds.x), std::floor(textBounds.y), std::ceil(textBounds.width), std::ceil(textBounds.height)));
	}
}
