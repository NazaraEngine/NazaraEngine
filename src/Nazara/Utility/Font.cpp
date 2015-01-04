// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Font.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/FontData.hpp>
#include <Nazara/Utility/FontGlyph.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace
{
	const unsigned int s_atlasStartSize = 512;
}

bool NzFontParams::IsValid() const
{
	return true; // Rien à tester
}

NzFont::NzFont() :
m_maxAtlasSize(0),
m_minimumSizeStep(1)
{
}

NzFont::~NzFont()
{
	Destroy();
}

void NzFont::ClearGlyphCache()
{
	// Destruction des atlas et glyphes mémorisés
	m_atlases.clear();
	m_glyphes.clear();
	m_glyphQueue.clear();

	// Création du premier atlas
	m_atlases.resize(1);
	Atlas& atlas = m_atlases.back();
	atlas.binPack.Reset(s_atlasStartSize, s_atlasStartSize);
}

void NzFont::ClearKerningCache()
{
	m_kerningCache.clear();
}

void NzFont::ClearSizeInfoCache()
{
	m_sizeInfoCache.clear();
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

	ClearGlyphCache(); // Création du premier atlas en mémoire

	return true;
}

void NzFont::Destroy()
{
	m_atlases.clear();
	m_data.reset();
	m_kerningCache.clear();
	m_glyphes.clear();
	m_glyphQueue.clear();
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

const NzFont::Atlas& NzFont::GetAtlas(unsigned int atlasIndex) const
{
	if (!m_glyphQueue.empty())
		ProcessGlyphQueue();

	return m_atlases.at(atlasIndex);
}

unsigned int NzFont::GetAtlasCount() const
{
	return m_atlases.size();
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

unsigned int NzFont::GetMaxAtlasSize() const
{
	return m_maxAtlasSize;
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

void NzFont::SetMaxAtlasSize(unsigned int maxAtlasSize)
{
	unsigned int oldMaxAtlasSize = GetRealMaxAtlasSize();
	m_maxAtlasSize = maxAtlasSize;

	// Si l'un de nos atlas dépasse la nouvelle taille, on doit vider le cache
	maxAtlasSize = GetRealMaxAtlasSize();
	if (maxAtlasSize < oldMaxAtlasSize)
	{
		for (Atlas& atlas : m_atlases)
		{
			unsigned int atlasSize = atlas.binPack.GetWidth();
			if (atlasSize > maxAtlasSize)
			{
				NazaraWarning("At least one atlas was over new max atlas size (" + NzString::Number(atlasSize) + " > " + NzString::Number(maxAtlasSize) + "), clearing glyph cache...");
				ClearGlyphCache();
				return;
			}
		}
	}
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
}

unsigned int NzFont::GetDefaultMaxAtlasSize()
{
	return s_maxAtlasSize;
}

void NzFont::SetDefaultMaxAtlasSize(unsigned int maxAtlasSize)
{
	s_maxAtlasSize = maxAtlasSize;
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

unsigned int NzFont::GetRealMaxAtlasSize() const
{
	unsigned int maxAtlasSize = (m_maxAtlasSize == 0) ? s_maxAtlasSize : m_maxAtlasSize;
	if (maxAtlasSize == 0)
		maxAtlasSize = std::numeric_limits<unsigned int>::max();

	return maxAtlasSize;
}

unsigned int NzFont::InsertRect(NzRectui* rect, bool* flipped) const
{
	///DOC: Tous les pointeurs doivent être valides
	// Précondition: Un rectangle ne peut pas être plus grand dans une dimension que la taille maximale de l'atlas

	unsigned int maxAtlasSize = GetRealMaxAtlasSize();

	// Cette fonction ne fait qu'insérer un rectangle de façon virtuelle, l'insertion des images se fait après
	for (unsigned int i = 0; i < m_atlases.size(); ++i)
	{
		Atlas& atlas = m_atlases[i];
		if (atlas.binPack.Insert(rect, flipped, 1, false, NzGuillotineBinPack::RectBestAreaFit, NzGuillotineBinPack::SplitMinimizeArea))
			// Insertion réussie dans l'un des atlas, pas de question à se poser
			return i;
		else if (i == m_atlases.size() - 1) // Dernière itération ?
		{
			// Dernier atlas, et le glyphe ne rentre pas, peut-on agrandir la taille de l'atlas ?
			unsigned int size = atlas.binPack.GetWidth(); // l'atlas étant carré, on ne teste qu'une dimension
			if (size < maxAtlasSize)
			{
				// On peut encore agrandir l'atlas
				size = std::min(size*2, maxAtlasSize);
				atlas.binPack.Expand(size, size);

				// On relance la boucle sur le dernier atlas
				i--;
			}
			else
			{
				// On ne peut plus agrandir le dernier atlas, il est temps d'en créer un nouveau
				m_atlases.resize(m_atlases.size() + 1);
				Atlas& newAtlas = m_atlases.back();

				newAtlas.binPack.Reset(s_atlasStartSize, s_atlasStartSize);

				// On laisse la boucle insérer toute seule le rectangle à la prochaine itération
			}
		}
	}

	// Si nous arrivons ici, c'est qu'une erreur a eu lieu en amont
	NazaraInternalError("This shouldn't happen");
	return std::numeric_limits<unsigned int>::max();
}

const NzFont::Glyph& NzFont::PrecacheGlyph(GlyphMap& glyphMap, unsigned int characterSize, bool bold, char32_t character) const
{
	auto it = glyphMap.find(character);
	if (it != glyphMap.end()) // Si le glyphe n'est pas déjà chargé
		return it->second;

	Glyph glyph;
	glyph.valid = false;

	// On extrait le glyphe depuis la police
	NzFontGlyph fontGlyph;
	if (ExtractGlyph(characterSize, character, bold, &fontGlyph))
	{
		glyph.atlasRect.width = fontGlyph.image.GetWidth();
		glyph.atlasRect.height = fontGlyph.image.GetHeight();

		unsigned int maxAtlasSize = GetRealMaxAtlasSize();
		if (glyph.atlasRect.width <= maxAtlasSize && glyph.atlasRect.height <= maxAtlasSize)
		{
			// Insertion du rectangle dans l'un des atlas
			glyph.aabb = fontGlyph.aabb;
			glyph.advance = fontGlyph.advance;
			glyph.valid = true;

			if (glyph.atlasRect.width > 0 && glyph.atlasRect.height > 0) // Si l'image contient quelque chose
			{
				// Padding (pour éviter le débordement lors du filtrage)
				const unsigned int padding = 1; // Un pixel entre chaque glyphe

				glyph.atlasRect.width += padding;
				glyph.atlasRect.height += padding;

				// Insertion du rectangle dans l'atlas virtuel
				glyph.atlasIndex = InsertRect(&glyph.atlasRect, &glyph.flipped);

				glyph.atlasRect.width -= padding;
				glyph.atlasRect.height -= padding;

				// Mise en queue pour insertion dans l'atlas réel
				m_glyphQueue.resize(m_glyphQueue.size()+1);
				QueuedGlyph& queuedGlyph = m_glyphQueue.back();
				queuedGlyph.codepoint = character;
				queuedGlyph.image = std::move(fontGlyph.image);
				queuedGlyph.map = &glyphMap;
			}
		}
		else
		{
			NazaraWarning("Glyph \"" + NzString::Unicode(character) + "\" is bigger than max atlas size");
		}
	}
	else
	{
		NazaraWarning("Failed to extract glyph \"" + NzString::Unicode(character) + "\"");
	}

	return glyphMap.insert(std::make_pair(character, std::move(glyph))).first->second;
}

void NzFont::ProcessGlyphQueue() const
{
	for (QueuedGlyph& queuedGlyph : m_glyphQueue)
	{
		GlyphMap& glyphMap = *queuedGlyph.map;
		auto glyphIt = glyphMap.find(queuedGlyph.codepoint);
		if (glyphIt == glyphMap.end())
			continue; // Le glyphe a certainement été supprimé du cache avant la mise à jour de l'atlas

		Glyph& glyph = glyphIt->second;
		Atlas& atlas = m_atlases[glyph.atlasIndex];

		// On s'assure que l'atlas est de la bonne taille
		NzVector2ui atlasSize(atlas.image.GetWidth(), atlas.image.GetHeight());
		NzVector2ui binPackSize = atlas.binPack.GetSize();
		if (atlasSize != binPackSize)
		{
			// Création d'une nouvelle image
			NzImage newAtlas(nzImageType_2D, nzPixelFormat_A8, binPackSize.x, binPackSize.y);
			if (atlas.image.IsValid())
			{
				newAtlas.Copy(atlas.image, NzRectui(atlasSize), NzVector2ui(0, 0)); // On copie les anciennes données

				// On initialise les nouvelles régions
				newAtlas.Fill(NzColor(255, 255, 255, 0), NzRectui(0, atlasSize.y, binPackSize.x, binPackSize.y - atlasSize.y));
				newAtlas.Fill(NzColor(255, 255, 255, 0), NzRectui(atlasSize.x, 0, binPackSize.x - atlasSize.x, atlasSize.y));
			}
			else
				newAtlas.Fill(NzColor(255, 255, 255, 0)); // On initialise les pixels

			atlas.image = std::move(newAtlas); // On déplace la nouvelle image vers l'atlas
		}

		unsigned int glyphWidth = queuedGlyph.image.GetWidth();
		unsigned int glyphHeight = queuedGlyph.image.GetHeight();

		// On copie le glyphe dans l'atlas
		if (glyph.flipped)
		{
			// On tourne le glyphe pour qu'il rentre dans le rectangle
			const nzUInt8* src = queuedGlyph.image.GetConstPixels();
			nzUInt8* ptr = atlas.image.GetPixels(glyph.atlasRect.x, glyph.atlasRect.y + glyphWidth - 1);

			unsigned int lineStride = atlas.image.GetWidth(); // BPP = 1
			for (unsigned int y = 0; y < glyphHeight; ++y)
			{
				for (unsigned int x = 0; x < glyphWidth; ++x)
				{
					*ptr = *src++; // On copie et on avance dans le glyphe
					ptr -= lineStride; // On remonte d'une ligne
				}

				ptr += lineStride*glyphWidth + 1;
			}
		}
		else
			atlas.image.Copy(queuedGlyph.image, NzRectui(glyphWidth, glyphHeight), glyph.atlasRect.GetPosition());

		queuedGlyph.image.Destroy(); // On libère l'image dès que possible (pour réduire la consommation)
	}

	m_glyphQueue.clear();
}

NzFontLoader::LoaderList NzFont::s_loaders;
unsigned int NzFont::s_maxAtlasSize = 8192; // Valeur totalement arbitraire
