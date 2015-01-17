// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FONT_HPP
#define NAZARA_FONT_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/NonCopyable.hpp>
#include <Nazara/Core/ResourceRef.hpp>
#include <Nazara/Core/ResourceLoader.hpp>
#include <Nazara/Utility/AbstractAtlas.hpp>
#include <memory>
#include <unordered_map>

struct NAZARA_API NzFontParams
{
	bool IsValid() const;
};

class NzFont;
class NzFontData;

struct NzFontGlyph;

using NzFontConstRef = NzResourceRef<const NzFont>;
using NzFontLoader = NzResourceLoader<NzFont, NzFontParams>;
using NzFontRef = NzResourceRef<NzFont>;

class NAZARA_API NzFont : public NzResource, NzAbstractAtlas::Listener, NzNonCopyable
{
	friend NzFontLoader;

	public:
		struct Glyph;
		struct SizeInfo;

		NzFont();
		~NzFont();

		void ClearGlyphCache();
		void ClearKerningCache();
		void ClearSizeInfoCache();

		bool Create(NzFontData* data);
		void Destroy();

		bool ExtractGlyph(unsigned int characterSize, char32_t character, nzUInt32 style, NzFontGlyph* glyph) const;

		const NzAbstractAtlas* GetAtlas() const;
		unsigned int GetCachedGlyphCount(unsigned int characterSize, nzUInt32 style) const;
		unsigned int GetCachedGlyphCount() const;
		NzString GetFamilyName() const;
		int GetKerning(unsigned int characterSize, char32_t first, char32_t second) const;
		const Glyph& GetGlyph(unsigned int characterSize, nzUInt32 style, char32_t character) const;
		unsigned int GetGlyphBorder() const;
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

		void SetAtlas(std::shared_ptr<NzAbstractAtlas> atlas);
		void SetGlyphBorder(unsigned int borderSize);
		void SetMinimumStepSize(unsigned int minimumSizeStep);

		static unsigned int GetDefaultGlyphBorder();
		static unsigned int GetDefaultMinimumStepSize();
		static void SetDefaultGlyphBorder(unsigned int borderSize);
		static void SetDefaultMinimumStepSize(unsigned int minimumSizeStep);

		enum ModicationCode
		{
			ModificationCode_AtlasChanged,
			ModificationCode_AtlasLayerChanged,
			ModificationCode_GlyphCacheCleared,
			ModificationCode_KerningCacheCleared,
			ModificationCode_SizeInfoCacheCleared
		};

		struct Glyph
		{
			NzRecti aabb;
			NzRectui atlasRect;
			bool requireFauxBold;
			bool requireFauxItalic;
			bool flipped;
			bool valid;
			int advance;
			unsigned int layerIndex;
		};

		struct SizeInfo
		{
			int spaceAdvance;
			unsigned int lineHeight;
			float underlinePosition;
			float underlineThickness;
		};

	private:
		using GlyphMap = std::unordered_map<char32_t, Glyph>;

		nzUInt64 ComputeKey(unsigned int characterSize, nzUInt32 style) const;
		bool OnAtlasCleared(const NzAbstractAtlas* atlas, void* userdata) override;
		bool OnAtlasLayerChange(const NzAbstractAtlas* atlas, NzAbstractImage* oldLayer, NzAbstractImage* newLayer, void* userdata) override;
		void OnAtlasReleased(const NzAbstractAtlas* atlas, void* userdata) override;
		const Glyph& PrecacheGlyph(GlyphMap& glyphMap, unsigned int characterSize, nzUInt32 style, char32_t character) const;

		std::shared_ptr<NzAbstractAtlas> m_atlas;
		std::unique_ptr<NzFontData> m_data;
		mutable std::unordered_map<nzUInt64, std::unordered_map<nzUInt64, int>> m_kerningCache;
		mutable std::unordered_map<nzUInt64, GlyphMap> m_glyphes;
		mutable std::unordered_map<nzUInt64, SizeInfo> m_sizeInfoCache;
		unsigned int m_glyphBorder;
		unsigned int m_minimumSizeStep;

		static NzFontLoader::LoaderList s_loaders;
		static unsigned int s_defaultGlyphBorder;
		static unsigned int s_defaultMinimumSizeStep;
};

#endif // NAZARA_FONT_HPP
