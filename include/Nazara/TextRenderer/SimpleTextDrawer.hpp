// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Text renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_TEXTRENDERER_SIMPLETEXTDRAWER_HPP
#define NAZARA_TEXTRENDERER_SIMPLETEXTDRAWER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/TextRenderer/AbstractTextDrawer.hpp>
#include <Nazara/TextRenderer/Font.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_TEXTRENDERER_API SimpleTextDrawer : public AbstractTextDrawer
	{
		public:
			inline SimpleTextDrawer();
			inline SimpleTextDrawer(const SimpleTextDrawer& drawer);
			inline SimpleTextDrawer(SimpleTextDrawer&& drawer) noexcept;
			~SimpleTextDrawer() = default;

			inline void AppendText(std::string_view str);

			void Clear() override;

			const Rectf& GetBounds() const override;
			inline float GetCharacterSpacingOffset() const;
			inline unsigned int GetCharacterSize() const;
			const std::shared_ptr<Font>& GetFont(std::size_t index) const override;
			std::size_t GetFontCount() const override;
			const Glyph* GetGlyphs() const override;
			std::size_t GetGlyphCount() const override;
			const Line* GetLines() const override;
			std::size_t GetLineCount() const override;
			inline float GetLineHeight() const;
			inline float GetLineSpacingOffset() const;
			float GetMaxLineWidth() const override;
			const Sprite* GetSprites() const override;
			std::size_t GetSpriteCount() const override;

			inline const std::string& GetText() const;
			inline const Color& GetTextColor() const;
			inline const std::shared_ptr<Font>& GetTextFont() const;
			inline const Color& GetTextOutlineColor() const;
			inline float GetTextOutlineThickness() const;
			inline TextStyleFlags GetTextStyle() const;

			inline void SetCharacterSpacingOffset(float offset);
			inline void SetCharacterSize(unsigned int characterSize);
			inline void SetLineSpacingOffset(float offset);
			inline void SetMaxLineWidth(float lineWidth) override;
			inline void SetText(std::string str);
			inline void SetTextColor(const Color& color);
			inline void SetTextFont(std::shared_ptr<Font> font);
			inline void SetTextOutlineColor(const Color& color);
			inline void SetTextOutlineThickness(float thickness);
			inline void SetTextStyle(TextStyleFlags style);

			inline SimpleTextDrawer& operator=(const SimpleTextDrawer& drawer);
			inline SimpleTextDrawer& operator=(SimpleTextDrawer&& drawer) noexcept;

			static inline SimpleTextDrawer Draw(std::string str, unsigned int characterSize, TextStyleFlags style = TextStyle_Regular, const Color& color = Color::White());
			static inline SimpleTextDrawer Draw(std::string str, unsigned int characterSize, TextStyleFlags style, const Color& color, float outlineThickness, const Color& outlineColor);
			static inline SimpleTextDrawer Draw(const std::shared_ptr<Font>& font, std::string str, unsigned int characterSize, TextStyleFlags style = TextStyle_Regular, const Color& color = Color::White());
			static inline SimpleTextDrawer Draw(const std::shared_ptr<Font>& font, std::string str, unsigned int characterSize, TextStyleFlags style, const Color& color, float outlineThickness, const Color& outlineColor);

		private:
			inline void AppendNewLine() const;
			void AppendNewLine(std::size_t glyphIndex, float glyphPosition) const;

			void ClearGlyphs() const;

			inline void ConnectFontSlots();
			inline void DisconnectFontSlots();

			void GenerateGlyphs(std::string_view text) const;
			bool GenerateSprite(Rectf& bounds, Sprite& sprite, char32_t character, float outlineThickness, bool lineWrap, Color color, int renderOrder, int* advance) const;

			inline float GetLineHeight(const Font::SizeInfo& sizeInfo) const;

			inline void InvalidateColor();
			inline void InvalidateGlyphs();

			void OnFontAtlasLayerChanged(const Font* font, AbstractImage* oldLayer, AbstractImage* newLayer);
			void OnFontInvalidated(const Font* font);
			void OnFontRelease(const Font* object);

			inline bool ShouldLineWrap(float size) const;

			inline void UpdateGlyphs() const;
			inline void UpdateSpriteColor() const;

			static constexpr std::size_t InvalidGlyph = std::numeric_limits<std::size_t>::max();

			NazaraSlot(Font, OnFontAtlasChanged, m_atlasChangedSlot);
			NazaraSlot(Font, OnFontAtlasLayerChanged, m_atlasLayerChangedSlot);
			NazaraSlot(Font, OnFontGlyphCacheCleared, m_glyphCacheClearedSlot);
			NazaraSlot(Font, OnFontRelease, m_fontReleaseSlot);

			mutable std::size_t m_lastSeparatorGlyph;
			mutable std::vector<Glyph> m_glyphs;
			mutable std::vector<Sprite> m_sprites;
			mutable std::vector<Line> m_lines;
			std::string m_text;
			Color m_color;
			Color m_outlineColor;
			std::shared_ptr<Font> m_font;
			mutable Rectf m_bounds;
			TextStyleFlags m_style;
			mutable UInt32 m_previousCharacter;
			mutable Vector2f m_drawPos;
			mutable bool m_colorUpdated;
			mutable bool m_glyphUpdated;
			mutable float m_lastSeparatorPosition;
			float m_characterSpacingOffset;
			float m_lineSpacingOffset;
			float m_maxLineWidth;
			float m_outlineThickness;
			unsigned int m_characterSize;
	};
}

#include <Nazara/TextRenderer/SimpleTextDrawer.inl>

#endif // NAZARA_TEXTRENDERER_SIMPLETEXTDRAWER_HPP
