// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Text renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TEXTRENDERER_ABSTRACTTEXTDRAWER_HPP
#define NAZARA_TEXTRENDERER_ABSTRACTTEXTDRAWER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/TextRenderer/Config.hpp>
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

			AbstractTextDrawer() = default;
			virtual ~AbstractTextDrawer();

			virtual void Clear() = 0;

			virtual const Rectf& GetBounds() const = 0;
			virtual const std::shared_ptr<Font>& GetFont(std::size_t index) const = 0;
			virtual std::size_t GetFontCount() const = 0;
			virtual const Glyph& GetGlyph(std::size_t index) const = 0;
			virtual std::size_t GetGlyphCount() const = 0;
			virtual const Line& GetLine(std::size_t index) const = 0;
			virtual std::size_t GetLineCount() const = 0;
			inline std::size_t GetLineGlyphCount(std::size_t index) const;
			virtual float GetMaxLineWidth() const = 0;

			virtual void SetMaxLineWidth(float lineWidth) = 0;

			struct Glyph
			{
				Color color;
				Rectf bounds;
				Rectui atlasRect;
				Vector2f corners[4];
				AbstractImage* atlas;
				bool flipped;
				int renderOrder;
			};

			struct Line
			{
				Rectf bounds;
				std::size_t glyphIndex;
			};
	};
}

#include <Nazara/TextRenderer/AbstractTextDrawer.inl>

#endif // NAZARA_TEXTRENDERER_ABSTRACTTEXTDRAWER_HPP
