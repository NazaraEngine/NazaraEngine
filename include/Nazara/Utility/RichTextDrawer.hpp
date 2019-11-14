// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RICHTEXTDRAWER_HPP
#define NAZARA_RICHTEXTDRAWER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Utility/AbstractTextDrawer.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <Nazara/Utility/Font.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_UTILITY_API RichTextDrawer : public AbstractTextDrawer
	{
		public:
			class BlockRef;

			RichTextDrawer();
			RichTextDrawer(const RichTextDrawer& drawer);
			RichTextDrawer(RichTextDrawer&& drawer);
			~RichTextDrawer();

			BlockRef AppendText(const String& str, bool forceNewBlock = false);

			void Clear() override;

			inline std::size_t FindBlock(std::size_t glyphIndex) const;

			inline unsigned int GetBlockCharacterSize(std::size_t index) const;
			inline const Color& GetBlockColor(std::size_t index) const;
			inline std::size_t GetBlockCount() const;
			inline std::size_t GetBlockFirstGlyphIndex(std::size_t index) const;
			inline const FontRef& GetBlockFont(std::size_t index) const;
			inline TextStyleFlags GetBlockStyle(std::size_t index) const;
			inline const String& GetBlockText(std::size_t index) const;

			inline BlockRef GetBlock(std::size_t index);
			const Recti& GetBounds() const override;
			inline unsigned int GetDefaultCharacterSize() const;
			inline const Color& GetDefaultColor() const;
			inline const FontRef& GetDefaultFont() const;
			inline TextStyleFlags GetDefaultStyle() const;
			Font* GetFont(std::size_t index) const override;
			std::size_t GetFontCount() const override;
			const Glyph& GetGlyph(std::size_t index) const override;
			std::size_t GetGlyphCount() const override;
			const Line& GetLine(std::size_t index) const override;
			std::size_t GetLineCount() const override;
			float GetMaxLineWidth() const override;

			inline bool HasBlocks() const;

			void MergeBlocks();

			void RemoveBlock(std::size_t index);

			inline void SetBlockCharacterSize(std::size_t index, unsigned int characterSize);
			inline void SetBlockColor(std::size_t index, const Color& color);
			inline void SetBlockFont(std::size_t index, FontRef font);
			inline void SetBlockStyle(std::size_t index, TextStyleFlags style);
			inline void SetBlockText(std::size_t index, String str);

			inline void SetDefaultCharacterSize(unsigned int characterSize);
			inline void SetDefaultColor(const Color& color);
			inline void SetDefaultFont(const FontRef& font);
			inline void SetDefaultStyle(TextStyleFlags style);

			void SetMaxLineWidth(float lineWidth) override;

			RichTextDrawer& operator=(const RichTextDrawer& drawer);
			RichTextDrawer& operator=(RichTextDrawer&& drawer);

			static constexpr std::size_t InvalidBlockIndex = std::numeric_limits<std::size_t>::max();

			//static RichTextDrawer Draw(const String& str, unsigned int characterSize, TextStyleFlags style = TextStyle_Regular, const Color& color = Color::White);
			//static RichTextDrawer Draw(Font* font, const String& str, unsigned int characterSize, TextStyleFlags style = TextStyle_Regular, const Color& color = Color::White);

		private:
			struct Block;

			inline void AppendNewLine(const Font* font, unsigned int characterSize) const;
			inline void ClearGlyphs() const;
			inline void ConnectFontSlots();
			inline void DisconnectFontSlots();
			bool GenerateGlyph(Glyph& glyph, char32_t character, float outlineThickness, bool lineWrap, const Font* font, const Color& color, TextStyleFlags style, unsigned int characterSize, int renderOrder, int* advance) const;
			void GenerateGlyphs(const Font* font, const Color& color, TextStyleFlags style, unsigned int characterSize, const Color& outlineColor, float outlineThickness, const String& text) const;
			inline std::size_t HandleFontAddition(const FontRef& font);
			inline void ReleaseFont(std::size_t fontIndex);

			inline void InvalidateGlyphs();

			void OnFontAtlasLayerChanged(const Font* font, AbstractImage* oldLayer, AbstractImage* newLayer);
			void OnFontInvalidated(const Font* font);
			void OnFontRelease(const Font* object);

			void UpdateGlyphs() const;

			struct Block
			{
				std::size_t fontIndex;
				std::size_t glyphIndex;
				Color color;
				String text;
				TextStyleFlags style;
				unsigned int characterSize;
			};

			struct FontData
			{
				FontRef font;
				std::size_t useCount = 0;

				NazaraSlot(Font, OnFontAtlasChanged, atlasChangedSlot);
				NazaraSlot(Font, OnFontAtlasLayerChanged, atlasLayerChangedSlot);
				NazaraSlot(Font, OnFontGlyphCacheCleared, glyphCacheClearedSlot);
				NazaraSlot(Font, OnFontRelease, fontReleaseSlot);
			};

			Color m_defaultColor;
			TextStyleFlags m_defaultStyle;
			FontRef m_defaultFont;
			std::unordered_map<FontRef, std::size_t> m_fontIndexes;
			std::vector<Block> m_blocks;
			std::vector<FontData> m_fonts;
			mutable std::vector<Glyph> m_glyphs;
			mutable std::vector<Line> m_lines;
			mutable Rectf m_workingBounds;
			mutable Recti m_bounds;
			mutable Vector2ui m_drawPos;
			mutable bool m_glyphUpdated;
			float m_maxLineWidth;
			unsigned int m_defaultCharacterSize;
	};

	class RichTextDrawer::BlockRef
	{
		friend RichTextDrawer;

		public:
			BlockRef(const BlockRef&) = default;
			BlockRef(BlockRef&&) = default;
			~BlockRef() = default;

			inline unsigned int GetCharacterSize() const;
			inline Color GetColor() const;
			inline std::size_t GetFirstGlyphIndex() const;
			inline const FontRef& GetFont() const;
			inline TextStyleFlags GetStyle() const;
			inline const String& GetText() const;

			inline void SetCharacterSize(unsigned int size);
			inline void SetColor(Color color);
			inline void SetFont(FontRef font);
			inline void SetStyle(TextStyleFlags style);
			inline void SetText(const String& text);

			BlockRef& operator=(const BlockRef&) = default;
			BlockRef& operator=(BlockRef&&) = default;

		private:
			inline BlockRef(RichTextDrawer& drawer, std::size_t index);

			std::size_t m_blockIndex;
			RichTextDrawer& m_drawer;
	};
}

#include <Nazara/Utility/RichTextDrawer.inl>

#endif // NAZARA_RICHTEXTDRAWER_HPP
