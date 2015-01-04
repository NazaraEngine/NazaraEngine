// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FONT_HPP
#define NAZARA_FONT_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/GuillotineBinPack.hpp>
#include <Nazara/Core/NonCopyable.hpp>
#include <Nazara/Core/ResourceRef.hpp>
#include <Nazara/Core/ResourceLoader.hpp>
#include <Nazara/Utility/Image.hpp>
#include <memory>
#include <unordered_map>

struct NAZARA_API NzFontParams
{
	bool IsValid() const;
};

class NzFont;
class NzFontData;

struct NzFontGlyph; // TEMP

using NzFontConstRef = NzResourceRef<const NzFont>;
using NzFontLoader = NzResourceLoader<NzFont, NzFontParams>;
using NzFontRef = NzResourceRef<NzFont>;

class NAZARA_API NzFont : public NzResource, NzNonCopyable
{
	friend NzFontLoader;

	public:
		struct Atlas
		{
			NzGuillotineBinPack binPack;
			NzImage image;
		};

		struct Glyph
		{
			NzRecti aabb;
			NzRectui atlasRect;
			bool flipped;
			bool valid;
			int advance;
			unsigned int atlasIndex;
		};

		struct SizeInfo
		{
			unsigned int lineHeight;
			float underlinePosition;
			float underlineThickness;
		};

		NzFont();
		NzFont(NzFont&& font) = default;
		~NzFont();

		void ClearGlyphCache();
		void ClearKerningCache();
		void ClearSizeInfoCache();

		bool Create(NzFontData* data);
		void Destroy();

		bool ExtractGlyph(unsigned int characterSize, char32_t character, nzUInt32 style, NzFontGlyph* glyph) const;

		const Atlas& GetAtlas(unsigned int atlasIndex) const;
		unsigned int GetAtlasCount() const;
		unsigned int GetCachedGlyphCount(unsigned int characterSize, nzUInt32 style) const;
		unsigned int GetCachedGlyphCount() const;
		NzString GetFamilyName() const;
		int GetKerning(unsigned int characterSize, char32_t first, char32_t second) const;
		const Glyph& GetGlyph(unsigned int characterSize, nzUInt32 style, char32_t character) const;
		unsigned int GetMaxAtlasSize() const;
		unsigned int GetMinimumStepSize() const;
		const SizeInfo& GetSizeInfo(unsigned int characterSize) const;
		NzString GetStyleName() const;

		bool IsValid() const;

		bool Precache(unsigned int characterSize, nzUInt32 style, char32_t character) const;
		bool Precache(unsigned int characterSize, nzUInt32 style, const NzString& characterSet) const;

		// Open
		bool OpenFromFile(const NzString& filePath, const NzFontParams& params = NzFontParams());
		bool OpenFromMemory(const void* data, std::size_t size, const NzFontParams& params = NzFontParams());
		bool OpenFromStream(NzInputStream& stream, const NzFontParams& params = NzFontParams());

		void SetMaxAtlasSize(unsigned int maxAtlasSize);
		void SetMinimumStepSize(unsigned int minimumSizeStep);

		NzFont& operator=(NzFont&& font) = default;

		static unsigned int GetDefaultMaxAtlasSize();
		static void SetDefaultMaxAtlasSize(unsigned int maxAtlasSize);

	private:
		using GlyphMap = std::unordered_map<char32_t, Glyph>;

		struct QueuedGlyph
		{
            char32_t codepoint;
            NzImage image;
            GlyphMap* map;
		};

		nzUInt64 ComputeKey(unsigned int characterSize, nzUInt32 style) const;
		unsigned int GetRealMaxAtlasSize() const;
		unsigned int InsertRect(NzRectui* rect, bool* flipped) const;
		const Glyph& PrecacheGlyph(GlyphMap& glyphMap, unsigned int characterSize, bool bold, char32_t character) const;
		void ProcessGlyphQueue() const;

		std::unique_ptr<NzFontData> m_data;
		mutable std::unordered_map<nzUInt64, std::unordered_map<nzUInt64, int>> m_kerningCache;
		mutable std::unordered_map<nzUInt64, GlyphMap> m_glyphes;
		mutable std::unordered_map<nzUInt64, SizeInfo> m_sizeInfoCache;
		mutable std::vector<Atlas> m_atlases;
		mutable std::vector<QueuedGlyph> m_glyphQueue;
		unsigned int m_maxAtlasSize;
		unsigned int m_minimumSizeStep;

		static NzFontLoader::LoaderList s_loaders;
		static unsigned int s_maxAtlasSize;
};

#endif // NAZARA_FONT_HPP
