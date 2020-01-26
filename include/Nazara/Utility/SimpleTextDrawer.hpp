// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SIMPLETEXTDRAWER_HPP
#define NAZARA_SIMPLETEXTDRAWER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Utility/AbstractTextDrawer.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <Nazara/Utility/Font.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_UTILITY_API SimpleTextDrawer : public AbstractTextDrawer
	{
		public:
			inline SimpleTextDrawer();
			inline SimpleTextDrawer(const SimpleTextDrawer& drawer);
			inline SimpleTextDrawer(SimpleTextDrawer&& drawer);
			~SimpleTextDrawer() = default;

			inline void AppendText(const String& str);

			void Clear() override;

			const Rectf& GetBounds() const override;
			inline float GetCharacterSpacingOffset() const;
			inline unsigned int GetCharacterSize() const;
			inline const Color& GetColor() const;
			inline Font* GetFont() const;
			Font* GetFont(std::size_t index) const override;
			std::size_t GetFontCount() const override;
			const Glyph& GetGlyph(std::size_t index) const override;
			std::size_t GetGlyphCount() const override;
			const Line& GetLine(std::size_t index) const override;
			std::size_t GetLineCount() const override;
			inline float GetLineHeight() const;
			inline float GetLineSpacingOffset() const;
			float GetMaxLineWidth() const override;
			inline const Color& GetOutlineColor() const;
			inline float GetOutlineThickness() const;
			inline TextStyleFlags GetStyle() const;
			inline const String& GetText() const;

			inline void SetCharacterSpacingOffset(float offset);
			inline void SetCharacterSize(unsigned int characterSize);
			inline void SetColor(const Color& color);
			inline void SetFont(Font* font);
			inline void SetLineSpacingOffset(float offset);
			inline void SetMaxLineWidth(float lineWidth) override;
			inline void SetOutlineColor(const Color& color);
			inline void SetOutlineThickness(float thickness);
			inline void SetStyle(TextStyleFlags style);
			inline void SetText(const String& str);

			inline SimpleTextDrawer& operator=(const SimpleTextDrawer& drawer);
			inline SimpleTextDrawer& operator=(SimpleTextDrawer&& drawer);

			static inline SimpleTextDrawer Draw(const String& str, unsigned int characterSize, TextStyleFlags style = TextStyle_Regular, const Color& color = Color::White);
			static inline SimpleTextDrawer Draw(const String& str, unsigned int characterSize, TextStyleFlags style, const Color& color, float outlineThickness, const Color& outlineColor);
			static inline SimpleTextDrawer Draw(Font* font, const String& str, unsigned int characterSize, TextStyleFlags style = TextStyle_Regular, const Color& color = Color::White);
			static inline SimpleTextDrawer Draw(Font* font, const String& str, unsigned int characterSize, TextStyleFlags style, const Color& color, float outlineThickness, const Color& outlineColor);

		private:
			inline void AppendNewLine() const;
			void AppendNewLine(std::size_t glyphIndex, float glyphPosition) const;

			void ClearGlyphs() const;

			inline void ConnectFontSlots();
			inline void DisconnectFontSlots();

			bool GenerateGlyph(Glyph& glyph, char32_t character, float outlineThickness, bool lineWrap, Nz::Color color, int renderOrder, int* advance) const;
			void GenerateGlyphs(const String& text) const;

			inline float GetLineHeight(const Font::SizeInfo& sizeInfo) const;

			inline void InvalidateColor();
			inline void InvalidateGlyphs();

			void OnFontAtlasLayerChanged(const Font* font, AbstractImage* oldLayer, AbstractImage* newLayer);
			void OnFontInvalidated(const Font* font);
			void OnFontRelease(const Font* object);

			inline bool ShouldLineWrap(float size) const;

			inline void UpdateGlyphColor() const;
			inline void UpdateGlyphs() const;

			static constexpr std::size_t InvalidGlyph = std::numeric_limits<std::size_t>::max();

			NazaraSlot(Font, OnFontAtlasChanged, m_atlasChangedSlot);
			NazaraSlot(Font, OnFontAtlasLayerChanged, m_atlasLayerChangedSlot);
			NazaraSlot(Font, OnFontGlyphCacheCleared, m_glyphCacheClearedSlot);
			NazaraSlot(Font, OnFontRelease, m_fontReleaseSlot);

			mutable std::size_t m_lastSeparatorGlyph;
			mutable std::vector<Glyph> m_glyphs;
			mutable std::vector<Line> m_lines;
			Color m_color;
			Color m_outlineColor;
			FontRef m_font;
			mutable Rectf m_bounds;
			String m_text;
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

#include <Nazara/Utility/SimpleTextDrawer.inl>

#endif // NAZARA_SIMPLETEXTDRAWER_HPP
