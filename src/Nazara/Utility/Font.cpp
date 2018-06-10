// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Font.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/FontData.hpp>
#include <Nazara/Utility/FontGlyph.hpp>
#include <Nazara/Utility/GuillotineImageAtlas.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	namespace
	{
		const UInt8 r_cabinRegular[] = {
			#include <Nazara/Utility/Resources/Fonts/Cabin-Regular.ttf.h>
		};
	}

	bool FontParams::IsValid() const
	{
		return true; // Rien à tester
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
		SetAtlas(nullptr); // On libère l'atlas proprement
	}

	void Font::ClearGlyphCache()
	{
		if (m_atlas)
		{
			if (m_atlas.unique())
				m_atlas->Clear(); // Appellera OnAtlasCleared
			else
			{
				// Au moins une autre police utilise cet atlas, on vire nos glyphes un par un
				for (auto mapIt = m_glyphes.begin(); mapIt != m_glyphes.end(); ++mapIt)
				{
					GlyphMap& glyphMap = mapIt->second;
					for (auto glyphIt = glyphMap.begin(); glyphIt != glyphMap.end(); ++glyphIt)
					{
						Glyph& glyph = glyphIt->second;
						m_atlas->Free(&glyph.atlasRect, &glyph.layerIndex, 1);
					}
				}

				// Destruction des glyphes mémorisés et notification
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

	bool Font::Create(FontData* data)
	{
		Destroy();

		#if NAZARA_UTILITY_SAFE
		if (!data)
		{
			NazaraError("Invalid font data");
			return false;
		}
		#endif

		m_data.reset(data);
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

	bool Font::ExtractGlyph(unsigned int characterSize, char32_t character, UInt32 style, FontGlyph* glyph) const
	{
		#if NAZARA_UTILITY_SAFE
		if (!IsValid())
		{
			NazaraError("Invalid font");
			return false;
		}
		#endif

		return m_data->ExtractGlyph(characterSize, character, style, glyph);
	}

	const std::shared_ptr<AbstractAtlas>& Font::GetAtlas() const
	{
		return m_atlas;
	}

	std::size_t Font::GetCachedGlyphCount(unsigned int characterSize, UInt32 style) const
	{
		UInt64 key = ComputeKey(characterSize, style);
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

	String Font::GetFamilyName() const
	{
		#if NAZARA_UTILITY_SAFE
		if (!IsValid())
		{
			NazaraError("Invalid font");
			return String("Invalid font");
		}
		#endif

		return m_data->GetFamilyName();
	}

	int Font::GetKerning(unsigned int characterSize, char32_t first, char32_t second) const
	{
		#if NAZARA_UTILITY_SAFE
		if (!IsValid())
		{
			NazaraError("Invalid font");
			return 0;
		}
		#endif

		// On utilise un cache car la méthode interne QueryKerning peut se révéler coûteuse (car pouvant induire un changement de taille)
		auto& map = m_kerningCache[characterSize];

		UInt64 key = (static_cast<UInt64>(first) << 32) | second; // Combinaison de deux caractères 32 bits dans un nombre 64 bits

		auto it = map.find(key);
		if (it == map.end())
		{
			// Absent du cache: on va demander l'information à la police
			int kerning = m_data->QueryKerning(characterSize, first, second);
			map.insert(std::make_pair(key, kerning));

			return kerning;
		}
		else
			return it->second; // Présent dans le cache, tout va bien
	}

	const Font::Glyph& Font::GetGlyph(unsigned int characterSize, UInt32 style, char32_t character) const
	{
		UInt64 key = ComputeKey(characterSize, style);
		return PrecacheGlyph(m_glyphes[key], characterSize, style, character);
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
		#if NAZARA_UTILITY_SAFE
		if (!IsValid())
		{
			NazaraError("Invalid font");

			static SizeInfo dummy;
			return dummy;
		}
		#endif

		auto it = m_sizeInfoCache.find(characterSize);
		if (it == m_sizeInfoCache.end())
		{
			SizeInfo sizeInfo;
			sizeInfo.lineHeight = m_data->QueryLineHeight(characterSize);
			sizeInfo.underlinePosition = m_data->QueryUnderlinePosition(characterSize);
			sizeInfo.underlineThickness = m_data->QueryUnderlineThickness(characterSize);

			FontGlyph glyph;
			if (m_data->ExtractGlyph(characterSize, ' ', TextStyle_Regular, &glyph))
				sizeInfo.spaceAdvance = glyph.advance;
			else
			{
				NazaraWarning("Failed to extract space character from font, using half the size");
				sizeInfo.spaceAdvance = characterSize/2;
			}

			it = m_sizeInfoCache.insert(std::make_pair(characterSize, sizeInfo)).first;
		}

		return it->second;
	}

	String Font::GetStyleName() const
	{
		#if NAZARA_UTILITY_SAFE
		if (!IsValid())
		{
			NazaraError("Invalid font");
			return String("Invalid font");
		}
		#endif

		return m_data->GetStyleName();
	}

	bool Font::IsValid() const
	{
		return m_data != nullptr;
	}

	bool Font::Precache(unsigned int characterSize, UInt32 style, char32_t character) const
	{
		UInt64 key = ComputeKey(characterSize, style);
		return PrecacheGlyph(m_glyphes[key], characterSize, style, character).valid;
	}

	bool Font::Precache(unsigned int characterSize, UInt32 style, const String& characterSet) const
	{
		///TODO: Itération UTF-8 => UTF-32 sans allocation de buffer (Exposer utf8cpp ?)
		std::u32string set = characterSet.GetUtf32String();
		if (set.empty())
		{
			NazaraError("Invalid character set");
			return false;
		}

		UInt64 key = ComputeKey(characterSize, style);
		auto& glyphMap = m_glyphes[key];
		for (char32_t character : set)
			PrecacheGlyph(glyphMap, characterSize, style, character);

		return true;
	}

	bool Font::OpenFromFile(const String& filePath, const FontParams& params)
	{
		return FontLoader::LoadFromFile(this, filePath, params);
	}

	bool Font::OpenFromMemory(const void* data, std::size_t size, const FontParams& params)
	{
		return FontLoader::LoadFromMemory(this, data, size, params);
	}

	bool Font::OpenFromStream(Stream& stream, const FontParams& params)
	{
		return FontLoader::LoadFromStream(this, stream, params);
	}

	void Font::SetAtlas(const std::shared_ptr<AbstractAtlas>& atlas)
	{
		if (m_atlas != atlas)
		{
			ClearGlyphCache();

			m_atlas = atlas;
			if (m_atlas)
			{
				m_atlasClearedSlot.Connect(m_atlas->OnAtlasCleared, this, &Font::OnAtlasCleared);
				m_atlasLayerChangeSlot.Connect(m_atlas->OnAtlasLayerChange, this, &Font::OnAtlasLayerChange);
				m_atlasReleaseSlot.Connect(m_atlas->OnAtlasRelease, this, &Font::OnAtlasRelease);
			}
			else
			{
				m_atlasClearedSlot.Disconnect();
				m_atlasLayerChangeSlot.Disconnect();
				m_atlasReleaseSlot.Disconnect();
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
			#if NAZARA_UTILITY_SAFE
			if (minimumStepSize == 0)
			{
				NazaraError("Minimum step size cannot be zero as it implies division by zero");
				return;
			}
			#endif

			m_minimumStepSize = minimumStepSize;
			ClearGlyphCache();
		}
	}

	std::shared_ptr<AbstractAtlas> Font::GetDefaultAtlas()
	{
		return s_defaultAtlas;
	}

	const FontRef& Font::GetDefault()
	{
		// Nous n'initialisons la police par défaut qu'à la demande pour qu'elle prenne
		// les paramètres par défaut (qui peuvent avoir étés changés par l'utilisateur),
		// et pour ne pas consommer de la mémoire vive inutilement (si elle n'est jamais utilisée, elle n'est jamais ouverte).

		if (!s_defaultFont)
		{
			FontRef cabin = Font::New();
			if (cabin->OpenFromMemory(r_cabinRegular, sizeof(r_cabinRegular)))
                s_defaultFont = cabin;
			else
				NazaraError("Failed to open default font");
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

	void Font::SetDefaultAtlas(const std::shared_ptr<AbstractAtlas>& atlas)
	{
		s_defaultAtlas = atlas;
	}

	void Font::SetDefaultGlyphBorder(unsigned int borderSize)
	{
		s_defaultGlyphBorder = borderSize;
	}

	void Font::SetDefaultMinimumStepSize(unsigned int minimumStepSize)
	{
		#if NAZARA_UTILITY_SAFE
		if (minimumStepSize == 0)
		{
			NazaraError("Minimum step size cannot be zero as it implies division by zero");
			return;
		}
		#endif

		s_defaultMinimumStepSize = minimumStepSize;
	}

	UInt64 Font::ComputeKey(unsigned int characterSize, UInt32 style) const
	{
		// On prend le pas en compte
		UInt64 sizePart = static_cast<UInt32>((characterSize/m_minimumStepSize)*m_minimumStepSize);

		// Ainsi que le style (uniquement le gras et l'italique, les autres sont gérés par un TextDrawer)
		UInt64 stylePart = 0;

		if (style & TextStyle_Bold)
			stylePart |= TextStyle_Bold;

		if (style & TextStyle_Italic)
			stylePart |= TextStyle_Italic;

		return (stylePart << 32) | sizePart;
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

	void Font::OnAtlasRelease(const AbstractAtlas* atlas)
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

		// Nous ne pouvons pas faire grand chose d'autre que de balancer une erreur à la tête de l'utilisateur avant un potentiel crash...
		NazaraError("Atlas has been released while in use");
	}

	const Font::Glyph& Font::PrecacheGlyph(GlyphMap& glyphMap, unsigned int characterSize, UInt32 style, char32_t character) const
	{
		auto it = glyphMap.find(character);
		if (it != glyphMap.end()) // Si le glyphe n'est pas déjà chargé
			return it->second;

		Glyph& glyph = glyphMap[character]; // Insertion du glyphe
		glyph.valid = false;

		#if NAZARA_UTILITY_SAFE
		if (!m_atlas)
		{
			NazaraError("Font has no atlas");
			return glyph;
		}
		#endif

		// On vérifie que le style demandé est supporté par la police (dans le cas contraire il devra être simulé au rendu)
		glyph.requireFauxBold = false;
		glyph.requireFauxItalic = false;

		UInt32 supportedStyle = style;
		if (style & TextStyle_Bold && !m_data->SupportsStyle(TextStyle_Bold))
		{
			glyph.requireFauxBold = true;
			supportedStyle &= ~TextStyle_Bold;
		}

		if (style & TextStyle_Italic && !m_data->SupportsStyle(TextStyle_Italic))
		{
			glyph.requireFauxItalic = true;
			supportedStyle &= ~TextStyle_Italic;
		}

		// Est-ce que la police supporte le style demandé ?
		if (style == supportedStyle)
		{
			// On extrait le glyphe depuis la police
			FontGlyph fontGlyph;
			if (ExtractGlyph(characterSize, character, style, &fontGlyph))
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

				// Insertion du rectangle dans l'un des atlas
				if (glyph.atlasRect.width > 0 && glyph.atlasRect.height > 0) // Si l'image contient quelque chose
				{
					// Bordure (pour éviter le débordement lors du filtrage)
					glyph.atlasRect.width += m_glyphBorder*2;
					glyph.atlasRect.height += m_glyphBorder*2;

					// Insertion du rectangle dans l'atlas virtuel
					if (!m_atlas->Insert(fontGlyph.image, &glyph.atlasRect, &glyph.flipped, &glyph.layerIndex))
					{
						NazaraError("Failed to insert glyph into atlas");
						return glyph;
					}

					// Compensation de la bordure (centrage du glyphe)
					glyph.atlasRect.x += m_glyphBorder;
					glyph.atlasRect.y += m_glyphBorder;
					glyph.atlasRect.width -= m_glyphBorder*2;
					glyph.atlasRect.height -= m_glyphBorder*2;
				}

				glyph.aabb = fontGlyph.aabb;
				glyph.advance = fontGlyph.advance;
				glyph.valid = true;
			}
			else
			{
				NazaraWarning("Failed to extract glyph \"" + String::Unicode(character) + "\"");
			}
		}
		else
		{
			// La police ne supporte pas le style demandé, nous allons donc précharger le glyphe supportant le style "minimum" supporté
			// et copier ses données
			UInt64 newKey = ComputeKey(characterSize, supportedStyle);
			const Glyph& referenceGlyph = PrecacheGlyph(m_glyphes[newKey], characterSize, supportedStyle, character);
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
		if (!FontLibrary::Initialize())
		{
			NazaraError("Failed to initialise library");
			return false;
		}

		s_defaultAtlas.reset(new GuillotineImageAtlas);
		s_defaultGlyphBorder = 1;
		s_defaultMinimumStepSize = 1;

		return true;
	}

	void Font::Uninitialize()
	{
		s_defaultAtlas.reset();
		s_defaultFont.Reset();
		FontLibrary::Uninitialize();
	}

	std::shared_ptr<AbstractAtlas> Font::s_defaultAtlas;
	FontRef Font::s_defaultFont;
	FontLibrary::LibraryMap Font::s_library;
	FontLoader::LoaderList Font::s_loaders;
	unsigned int Font::s_defaultGlyphBorder;
unsigned int Font::s_defaultMinimumStepSize;
}
