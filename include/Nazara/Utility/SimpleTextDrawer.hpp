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
			SimpleTextDrawer();
			SimpleTextDrawer(const SimpleTextDrawer& drawer);
			SimpleTextDrawer(SimpleTextDrawer&& drawer);
			virtual ~SimpleTextDrawer();

			void AppendText(const String& str);

			void Clear() override;

			const Recti& GetBounds() const override;
			unsigned int GetCharacterSize() const;
			const Color& GetColor() const;
			Font* GetFont() const;
			Font* GetFont(std::size_t index) const override;
			std::size_t GetFontCount() const override;
			const Glyph& GetGlyph(std::size_t index) const override;
			std::size_t GetGlyphCount() const override;
			const Line& GetLine(std::size_t index) const override;
			std::size_t GetLineCount() const override;
			float GetMaxLineWidth() const;
			const Color& GetOutlineColor() const;
			float GetOutlineThickness() const;
			TextStyleFlags GetStyle() const;
			const String& GetText() const;

			void SetCharacterSize(unsigned int characterSize);
			void SetColor(const Color& color);
			void SetFont(Font* font);
			void SetMaxLineWidth(float lineWidth);
			void SetOutlineColor(const Color& color);
			void SetOutlineThickness(float thickness);
			void SetStyle(TextStyleFlags style);
			void SetText(const String& str);

			SimpleTextDrawer& operator=(const SimpleTextDrawer& drawer);
			SimpleTextDrawer& operator=(SimpleTextDrawer&& drawer);

			static SimpleTextDrawer Draw(const String& str, unsigned int characterSize, TextStyleFlags style = TextStyle_Regular, const Color& color = Color::White);
			static SimpleTextDrawer Draw(const String& str, unsigned int characterSize, TextStyleFlags style, const Color& color, float outlineThickness, const Color& outlineColor);
			static SimpleTextDrawer Draw(Font* font, const String& str, unsigned int characterSize, TextStyleFlags style = TextStyle_Regular, const Color& color = Color::White);
			static SimpleTextDrawer Draw(Font* font, const String& str, unsigned int characterSize, TextStyleFlags style, const Color& color, float outlineThickness, const Color& outlineColor);

		private:
			void AppendNewLine() const;
			void AppendNewLine(std::size_t glyphIndex, unsigned int glyphPosition) const;
			void ClearGlyphs() const;
			void ConnectFontSlots();
			void DisconnectFontSlots();
			bool GenerateGlyph(Glyph& glyph, char32_t character, float outlineThickness, bool lineWrap, Nz::Color color, int renderOrder, int* advance) const;
			void GenerateGlyphs(const String& text) const;
			void OnFontAtlasLayerChanged(const Font* font, AbstractImage* oldLayer, AbstractImage* newLayer);
			void OnFontInvalidated(const Font* font);
			void OnFontRelease(const Font* object);
			bool ShouldLineWrap(Glyph& glyph, float size, bool checkFirstGlyph = true) const;
			void UpdateGlyphColor() const;
			void UpdateGlyphs() const;

			static constexpr std::size_t InvalidGlyph = std::numeric_limits<std::size_t>::max();

			NazaraSlot(Font, OnFontAtlasChanged, m_atlasChangedSlot);
			NazaraSlot(Font, OnFontAtlasLayerChanged, m_atlasLayerChangedSlot);
			NazaraSlot(Font, OnFontGlyphCacheCleared, m_glyphCacheClearedSlot);
			NazaraSlot(Font, OnFontRelease, m_fontReleaseSlot);

			mutable std::size_t m_lastSeparatorGlyph;
			mutable unsigned int m_lastSeparatorPosition;
			mutable std::vector<Glyph> m_glyphs;
			mutable std::vector<Line> m_lines;
			Color m_color;
			Color m_outlineColor;
			FontRef m_font;
			mutable Rectf m_workingBounds;
			mutable Recti m_bounds;
			String m_text;
			TextStyleFlags m_style;
			mutable UInt32 m_previousCharacter;
			mutable Vector2ui m_drawPos;
			mutable bool m_colorUpdated;
			mutable bool m_glyphUpdated;
			float m_maxLineWidth;
			float m_outlineThickness;
			unsigned int m_characterSize;
	};
}

#endif // NAZARA_SIMPLETEXTDRAWER_HPP
