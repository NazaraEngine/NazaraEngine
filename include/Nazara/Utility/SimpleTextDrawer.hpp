// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SIMPLETEXTDRAWER_HPP
#define NAZARA_SIMPLETEXTDRAWER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Utility/AbstractTextDrawer.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <Nazara/Utility/Font.hpp>
#include <vector>
#include <deque>

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

			void Clear();

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
			UInt32 GetStyle() const;
			const String& GetText() const;

			void SetCharacterSize(unsigned int characterSize);
			void SetColor(const Color& color);
			void SetFont(Font* font);
			void SetStyle(UInt32 style);
			void SetText(const String& str);
			void SetSize(const Vector2f&);

			SimpleTextDrawer& operator=(const SimpleTextDrawer& drawer);
			SimpleTextDrawer& operator=(SimpleTextDrawer&& drawer);

			static SimpleTextDrawer Draw(const String& str, unsigned int characterSize, UInt32 style = TextStyle_Regular, const Color& color = Color::White);
			static SimpleTextDrawer Draw(Font* font, const String& str, unsigned int characterSize, UInt32 style = TextStyle_Regular, const Color& color = Color::White);

		private:
			void ClearGlyphs() const;
			void ConnectFontSlots();
			void DisconnectFontSlots();
			void GenerateGlyphs(const String& text, size_t index = 0) const;
			void OnFontAtlasLayerChanged(const Font* font, AbstractImage* oldLayer, AbstractImage* newLayer);
			void OnFontInvalidated(const Font* font);
			void OnFontRelease(const Font* object);
			void UpdateGlyphColor() const;
			void UpdateGlyphs() const;

			NazaraSlot(Font, OnFontAtlasChanged, m_atlasChangedSlot);
			NazaraSlot(Font, OnFontAtlasLayerChanged, m_atlasLayerChangedSlot);
			NazaraSlot(Font, OnFontGlyphCacheCleared, m_glyphCacheClearedSlot);
			NazaraSlot(Font, OnFontRelease, m_fontReleaseSlot);

			mutable std::vector<Glyph> m_glyphs;
			mutable std::vector<Line> m_lines;
			Color m_color;
			FontRef m_font;
			mutable Rectf m_workingBounds;
			mutable Recti m_bounds;
			String m_text;
			mutable UInt32 m_previousCharacter;
			UInt32 m_style;
			mutable Vector2ui m_drawPos;
			mutable bool m_colorUpdated;
			mutable bool m_glyphUpdated;
			unsigned int m_characterSize;
			Vector2f m_size;
	};
}

#endif // NAZARA_SIMPLETEXTDRAWER_HPP
