// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Text renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_TEXTRENDERER_ABSTRACTTEXTDRAWER_HPP
#define NAZARA_TEXTRENDERER_ABSTRACTTEXTDRAWER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/TextRenderer/Export.hpp>
#include <limits>
#include <memory>

namespace Nz
{
	class AbstractImage;
	class Font;

	class NAZARA_TEXTRENDERER_API AbstractTextDrawer
	{
		public:
			struct Glyph;
			struct Line;
			struct Sprite;

			AbstractTextDrawer() = default;
			virtual ~AbstractTextDrawer();

			virtual void Clear() = 0;

			virtual const Rectf& GetBounds() const = 0;
			virtual const std::shared_ptr<Font>& GetFont(std::size_t index) const = 0;
			virtual std::size_t GetFontCount() const = 0;
			virtual const Glyph* GetGlyphs() const = 0;
			virtual std::size_t GetGlyphCount() const = 0;
			virtual const Line* GetLines() const = 0;
			virtual std::size_t GetLineCount() const = 0;
			inline std::size_t GetLineGlyphCount(std::size_t index) const;
			virtual float GetMaxLineWidth() const = 0;
			virtual const Sprite* GetSprites() const = 0;
			virtual std::size_t GetSpriteCount() const = 0;

			virtual void SetMaxLineWidth(float lineWidth) = 0;

			struct Glyph
			{
				std::size_t outlineSpriteIndex = InvalidIndex;
				std::size_t spriteIndex = InvalidIndex;
				Rectf bounds;
			};

			struct Line
			{
				std::size_t glyphIndex;
				Rectf bounds;
				bool allowsOvershoot = true; //< to handle \n which shouldn't appear when placing the cursor at end of a line but still can be selected
			};

			struct Sprite
			{
				Color color;
				Rectui atlasRect;
				Vector2f corners[4];
				AbstractImage* atlas;
				bool flipped;
				int renderOrder;
			};

			static constexpr std::size_t InvalidIndex = std::numeric_limits<std::size_t>::max();
	};
}

#include <Nazara/TextRenderer/AbstractTextDrawer.inl>

#endif // NAZARA_TEXTRENDERER_ABSTRACTTEXTDRAWER_HPP
