// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Text renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/TextRenderer/Font.hpp>
#include <Nazara/Core/Export.hpp>
#include <Nazara/Core/GuillotineImageAtlas.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/TextRenderer/FontData.hpp>
#include <Nazara/TextRenderer/FontGlyph.hpp>
#include <Nazara/TextRenderer/TextRenderer.hpp>

namespace Nz
{
	namespace
	{
		const UInt8 r_sansationRegular[] = {
			#include <Nazara/TextRenderer/Resources/Fonts/OpenSans-Regular.ttf.h>
		};
	}

	bool FontParams::IsValid() const
	{
		return true; // Nothing to test
	}

	Font::Font() :
	m_glyphBorder(s_defaultGlyphBorder),
	m_minimumStepSize(s_defaultMinimumStepSize)
	{
		SetAtlas(s_defaultAtlas);
	}

	Font::~Font()
	{
		OnFontRelease(this);

		Destroy();
		SetAtlas({}); // Reset our atlas
	}

	void Font::ClearGlyphCache()
	{
		if (m_atlas)
		{
			if (m_atlas.use_count() == 1)
				m_atlas->Clear(); // Will call OnAtlasCleared
			else
			{
				// At least one font is using this atlas, remove our glyphes
				for (auto&& [glyphKey, glyphMap] : m_glyphes)
				{
					NazaraUnused(glyphKey);
					for (auto&& [character, glyph] : glyphMap)
					{
						NazaraUnused(character);
						m_atlas->Free(&glyph.atlasRect, &glyph.layerIndex, 1);
					}
				}

				// Free all cached glyphes
				m_glyphes.clear();

				OnFontGlyphCacheCleared(this);
			}
		}
	}

	void Font::ClearKerningCache()
	{
		m_kerningCache.clear();

		OnFontKerningCacheCleared(this);
	}

	void Font::ClearSizeInfoCache()
	{
		m_sizeInfoCache.clear();

		OnFontSizeInfoCacheCleared(this);
	}

	bool Font::Create(std::unique_ptr<FontData> data)
	{
		NazaraAssert(data, "invalid font data");

		Destroy();
		m_data = std::move(data);
		return true;
	}

	void Font::Destroy()
	{
		if (m_data)
		{
			OnFontDestroy(this);

			ClearGlyphCache();

			m_data.reset();
			m_kerningCache.clear();
			m_sizeInfoCache.clear();
		}
	}

	bool Font::ExtractGlyph(unsigned int characterSize, char32_t character, TextStyleFlags style, float outlineThickness, FontGlyph* glyph) const
	{
		NazaraAssert(IsValid(), "invalid font");
		return m_data->ExtractGlyph(characterSize, character, style, outlineThickness, glyph);
	}

	const std::shared_ptr<AbstractAtlas>& Font::GetAtlas() const
	{
		return m_atlas;
	}

	std::size_t Font::GetCachedGlyphCount(unsigned int characterSize, TextStyleFlags style, float outlineThickness) const
	{
		UInt64 key = ComputeKey(characterSize, style, outlineThickness);
		auto it = m_glyphes.find(key);
		if (it == m_glyphes.end())
			return 0;

		return it->second.size();
	}

	std::size_t Font::GetCachedGlyphCount() const
	{
		std::size_t count = 0;
		for (auto& pair : m_glyphes)
			count += pair.second.size();

		return count;
	}

	std::string Font::GetFamilyName() const
	{
		NazaraAssert(IsValid(), "invalid font");
		return m_data->GetFamilyName();
	}

	int Font::GetKerning(unsigned int characterSize, char32_t first, char32_t second) const
	{
		NazaraAssert(IsValid(), "invalid font");

		// Use a cache as QueryKerning may be costly (may induce an internal size change)
		auto& map = m_kerningCache[characterSize];

		UInt64 key = (static_cast<UInt64>(first) << 32) | second;

		auto it = map.find(key);
		if (it == map.end())
		{
			int kerning = m_data->QueryKerning(characterSize, first, second);
			map.insert(std::make_pair(key, kerning));

			return kerning;
		}
		else
			return it->second;
	}

	const Font::Glyph& Font::GetGlyph(unsigned int characterSize, TextStyleFlags style, float outlineThickness, char32_t character) const
	{
		UInt64 key = ComputeKey(characterSize, style, outlineThickness);
		return PrecacheGlyph(m_glyphes[key], characterSize, style, outlineThickness, character);
	}

	unsigned int Font::GetGlyphBorder() const
	{
		return m_glyphBorder;
	}

	unsigned int Font::GetMinimumStepSize() const
	{
		return m_minimumStepSize;
	}

	const Font::SizeInfo& Font::GetSizeInfo(unsigned int characterSize) const
	{
		NazaraAssert(IsValid(), "invalid font");

		auto it = m_sizeInfoCache.find(characterSize);
		if (it == m_sizeInfoCache.end())
		{
			SizeInfo sizeInfo;
			sizeInfo.lineHeight = m_data->QueryLineHeight(characterSize);
			sizeInfo.underlinePosition = m_data->QueryUnderlinePosition(characterSize);
			sizeInfo.underlineThickness = m_data->QueryUnderlineThickness(characterSize);

			FontGlyph glyph;
			if (m_data->ExtractGlyph(characterSize, ' ', TextStyle_Regular, 0.f, &glyph))
				sizeInfo.spaceAdvance = glyph.advance;
			else
			{
				NazaraWarning("Failed to extract space character from font, using half the character size");
				sizeInfo.spaceAdvance = characterSize / 2;
			}

			it = m_sizeInfoCache.insert(std::make_pair(characterSize, sizeInfo)).first;
		}

		return it->second;
	}

	std::string Font::GetStyleName() const
	{
		NazaraAssert(IsValid(), "invalid font");
		return m_data->GetStyleName();
	}

	bool Font::IsValid() const
	{
		return m_data != nullptr;
	}

	bool Font::Precache(unsigned int characterSize, TextStyleFlags style, float outlineThickness, char32_t character) const
	{
		UInt64 key = ComputeKey(characterSize, style, outlineThickness);
		return PrecacheGlyph(m_glyphes[key], characterSize, style, outlineThickness, character).valid;
	}

	bool Font::Precache(unsigned int characterSize, TextStyleFlags style, float outlineThickness, std::string_view characterSet) const
	{
		NazaraAssert(!characterSet.empty(), "empty character set");

		UInt64 key = ComputeKey(characterSize, style, outlineThickness);
		auto& glyphMap = m_glyphes[key];

		IterateOnCodepoints(characterSet, [&](std::u32string_view characters)
		{
			for (char32_t character : characters)
				PrecacheGlyph(glyphMap, characterSize, style, outlineThickness, character);

			return true;
		});

		return true;
	}

	void Font::SetAtlas(std::shared_ptr<AbstractAtlas> atlas)
	{
		if (m_atlas != atlas)
		{
			ClearGlyphCache();

			m_atlas = std::move(atlas);
			if (m_atlas)
			{
				m_atlasClearedSlot.Connect(m_atlas->OnAtlasCleared, this, &Font::OnAtlasCleared);
				m_atlasLayerChangeSlot.Connect(m_atlas->OnAtlasLayerChange, this, &Font::OnAtlasLayerChange);
			}
			else
			{
				m_atlasClearedSlot.Disconnect();
				m_atlasLayerChangeSlot.Disconnect();
			}

			OnFontAtlasChanged(this);
		}
	}

	void Font::SetGlyphBorder(unsigned int borderSize)
	{
		if (m_glyphBorder != borderSize)
		{
			m_glyphBorder = borderSize;
			ClearGlyphCache();
		}
	}

	void Font::SetMinimumStepSize(unsigned int minimumStepSize)
	{
		if (m_minimumStepSize != minimumStepSize)
		{
			NazaraAssert(minimumStepSize != 0, "Minimum step size cannot be zero");

			m_minimumStepSize = minimumStepSize;
			ClearGlyphCache();
		}
	}

	std::shared_ptr<AbstractAtlas> Font::GetDefaultAtlas()
	{
		return s_defaultAtlas;
	}

	const std::shared_ptr<Font>& Font::GetDefault()
	{
		// Nous n'initialisons la police par défaut qu'à la demande pour qu'elle prenne
		// les paramètres par défaut (qui peuvent avoir étés changés par l'utilisateur),
		// et pour ne pas consommer de la mémoire vive inutilement (si elle n'est jamais utilisée, elle n'est jamais ouverte).

		if (!s_defaultFont)
		{
			s_defaultFont = Font::OpenFromMemory(r_sansationRegular, sizeof(r_sansationRegular));
			if (!s_defaultFont)
				NazaraError("failed to open default font");
		}

		return s_defaultFont;
	}

	unsigned int Font::GetDefaultGlyphBorder()
	{
		return s_defaultGlyphBorder;
	}

	unsigned int Font::GetDefaultMinimumStepSize()
	{
		return s_defaultMinimumStepSize;
	}

	std::shared_ptr<Font> Font::OpenFromFile(const std::filesystem::path& filePath, const FontParams& params)
	{
		TextRenderer* textRenderer = TextRenderer::Instance();
		NazaraAssert(textRenderer, "TextRenderer module has not been initialized");

		return textRenderer->GetFontLoader().LoadFromFile(filePath, params);
	}

	std::shared_ptr<Font> Font::OpenFromMemory(const void* data, std::size_t size, const FontParams& params)
	{
		TextRenderer* textRenderer = TextRenderer::Instance();
		NazaraAssert(textRenderer, "TextRenderer module has not been initialized");

		return textRenderer->GetFontLoader().LoadFromMemory(data, size, params);
	}

	std::shared_ptr<Font> Font::OpenFromStream(Stream& stream, const FontParams& params)
	{
		TextRenderer* textRenderer = TextRenderer::Instance();
		NazaraAssert(textRenderer, "TextRenderer module has not been initialized");

		return textRenderer->GetFontLoader().LoadFromStream(stream, params);
	}

	void Font::SetDefaultAtlas(std::shared_ptr<AbstractAtlas> atlas)
	{
		s_defaultAtlas = std::move(atlas);
	}

	void Font::SetDefaultGlyphBorder(unsigned int borderSize)
	{
		s_defaultGlyphBorder = borderSize;
	}

	void Font::SetDefaultMinimumStepSize(unsigned int minimumStepSize)
	{
		NazaraAssert(minimumStepSize, "minimum step size cannot be zero as it implies a division by zero");

		s_defaultMinimumStepSize = minimumStepSize;
	}

	UInt64 Font::ComputeKey(unsigned int characterSize, TextStyleFlags style, float outlineThickness) const
	{
		// Adjust size to step size
		UInt64 sizeStylePart = static_cast<UInt32>((characterSize/m_minimumStepSize)*m_minimumStepSize);
		sizeStylePart = std::min<UInt64>(sizeStylePart, Nz::IntegralPow(2, 30)); //< 2^30 should be more than enough as a max size
		sizeStylePart <<= 2;

		// Store bold and italic flags (other style are handled directly by a TextDrawer)
		if (style & TextStyle::Bold)
			sizeStylePart |= 1 << 0;

		if (style & TextStyle::Italic)
			sizeStylePart |= 1 << 1;

		return (sizeStylePart << 32) | reinterpret_cast<Nz::UInt32&>(outlineThickness);
	}

	void Font::OnAtlasCleared(const AbstractAtlas* atlas)
	{
		NazaraUnused(atlas);

		#ifdef NAZARA_DEBUG
		// Est-ce qu'il s'agit bien de notre atlas ?
		if (m_atlas.get() != atlas)
		{
			NazaraInternalError("Notified by a non-listening-to resource");
			return;
		}
		#endif

		// Notre atlas vient d'être vidé, détruisons le cache de glyphe
		m_glyphes.clear();

		OnFontGlyphCacheCleared(this);
	}

	void Font::OnAtlasLayerChange(const AbstractAtlas* atlas, AbstractImage* oldLayer, AbstractImage* newLayer)
	{
		NazaraUnused(atlas);
		NazaraUnused(oldLayer);
		NazaraUnused(newLayer);

		#ifdef NAZARA_DEBUG
		// Est-ce qu'il s'agit bien de notre atlas ?
		if (m_atlas.get() != atlas)
		{
			NazaraInternalError("Notified by a non-listening-to resource");
			return;
		}
		#endif

		// Pour faciliter le travail des ressources qui nous écoutent
		OnFontAtlasLayerChanged(this, oldLayer, newLayer);
	}

	const Font::Glyph& Font::PrecacheGlyph(GlyphMap& glyphMap, unsigned int characterSize, TextStyleFlags style, float outlineThickness, char32_t character) const
	{
		auto it = glyphMap.find(character);
		if (it != glyphMap.end())
			return it->second;

		Glyph& glyph = glyphMap[character]; //< Insert a new glyph
		glyph.valid = false;

		NazaraAssert(m_atlas, "font has no atlas");

		// Check if requested style is supported by our font (otherwise it will need to be simulated)
		glyph.fauxOutlineThickness = 0.f;
		glyph.requireFauxBold = false;
		glyph.requireFauxItalic = false;

		TextStyleFlags supportedStyle = style;
		if (style & TextStyle::Bold && !m_data->SupportsStyle(TextStyle::Bold))
		{
			glyph.requireFauxBold = true;
			supportedStyle &= ~TextStyle::Bold;
		}

		if (style & TextStyle::Italic && !m_data->SupportsStyle(TextStyle::Italic))
		{
			glyph.requireFauxItalic = true;
			supportedStyle &= ~TextStyle::Italic;
		}

		float supportedOutlineThickness = outlineThickness;
		if (outlineThickness > 0.f && !m_data->SupportsOutline(outlineThickness))
		{
			glyph.fauxOutlineThickness = supportedOutlineThickness;
			supportedOutlineThickness = 0.f;
		}

		// Does font support requested style?
		if (style == supportedStyle && outlineThickness == supportedOutlineThickness)
		{
			FontGlyph fontGlyph;
			if (ExtractGlyph(characterSize, character, style, outlineThickness, &fontGlyph))
			{
				if (fontGlyph.image.IsValid())
				{
					glyph.atlasRect.width = fontGlyph.image.GetWidth();
					glyph.atlasRect.height = fontGlyph.image.GetHeight();
				}
				else
				{
					glyph.atlasRect.width = 0;
					glyph.atlasRect.height = 0;
				}

				// Insert rectangle (if not empty) into our atlas
				if (glyph.atlasRect.width > 0 && glyph.atlasRect.height > 0)
				{
					// Add a small border to prevent GPU to sample another glyph pixel
					glyph.atlasRect.width += m_glyphBorder * 2;
					glyph.atlasRect.height += m_glyphBorder * 2;

					if (!m_atlas->Insert(fontGlyph.image, &glyph.atlasRect, &glyph.flipped, &glyph.layerIndex))
					{
						NazaraError("failed to insert glyph into atlas");
						return glyph;
					}

					// Recenter and remove glyph border
					glyph.atlasRect.x += m_glyphBorder;
					glyph.atlasRect.y += m_glyphBorder;
					glyph.atlasRect.width -= m_glyphBorder * 2;
					glyph.atlasRect.height -= m_glyphBorder * 2;
				}

				glyph.aabb = fontGlyph.aabb;
				glyph.advance = fontGlyph.advance;
				glyph.valid = true;
			}
			else
				NazaraWarningFmt("failed to extract glyph \"{0}\"", FromUtf32String(std::u32string_view(&character, 1)));
		}
		else
		{
			// Font doesn't support request style, precache the minimal supported version and copy its data
			UInt64 newKey = ComputeKey(characterSize, supportedStyle, supportedOutlineThickness);
			const Glyph& referenceGlyph = PrecacheGlyph(m_glyphes[newKey], characterSize, supportedStyle, supportedOutlineThickness, character);
			if (referenceGlyph.valid)
			{
				glyph.aabb = referenceGlyph.aabb;
				glyph.advance = referenceGlyph.advance;
				glyph.atlasRect = referenceGlyph.atlasRect;
				glyph.flipped = referenceGlyph.flipped;
				glyph.layerIndex = referenceGlyph.layerIndex;
				glyph.valid = true;
			}
		}

		return glyph;
	}

	bool Font::Initialize()
	{
		s_defaultAtlas = std::make_shared<GuillotineImageAtlas>();
		s_defaultGlyphBorder = 1;
		s_defaultMinimumStepSize = 1;

		return true;
	}

	void Font::Uninitialize()
	{
		s_defaultAtlas.reset();
		s_defaultFont.reset();
	}

	std::shared_ptr<AbstractAtlas> Font::s_defaultAtlas;
	std::shared_ptr<Font> Font::s_defaultFont;
	unsigned int Font::s_defaultGlyphBorder;
	unsigned int Font::s_defaultMinimumStepSize;
}
