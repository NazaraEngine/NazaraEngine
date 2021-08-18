// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RICHTEXTDRAWER_HPP
#define NAZARA_RICHTEXTDRAWER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Utility/AbstractTextDrawer.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <Nazara/Utility/Font.hpp>
#include <string>
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

			BlockRef AppendText(const std::string& str, bool forceNewBlock = false);

			void Clear() override;

			inline std::size_t FindBlock(std::size_t glyphIndex) const;

			inline unsigned int GetBlockCharacterSize(std::size_t index) const;
			inline float GetBlockCharacterSpacingOffset(std::size_t index) const;
			inline const Color& GetBlockColor(std::size_t index) const;
			inline std::size_t GetBlockCount() const;
			inline std::size_t GetBlockFirstGlyphIndex(std::size_t index) const;
			inline const std::shared_ptr<Font>& GetBlockFont(std::size_t index) const;
			inline float GetBlockLineHeight(std::size_t index) const;
			inline float GetBlockLineSpacingOffset(std::size_t index) const;
			inline const Color& GetBlockOutlineColor(std::size_t index) const;
			inline float GetBlockOutlineThickness(std::size_t index) const;
			inline TextStyleFlags GetBlockStyle(std::size_t index) const;
			inline const std::string& GetBlockText(std::size_t index) const;

			inline BlockRef GetBlock(std::size_t index);
			const Rectf& GetBounds() const override;
			inline unsigned int GetDefaultCharacterSize() const;
			inline float GetDefaultCharacterSpacingOffset() const;
			inline const Color& GetDefaultColor() const;
			inline const std::shared_ptr<Font>& GetDefaultFont() const;
			inline float GetDefaultLineSpacingOffset() const;
			inline const Color& GetDefaultOutlineColor() const;
			inline float GetDefaultOutlineThickness() const;
			inline TextStyleFlags GetDefaultStyle() const;
			const std::shared_ptr<Font>& GetFont(std::size_t index) const override;
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
			inline void SetBlockCharacterSpacingOffset(std::size_t index, float offset);
			inline void SetBlockColor(std::size_t index, const Color& color);
			inline void SetBlockFont(std::size_t index, std::shared_ptr<Font> font);
			inline void SetBlockLineSpacingOffset(std::size_t index, float offset);
			inline void SetBlockOutlineColor(std::size_t index, const Color& color);
			inline void SetBlockOutlineThickness(std::size_t index, float thickness);
			inline void SetBlockStyle(std::size_t index, TextStyleFlags style);
			inline void SetBlockText(std::size_t index, std::string str);

			inline void SetDefaultCharacterSize(unsigned int characterSize);
			inline void SetDefaultCharacterSpacingOffset(float offset);
			inline void SetDefaultColor(const Color& color);
			inline void SetDefaultFont(const std::shared_ptr<Font>& font);
			inline void SetDefaultLineSpacingOffset(float offset);
			inline void SetDefaultOutlineColor(const Color& color);
			inline void SetDefaultOutlineThickness(float thickness);
			inline void SetDefaultStyle(TextStyleFlags style);

			void SetMaxLineWidth(float lineWidth) override;

			RichTextDrawer& operator=(const RichTextDrawer& drawer);
			RichTextDrawer& operator=(RichTextDrawer&& drawer);

			static constexpr std::size_t InvalidBlockIndex = std::numeric_limits<std::size_t>::max();

		private:
			struct Block;

			inline void AppendNewLine(const Font& font, unsigned int characterSize, float lineSpacingOffset) const;
			void AppendNewLine(const Font& font, unsigned int characterSize, float lineSpacingOffset, std::size_t glyphIndex, float glyphPosition) const;
			inline void ClearGlyphs() const;
			inline void ConnectFontSlots();
			inline void DisconnectFontSlots();
			bool GenerateGlyph(Glyph& glyph, char32_t character, float outlineThickness, bool lineWrap, const Font& font, const Color& color, TextStyleFlags style, float lineSpacingOffset, unsigned int characterSize, int renderOrder, int* advance) const;
			void GenerateGlyphs(const Font& font, const Color& color, TextStyleFlags style, unsigned int characterSize, const Color& outlineColor, float characterSpacingOffset, float lineSpacingOffset, float outlineThickness, const std::string& text) const;
			inline float GetLineHeight(const Block& block) const;
			inline float GetLineHeight(float lineSpacingOffset, const Font::SizeInfo& sizeInfo) const;
			inline std::size_t HandleFontAddition(const std::shared_ptr<Font>& font);
			inline void InvalidateGlyphs();
			inline void ReleaseFont(std::size_t fontIndex);
			inline bool ShouldLineWrap(float size) const;

			void OnFontAtlasLayerChanged(const Font* font, AbstractImage* oldLayer, AbstractImage* newLayer);
			void OnFontInvalidated(const Font* font);
			void OnFontRelease(const Font* object);

			void UpdateGlyphs() const;

			static constexpr std::size_t InvalidGlyph = std::numeric_limits<std::size_t>::max();

			struct Block
			{
				std::size_t fontIndex;
				std::size_t glyphIndex;
				std::string text;
				Color color;
				Color outlineColor;
				TextStyleFlags style;
				float characterSpacingOffset;
				float lineSpacingOffset;
				float outlineThickness;
				unsigned int characterSize;
			};

			struct FontData
			{
				std::shared_ptr<Font> font;
				std::size_t useCount = 0;

				NazaraSlot(Font, OnFontAtlasChanged, atlasChangedSlot);
				NazaraSlot(Font, OnFontAtlasLayerChanged, atlasLayerChangedSlot);
				NazaraSlot(Font, OnFontGlyphCacheCleared, glyphCacheClearedSlot);
				NazaraSlot(Font, OnFontRelease, fontReleaseSlot);
			};

			Color m_defaultColor;
			Color m_defaultOutlineColor;
			TextStyleFlags m_defaultStyle;
			std::shared_ptr<Font> m_defaultFont;
			mutable std::size_t m_lastSeparatorGlyph;
			std::unordered_map<std::shared_ptr<Font>, std::size_t> m_fontIndexes;
			std::vector<Block> m_blocks;
			std::vector<FontData> m_fonts;
			mutable std::vector<Glyph> m_glyphs;
			mutable std::vector<Line> m_lines;
			mutable Rectf m_bounds;
			mutable Vector2f m_drawPos;
			mutable bool m_glyphUpdated;
			float m_defaultCharacterSpacingOffset;
			float m_defaultLineSpacingOffset;
			float m_defaultOutlineThickness;
			float m_maxLineWidth;
			unsigned int m_defaultCharacterSize;
			mutable float m_lastSeparatorPosition;
	};

	class RichTextDrawer::BlockRef
	{
		friend RichTextDrawer;

		public:
			BlockRef(const BlockRef&) = default;
			BlockRef(BlockRef&&) = delete;
			~BlockRef() = default;

			inline float GetCharacterSpacingOffset() const;
			inline unsigned int GetCharacterSize() const;
			inline Color GetColor() const;
			inline std::size_t GetFirstGlyphIndex() const;
			inline const std::shared_ptr<Font>& GetFont() const;
			inline float GetLineSpacingOffset() const;
			inline Color GetOutlineColor() const;
			inline float GetOutlineThickness() const;
			inline TextStyleFlags GetStyle() const;
			inline const std::string& GetText() const;

			inline void SetCharacterSpacingOffset(float offset);
			inline void SetCharacterSize(unsigned int size);
			inline void SetColor(Color color);
			inline void SetFont(std::shared_ptr<Font> font);
			inline void SetLineSpacingOffset(float offset);
			inline void SetOutlineColor(Color color);
			inline void SetOutlineThickness(float thickness);
			inline void SetStyle(TextStyleFlags style);
			inline void SetText(std::string text);

			BlockRef& operator=(const BlockRef&) = delete;
			BlockRef& operator=(BlockRef&&) = delete;

		private:
			inline BlockRef(RichTextDrawer& drawer, std::size_t index);

			std::size_t m_blockIndex;
			RichTextDrawer& m_drawer;
	};
}

#include <Nazara/Utility/RichTextDrawer.inl>

#endif // NAZARA_RICHTEXTDRAWER_HPP
