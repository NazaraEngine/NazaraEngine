// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ABSTRACTTEXTDRAWER_HPP
#define NAZARA_ABSTRACTTEXTDRAWER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Utility/Config.hpp>

namespace Nz
{
	class AbstractImage;
	class Font;

	class NAZARA_UTILITY_API AbstractTextDrawer
	{
		public:
			struct Glyph;
			struct Line;

			AbstractTextDrawer() = default;
			virtual ~AbstractTextDrawer();

			virtual const Recti& GetBounds() const = 0;
			virtual Font* GetFont(std::size_t index) const = 0;
			virtual std::size_t GetFontCount() const = 0;
			virtual const Glyph& GetGlyph(std::size_t index) const = 0;
			virtual std::size_t GetGlyphCount() const = 0;
			virtual const Line& GetLine(std::size_t index) const = 0;
			virtual std::size_t GetLineCount() const = 0;

			struct Glyph
			{
				Color color;
				Rectf bounds;
				Rectui atlasRect;
				Vector2f corners[4];
				AbstractImage* atlas;
				bool flipped;
			};

			struct Line
			{
				Rectf bounds;
				std::size_t glyphIndex;
				std::size_t glyphCount;
			};
	};
}

#endif // NAZARA_ABSTRACTTEXTDRAWER_HPP
