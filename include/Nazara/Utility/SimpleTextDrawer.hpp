// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SIMPLETEXTDRAWER_HPP
#define NAZARA_SIMPLETEXTDRAWER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/ObjectListener.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Utility/AbstractTextDrawer.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <Nazara/Utility/Font.hpp>
#include <vector>

class NAZARA_API NzSimpleTextDrawer : public NzAbstractTextDrawer, NzObjectListener
{
	public:
		NzSimpleTextDrawer();
		virtual ~NzSimpleTextDrawer() = default;

		const NzRectui& GetBounds() const;
		unsigned int GetCharacterSize() const;
		const NzColor& GetColor() const;
		NzFont* GetFont() const;
		NzFont* GetFont(unsigned int index) const override;
		unsigned int GetFontCount() const override;
		const Glyph& GetGlyph(unsigned int index) const override;
		unsigned int GetGlyphCount() const override;
		nzUInt32 GetStyle() const;
		const NzString& GetText() const;

		void SetCharacterSize(unsigned int characterSize);
		void SetColor(const NzColor& color);
		void SetFont(NzFont* font);
		void SetStyle(nzUInt32 style);
		void SetText(const NzString& str);

		static NzSimpleTextDrawer Draw(const NzString& str, unsigned int characterSize, nzUInt32 style = nzTextStyle_Regular, const NzColor& color = NzColor::White);
		static NzSimpleTextDrawer Draw(NzFont* font, const NzString& str, unsigned int characterSize, nzUInt32 style = nzTextStyle_Regular, const NzColor& color = NzColor::White);

	private:
		bool OnObjectModified(const NzRefCounted* object, int index, unsigned int code) override;
		void OnObjectReleased(const NzRefCounted* object, int index) override;
		void UpdateGlyphs() const;

		mutable std::vector<Glyph> m_glyphs;
		NzColor m_color;
		NzFontRef m_font;
		NzFontListener m_fontListener; // Doit se situer après le FontRef (pour être libéré avant)
		mutable NzRectui m_bounds;
		NzString m_text;
		nzUInt32 m_style;
		mutable bool m_glyphUpdated;
		unsigned int m_characterSize;
};

#endif // NAZARA_SIMPLETEXTDRAWER_HPP
