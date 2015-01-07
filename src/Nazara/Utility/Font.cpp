// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Font.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/FontData.hpp>
#include <Nazara/Utility/FontGlyph.hpp>
#include <Nazara/Utility/Debug.hpp>

bool NzFontParams::IsValid() const
{
	return true; // Rien à tester
}

NzFont::NzFont() :
m_minimumSizeStep(1)
{
}

NzFont::~NzFont()
{
	Destroy();
}

void NzFont::ClearGlyphCache()
{
	if (m_atlas)
	{
		if (m_atlas.use_count() > 1) // Au moins une autre police utilise cet atlas, on vire nos glyphes
		{
			for (auto mapIt = m_glyphes.begin(); mapIt != m_glyphes.end(); ++mapIt)
			{
				GlyphMap& glyphMap = mapIt->second;
				for (auto glyphIt = glyphMap.begin(); glyphIt != glyphMap.end(); ++glyphIt)
				{
					Glyph& glyph = glyphIt->second;
					m_atlas->Free(&glyph.atlasRect, &glyph.layerIndex, 1);
				}
			}

			// Destruction des glyphes mémorisés
			m_glyphes.clear();
			NotifyModified(ModificationCode_GlyphCacheCleared);
		}
		else
			m_atlas->Clear();
	}
}

void NzFont::ClearKerningCache()
{
	m_kerningCache.clear();
	NotifyModified(ModificationCode_KerningCacheCleared);
}

void NzFont::ClearSizeInfoCache()
{
	m_sizeInfoCache.clear();
	NotifyModified(ModificationCode_SizeInfoCacheCleared);
}

bool NzFont::Create(NzFontData* data)
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

void NzFont::Destroy()
{
	ClearGlyphCache();

	m_data.reset();
	m_kerningCache.clear();
	m_sizeInfoCache.clear();
}

bool NzFont::ExtractGlyph(unsigned int characterSize, char32_t character, nzUInt32 style, NzFontGlyph* glyph) const
{
	#if NAZARA_UTILITY_SAFE
	if (!IsValid())
	{
		NazaraError("Invalid font");
		return false;
	}
	#endif

	return m_data->ExtractGlyph(characterSize, character, style & nzTextStyle_Bold, glyph);
}

const NzAbstractFontAtlas* NzFont::GetAtlas() const
{
	return m_atlas.get();
}

unsigned int NzFont::GetCachedGlyphCount(unsigned int characterSize, nzUInt32 style) const
{
	nzUInt64 key = ComputeKey(characterSize, style);
	auto it = m_glyphes.find(key);
	if (it == m_glyphes.end())
		return 0;

	return it->second.size();
}

unsigned int NzFont::GetCachedGlyphCount() const
{
	unsigned int count = 0;
	for (auto& pair : m_glyphes)
		count += pair.second.size();

	return count;
}

NzString NzFont::GetFamilyName() const
{
	#if NAZARA_UTILITY_SAFE
	if (!IsValid())
	{
		NazaraError("Invalid font");
		return NzString("Invalid font");
	}
	#endif

	return m_data->GetFamilyName();
}

int NzFont::GetKerning(unsigned int characterSize, char32_t first, char32_t second) const
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

	nzUInt64 key = (static_cast<nzUInt64>(first) << 32) | second; // Combinaison de deux caractères 32 bits dans un nombre 64 bits

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

const NzFont::Glyph& NzFont::GetGlyph(unsigned int characterSize, nzUInt32 style, char32_t character) const
{
	nzUInt64 key = ComputeKey(characterSize, style);
	return PrecacheGlyph(m_glyphes[key], characterSize, style, character);
}

unsigned int NzFont::GetMinimumStepSize() const
{
	return m_minimumSizeStep;
}

const NzFont::SizeInfo& NzFont::GetSizeInfo(unsigned int characterSize) const
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

		it = m_sizeInfoCache.insert(std::make_pair(characterSize, sizeInfo)).first;
	}

	return it->second;
}

NzString NzFont::GetStyleName() const
{
	#if NAZARA_UTILITY_SAFE
	if (!IsValid())
	{
		NazaraError("Invalid font");
		return NzString("Invalid font");
	}
	#endif

	return m_data->GetStyleName();
}

bool NzFont::IsValid() const
{
	return m_data != nullptr;
}

bool NzFont::Precache(unsigned int characterSize, nzUInt32 style, char32_t character) const
{
	nzUInt64 key = ComputeKey(characterSize, style);
	return PrecacheGlyph(m_glyphes[key], characterSize, style, character).valid;
}

bool NzFont::Precache(unsigned int characterSize, nzUInt32 style, const NzString& characterSet) const
{
	unsigned int size;
	std::unique_ptr<char32_t[]> characters(characterSet.GetUtf32Buffer(&size));
	if (!characters)
	{
		NazaraError("Invalid character set");
		return false;
	}

	nzUInt64 key = ComputeKey(characterSize, style);
	auto& glyphMap = m_glyphes[key];
	for (unsigned int i = 0; i < size; ++i)
		PrecacheGlyph(glyphMap, characterSize, style, characters[i]);

	return true;
}

bool NzFont::OpenFromFile(const NzString& filePath, const NzFontParams& params)
{
	return NzFontLoader::LoadFromFile(this, filePath, params);
}

bool NzFont::OpenFromMemory(const void* data, std::size_t size, const NzFontParams& params)
{
	return NzFontLoader::LoadFromMemory(this, data, size, params);
}

bool NzFont::OpenFromStream(NzInputStream& stream, const NzFontParams& params)
{
	return NzFontLoader::LoadFromStream(this, stream, params);
}

void NzFont::SetAtlas(std::shared_ptr<NzAbstractFontAtlas> atlas)
{
	ClearGlyphCache();

	m_atlas = atlas;
}

void NzFont::SetMinimumStepSize(unsigned int minimumStepSize)
{
	#if NAZARA_UTILITY_SAFE
	if (minimumStepSize == 0)
	{
		NazaraError("Minimum step size cannot be zero as it implies division by zero");
		return;
	}
	#endif

	m_minimumSizeStep = minimumStepSize;
	ClearGlyphCache();
}

nzUInt64 NzFont::ComputeKey(unsigned int characterSize, nzUInt32 style) const
{
	nzUInt64 sizePart = static_cast<nzUInt32>((characterSize/m_minimumSizeStep)*m_minimumSizeStep);
	nzUInt64 stylePart = 0;

	if (style & nzTextStyle_Bold) // Les caractères gras sont générés différemment
		stylePart |= nzTextStyle_Bold;

	// Les caractères italiques peuvent venir d'une autre police, dans le cas contraire ils sont générés au runtime
	//if (style & nzTextStyle_Italic)
	//	stylePart |= nzTextStyle_Italic;

	return (stylePart << 32) | sizePart;
}

void NzFont::OnAtlasCleared()
{
	// Notre atlas vient d'être vidé, détruisons le cache de glyphe
	m_glyphes.clear();

	NotifyModified(ModificationCode_GlyphCacheCleared);
}

const NzFont::Glyph& NzFont::PrecacheGlyph(GlyphMap& glyphMap, unsigned int characterSize, bool bold, char32_t character) const
{
	auto it = glyphMap.find(character);
	if (it != glyphMap.end()) // Si le glyphe n'est pas déjà chargé
		return it->second;

	Glyph& glyph = glyphMap[character]; // Insertion du glyphe
	glyph.valid = false;

	// On extrait le glyphe depuis la police
	NzFontGlyph fontGlyph;
	if (ExtractGlyph(characterSize, character, bold, &fontGlyph))
	{
		glyph.atlasRect.width = fontGlyph.image.GetWidth();
		glyph.atlasRect.height = fontGlyph.image.GetHeight();

		// Insertion du rectangle dans l'un des atlas
		if (glyph.atlasRect.width > 0 && glyph.atlasRect.height > 0) // Si l'image contient quelque chose
		{
			// Padding (pour éviter le débordement lors du filtrage)
			const unsigned int padding = 1; // Un pixel de contour

			glyph.atlasRect.width += padding*2;
			glyph.atlasRect.height += padding*2;

			// Insertion du rectangle dans l'atlas virtuel
			if (!m_atlas->Insert(fontGlyph.image, &glyph.atlasRect, &glyph.flipped, &glyph.layerIndex))
			{
				NazaraError("Failed to insert glyph into atlas");
				return glyph;
			}

			// Compensation du contour (centrage du glyphe)
			glyph.atlasRect.x += padding;
			glyph.atlasRect.y += padding;
			glyph.atlasRect.width -= padding*2;
			glyph.atlasRect.height -= padding*2;
		}

		glyph.aabb = fontGlyph.aabb;
		glyph.advance = fontGlyph.advance;
		glyph.valid = true;
	}
	else
	{
		NazaraWarning("Failed to extract glyph \"" + NzString::Unicode(character) + "\"");
	}

	return glyph;
}

NzFontLoader::LoaderList NzFont::s_loaders;
