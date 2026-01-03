// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Text renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_TEXTRENDERER_RICHTEXTDRAWER_HPP
#define NAZARA_TEXTRENDERER_RICHTEXTDRAWER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/TextRenderer/AbstractTextDrawer.hpp>
#include <Nazara/TextRenderer/Enums.hpp>
#include <Nazara/TextRenderer/Font.hpp>
#include <string>
#include <vector>

namespace Nz
{
	class NAZARA_TEXTRENDERER_API RichTextDrawer : public AbstractTextDrawer
	{
		public:
			class BlockRef;

			RichTextDrawer();
			RichTextDrawer(const RichTextDrawer& drawer);
			RichTextDrawer(RichTextDrawer&& drawer) noexcept;
			~RichTextDrawer();

			BlockRef AppendText(std::string_view str, bool forceNewBlock = false);

			void Clear() override;

			inline std::size_t FindBlock(std::size_t glyphIndex) const;

			inline unsigned int GetBlockCharacterSize(std::size_t index) const;
			inline float GetBlockCharacterSpacingOffset(std::size_t index) const;
			inline const Color& GetBlockColor(std::size_t index) const;
			inline std::size_t GetBlockCount() const;
			inline std::size_t GetBlockFirstGlyphIndex(std::size_t index) const;
			inline const std::shared_ptr<Font>& GetBlockFont(std::size_t index) const;
			inline float GetBlockLineSpacingOffset(std::size_t index) const;
			inline const Color& GetBlockOutlineColor(std::size_t index) const;
			inline float GetBlockOutlineThickness(std::size_t index) const;
			inline TextStyleFlags GetBlockStyle(std::size_t index) const;
			inline const std::string& GetBlockText(std::size_t index) const;

			inline BlockRef GetBlock(std::size_t index);
			const Rectf& GetBounds() const override;
			inline unsigned int GetCharacterSize() const;
			inline float GetCharacterSpacingOffset() const;
			inline float GetLineSpacingOffset() const;
			const std::shared_ptr<Font>& GetFont(std::size_t index) const override;
			std::size_t GetFontCount() const override;
			const Glyph* GetGlyphs() const override;
			std::size_t GetGlyphCount() const override;
			const Line* GetLines() const override;
			std::size_t GetLineCount() const override;
			float GetMaxLineWidth() const override;
			const Sprite* GetSprites() const override;
			std::size_t GetSpriteCount() const override;

			inline const Color& GetTextColor() const;
			inline const std::shared_ptr<Font>& GetTextFont() const;
			inline const Color& GetTextOutlineColor() const;
			inline float GetTextOutlineThickness() const;
			inline TextStyleFlags GetTextStyle() const;

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

			inline void SetCharacterSize(unsigned int characterSize);
			inline void SetCharacterSpacingOffset(float offset);
			inline void SetLineSpacingOffset(float offset);
			inline void SetTextColor(const Color& color);
			inline void SetTextFont(const std::shared_ptr<Font>& font);
			inline void SetTextOutlineColor(const Color& color);
			inline void SetTextOutlineThickness(float thickness);
			inline void SetTextStyle(TextStyleFlags style);

			void SetMaxLineWidth(float lineWidth) override;

			RichTextDrawer& operator=(const RichTextDrawer& drawer);
			RichTextDrawer& operator=(RichTextDrawer&& drawer) noexcept;

			static constexpr std::size_t InvalidBlockIndex = std::numeric_limits<std::size_t>::max();

		private:
			struct Block;

			inline void AppendNewLine(const Font& font, unsigned int characterSize, float lineSpacingOffset) const;
			void AppendNewLine(const Font& font, unsigned int characterSize, float lineSpacingOffset, std::size_t glyphIndex, float glyphPosition) const;
			inline void ClearGlyphs() const;
			inline void ConnectFontSlots();
			inline void DisconnectFontSlots();
			void GenerateGlyphs(const Font& font, const Color& color, TextStyleFlags style, unsigned int characterSize, const Color& outlineColor, float outlineThickness, float characterSpacingOffset, float lineSpacingOffset, std::string_view text) const;
			bool GenerateSprite(Rectf& bounds, Sprite& sprite, char32_t character, float outlineThickness, bool lineWrap, const Font& font, const Color& color, TextStyleFlags style, float lineSpacingOffset, unsigned int characterSize, int renderOrder, int* advance) const;
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

			Color m_currentColor;
			Color m_currentOutlineColor;
			TextStyleFlags m_currentStyle;
			std::shared_ptr<Font> m_currentFont;
			mutable std::size_t m_lastSeparatorGlyph;
			std::unordered_map<std::shared_ptr<Font>, std::size_t> m_fontIndexes;
			std::vector<Block> m_blocks;
			std::vector<FontData> m_fonts;
			mutable std::vector<Glyph> m_glyphs;
			mutable std::vector<Line> m_lines;
			mutable std::vector<Sprite> m_sprites;
			mutable Rectf m_bounds;
			mutable Vector2f m_drawPos;
			mutable bool m_glyphUpdated;
			float m_currentCharacterSpacingOffset;
			float m_currentLineSpacingOffset;
			float m_currentOutlineThickness;
			float m_maxLineWidth;
			unsigned int m_currentCharacterSize;
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

#include <Nazara/TextRenderer/RichTextDrawer.inl>

#endif // NAZARA_TEXTRENDERER_RICHTEXTDRAWER_HPP
