// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ABSTRACTTEXTDRAWER_HPP
#define NAZARA_ABSTRACTTEXTDRAWER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Math/Vector2.hpp>

class NzAbstractImage;
class NzFont;

class NAZARA_API NzAbstractTextDrawer
{
	public:
		struct Glyph;

		NzAbstractTextDrawer() = default;
		virtual ~NzAbstractTextDrawer();

		virtual const NzRectui& GetBounds() const = 0;
		virtual NzFont* GetFont(unsigned int index) const = 0;
		virtual unsigned int GetFontCount() const = 0;
		virtual const Glyph& GetGlyph(unsigned int index) const = 0;
		virtual unsigned int GetGlyphCount() const = 0;

		struct Glyph
		{
			NzColor color;
			NzRectui atlasRect;
			NzVector2f corners[4];
			NzAbstractImage* atlas;
			bool flipped;
		};
};

#endif // NAZARA_ABSTRACTTEXTDRAWER_HPP
