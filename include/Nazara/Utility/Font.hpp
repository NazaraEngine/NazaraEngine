// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

///TODO: FontManager ?

#pragma once

#ifndef NAZARA_FONT_HPP
#define NAZARA_FONT_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceLoader.hpp>
#include <Nazara/Core/ResourceParameters.hpp>
#include <Nazara/Utility/AbstractAtlas.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <memory>
#include <unordered_map>

namespace Nz
{
	struct NAZARA_UTILITY_API FontParams : ResourceParameters
	{
		bool IsValid() const;
	};

	class Font;
	class FontData;

	struct FontGlyph;

	using FontLibrary = ObjectLibrary<Font>;
	using FontLoader = ResourceLoader<Font, FontParams>;

	class NAZARA_UTILITY_API Font : public Resource
	{
		friend FontLibrary;
		friend FontLoader;
		friend class Utility;

		public:
			struct Glyph;
			struct SizeInfo;

			Font();
			Font(const Font&) = delete;
			Font(Font&&) = delete;
			~Font();

			void ClearGlyphCache();
			void ClearKerningCache();
			void ClearSizeInfoCache();

			bool Create(FontData* data);
			void Destroy();

			bool ExtractGlyph(unsigned int characterSize, char32_t character, TextStyleFlags style, float outlineThickness, FontGlyph* glyph) const;

			const std::shared_ptr<AbstractAtlas>& GetAtlas() const;
			std::size_t GetCachedGlyphCount(unsigned int characterSize, TextStyleFlags style, float outlineThickness) const;
			std::size_t GetCachedGlyphCount() const;
			std::string GetFamilyName() const;
			int GetKerning(unsigned int characterSize, char32_t first, char32_t second) const;
			const Glyph& GetGlyph(unsigned int characterSize, TextStyleFlags style, float outlineThickness, char32_t character) const;
			unsigned int GetGlyphBorder() const;
			unsigned int GetMinimumStepSize() const;
			const SizeInfo& GetSizeInfo(unsigned int characterSize) const;
			std::string GetStyleName() const;

			bool IsValid() const;

			bool Precache(unsigned int characterSize, TextStyleFlags style, float outlineThickness, char32_t character) const;
			bool Precache(unsigned int characterSize, TextStyleFlags style, float outlineThickness, const std::string& characterSet) const;

			void SetAtlas(const std::shared_ptr<AbstractAtlas>& atlas);
			void SetGlyphBorder(unsigned int borderSize);
			void SetMinimumStepSize(unsigned int minimumStepSize);

			Font& operator=(const Font&) = delete;
			Font& operator=(Font&&) = delete;

			static std::shared_ptr<AbstractAtlas> GetDefaultAtlas();
			static const std::shared_ptr<Font>& GetDefault();
			static unsigned int GetDefaultGlyphBorder();
			static unsigned int GetDefaultMinimumStepSize();

			static std::shared_ptr<Font> OpenFromFile(const std::filesystem::path& filePath, const FontParams& params = FontParams());
			static std::shared_ptr<Font> OpenFromMemory(const void* data, std::size_t size, const FontParams& params = FontParams());
			static std::shared_ptr<Font> OpenFromStream(Stream& stream, const FontParams& params = FontParams());

			static void SetDefaultAtlas(const std::shared_ptr<AbstractAtlas>& atlas);
			static void SetDefaultGlyphBorder(unsigned int borderSize);
			static void SetDefaultMinimumStepSize(unsigned int minimumStepSize);

			struct Glyph
			{
				Recti aabb;
				Rectui atlasRect;
				bool requireFauxBold;
				bool requireFauxItalic;
				bool flipped;
				bool valid;
				float fauxOutlineThickness;
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

			// Signals:
			NazaraSignal(OnFontAtlasChanged, const Font* /*font*/);
			NazaraSignal(OnFontAtlasLayerChanged, const Font* /*font*/, AbstractImage* /*oldLayer*/, AbstractImage* /*newLayer*/);
			NazaraSignal(OnFontDestroy, const Font* /*font*/);
			NazaraSignal(OnFontGlyphCacheCleared, const Font* /*font*/);
			NazaraSignal(OnFontKerningCacheCleared, const Font* /*font*/);
			NazaraSignal(OnFontRelease, const Font* /*font*/);
			NazaraSignal(OnFontSizeInfoCacheCleared, const Font* /*font*/);

		private:
			using GlyphMap = std::unordered_map<char32_t, Glyph>;

			UInt64 ComputeKey(unsigned int characterSize, TextStyleFlags style, float outlineThickness) const;
			void OnAtlasCleared(const AbstractAtlas* atlas);
			void OnAtlasLayerChange(const AbstractAtlas* atlas, AbstractImage* oldLayer, AbstractImage* newLayer);
			void OnAtlasRelease(const AbstractAtlas* atlas);
			const Glyph& PrecacheGlyph(GlyphMap& glyphMap, unsigned int characterSize, TextStyleFlags style, float outlineThickness, char32_t character) const;

			static bool Initialize();
			static void Uninitialize();

			NazaraSlot(AbstractAtlas, OnAtlasCleared, m_atlasClearedSlot);
			NazaraSlot(AbstractAtlas, OnAtlasLayerChange, m_atlasLayerChangeSlot);
			NazaraSlot(AbstractAtlas, OnAtlasRelease, m_atlasReleaseSlot);

			std::shared_ptr<AbstractAtlas> m_atlas;
			std::unique_ptr<FontData> m_data;
			mutable std::unordered_map<UInt64, std::unordered_map<UInt64, int>> m_kerningCache;
			mutable std::unordered_map<UInt64, GlyphMap> m_glyphes;
			mutable std::unordered_map<UInt64, SizeInfo> m_sizeInfoCache;
			unsigned int m_glyphBorder;
			unsigned int m_minimumStepSize;

			static std::shared_ptr<AbstractAtlas> s_defaultAtlas;
			static std::shared_ptr<Font> s_defaultFont;
			static unsigned int s_defaultGlyphBorder;
			static unsigned int s_defaultMinimumStepSize;
	};
}

#include <Nazara/Utility/Font.inl>

#endif // NAZARA_FONT_HPP
