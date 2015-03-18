// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Font.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/FontData.hpp>
#include <Nazara/Utility/FontGlyph.hpp>
#include <Nazara/Utility/GuillotineImageAtlas.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace
{
	const nzUInt8 r_cabinRegular[] = {
		#include <Nazara/Utility/Resources/Fonts/Cabin-Regular.ttf.h>
	};
}

bool NzFontParams::IsValid() const
{
	return true; // Rien à tester
}

NzFont::NzFont() :
m_atlas(s_defaultAtlas),
m_glyphBorder(s_defaultGlyphBorder),
m_minimumStepSize(s_defaultMinimumStepSize)
{
}

NzFont::~NzFont()
{
	Destroy();
	SetAtlas(nullptr); // On libère l'atlas proprement
}

void NzFont::ClearGlyphCache()
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
			NotifyModified(ModificationCode_GlyphCacheCleared);
		}
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

	return m_data->ExtractGlyph(characterSize, character, style, glyph);
}

const std::shared_ptr<NzAbstractAtlas>& NzFont::GetAtlas() const
{
	return m_atlas;
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

unsigned int NzFont::GetGlyphBorder() const
{
	return m_glyphBorder;
}

unsigned int NzFont::GetMinimumStepSize() const
{
	return m_minimumStepSize;
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

		NzFontGlyph glyph;
		if (m_data->ExtractGlyph(characterSize, ' ', nzTextStyle_Regular, &glyph))
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
	///TODO: Itération UTF-8 => UTF-32 sans allocation de buffer (Exposer utf8cpp ?)
	std::u32string set = characterSet.GetUtf32String();
	if (set.empty())
	{
		NazaraError("Invalid character set");
		return false;
	}

	nzUInt64 key = ComputeKey(characterSize, style);
	auto& glyphMap = m_glyphes[key];
	for (char32_t character : set)
		PrecacheGlyph(glyphMap, characterSize, style, character);

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

void NzFont::SetAtlas(const std::shared_ptr<NzAbstractAtlas>& atlas)
{
	if (m_atlas != atlas)
	{
		ClearGlyphCache();

		if (m_atlas)
			m_atlas->RemoveListener(this);

		m_atlas = atlas;
		if (m_atlas)
			m_atlas->AddListener(this);

		NotifyModified(ModificationCode_AtlasChanged);
	}
}

void NzFont::SetGlyphBorder(unsigned int borderSize)
{
	if (m_glyphBorder != borderSize)
	{
		m_glyphBorder = borderSize;
		ClearGlyphCache();
	}
}

void NzFont::SetMinimumStepSize(unsigned int minimumStepSize)
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

std::shared_ptr<NzAbstractAtlas> NzFont::GetDefaultAtlas()
{
	return s_defaultAtlas;
}

NzFont* NzFont::GetDefault()
{
	// Nous n'initialisons la police par défaut qu'à la demande pour qu'elle prenne
	// les paramètres par défaut (qui peuvent avoir étés changés par l'utilisateur),
	// et pour ne pas consommer de la mémoire vive inutilement (si elle n'est jamais utilisée, elle n'est jamais ouverte).

	if (!s_defaultFont)
	{
		NzFontRef cabin = NzFont::New();
		if (!cabin->OpenFromMemory(r_cabinRegular, sizeof(r_cabinRegular)))
		{
			NazaraError("Failed to open default font");
			return nullptr;
		}

		s_defaultFont = cabin;
	}

	return s_defaultFont;
}

unsigned int NzFont::GetDefaultGlyphBorder()
{
	return s_defaultGlyphBorder;
}

unsigned int NzFont::GetDefaultMinimumStepSize()
{
	return s_defaultMinimumStepSize;
}

void NzFont::SetDefaultAtlas(const std::shared_ptr<NzAbstractAtlas>& atlas)
{
	s_defaultAtlas = atlas;
}

void NzFont::SetDefaultGlyphBorder(unsigned int borderSize)
{
	s_defaultGlyphBorder = borderSize;
}

void NzFont::SetDefaultMinimumStepSize(unsigned int minimumStepSize)
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

nzUInt64 NzFont::ComputeKey(unsigned int characterSize, nzUInt32 style) const
{
	// On prend le pas en compte
	nzUInt64 sizePart = static_cast<nzUInt32>((characterSize/m_minimumStepSize)*m_minimumStepSize);

	// Ainsi que le style (uniquement le gras et l'italique, les autres sont gérés par un TextDrawer)
	nzUInt64 stylePart = 0;

	if (style & nzTextStyle_Bold)
		stylePart |= nzTextStyle_Bold;

	if (style & nzTextStyle_Italic)
		stylePart |= nzTextStyle_Italic;

	return (stylePart << 32) | sizePart;
}

bool NzFont::OnAtlasCleared(const NzAbstractAtlas* atlas, void* userdata)
{
	NazaraUnused(atlas);
	NazaraUnused(userdata);

	#ifdef NAZARA_DEBUG
	// Est-ce qu'il s'agit bien de notre atlas ?
	if (m_atlas.get() != atlas)
	{
		NazaraInternalError("Notified by a non-listening-to resource");
		return false; // On ne veut plus être notifié par cette ressource, évidemment
	}
	#endif

	// Notre atlas vient d'être vidé, détruisons le cache de glyphe
	m_glyphes.clear();
	NotifyModified(ModificationCode_GlyphCacheCleared);

	return true;
}

bool NzFont::OnAtlasLayerChange(const NzAbstractAtlas* atlas, NzAbstractImage* oldLayer, NzAbstractImage* newLayer, void* userdata)
{
	NazaraUnused(atlas);
	NazaraUnused(oldLayer);
	NazaraUnused(newLayer);
	NazaraUnused(userdata);

	#ifdef NAZARA_DEBUG
	// Est-ce qu'il s'agit bien de notre atlas ?
	if (m_atlas.get() != atlas)
	{
		NazaraInternalError("Notified by a non-listening-to resource");
		return false; // On ne veut plus être notifié par cette ressource, évidemment
	}
	#endif

	// Pour faciliter le travail des ressources qui nous écoutent
	NotifyModified(ModificationCode_AtlasLayerChanged);

	return true;
}

void NzFont::OnAtlasReleased(const NzAbstractAtlas* atlas, void* userdata)
{
	NazaraUnused(atlas);
	NazaraUnused(userdata);

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

const NzFont::Glyph& NzFont::PrecacheGlyph(GlyphMap& glyphMap, unsigned int characterSize, nzUInt32 style, char32_t character) const
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

	nzUInt32 supportedStyle = style;
	if (style & nzTextStyle_Bold && !m_data->SupportsStyle(nzTextStyle_Bold))
	{
		glyph.requireFauxBold = true;
		supportedStyle &= ~nzTextStyle_Bold;
	}

	if (style & nzTextStyle_Italic && !m_data->SupportsStyle(nzTextStyle_Italic))
	{
		glyph.requireFauxItalic = true;
		supportedStyle &= ~nzTextStyle_Italic;
	}

	// Est-ce que la police supporte le style demandé ?
	if (style == supportedStyle)
	{
		// On extrait le glyphe depuis la police
		NzFontGlyph fontGlyph;
		if (ExtractGlyph(characterSize, character, style, &fontGlyph))
		{
			glyph.atlasRect.width = fontGlyph.image.GetWidth();
			glyph.atlasRect.height = fontGlyph.image.GetHeight();

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
			NazaraWarning("Failed to extract glyph \"" + NzString::Unicode(character) + "\"");
		}
	}
	else
	{
		// La police ne supporte pas le style demandé, nous allons donc précharger le glyphe supportant le style "minimum" supporté
		// et copier ses données
		nzUInt64 newKey = ComputeKey(characterSize, supportedStyle);
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

bool NzFont::Initialize()
{
	if (!NzFontLibrary::Initialize())
	{
		NazaraError("Failed to initialise library");
		return false;
	}

	s_defaultAtlas.reset(new NzGuillotineImageAtlas);
	s_defaultGlyphBorder = 1;
	s_defaultMinimumStepSize = 1;

	return true;
}

void NzFont::Uninitialize()
{
	s_defaultAtlas.reset();
	s_defaultFont.Reset();
	NzFontLibrary::Uninitialize();
}

std::shared_ptr<NzAbstractAtlas> NzFont::s_defaultAtlas;
NzFontRef NzFont::s_defaultFont;
NzFontLibrary::LibraryMap NzFont::s_library;
NzFontLoader::LoaderList NzFont::s_loaders;
unsigned int NzFont::s_defaultGlyphBorder;
unsigned int NzFont::s_defaultMinimumStepSize;
