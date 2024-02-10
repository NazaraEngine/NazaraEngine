// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Text renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TEXTRENDERER_FONTDATA_HPP
#define NAZARA_TEXTRENDERER_FONTDATA_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/TextRenderer/Config.hpp>
#include <string>

namespace Nz
{
	struct FontGlyph;

	class NAZARA_TEXTRENDERER_API FontData
	{
		public:
			FontData() = default;
			virtual ~FontData();

			virtual bool ExtractGlyph(unsigned int characterSize, char32_t character, TextStyleFlags style, float outlineThickness, FontGlyph* dst) = 0;

			virtual std::string GetFamilyName() const = 0;
			virtual std::string GetStyleName() const = 0;

			virtual bool HasKerning() const = 0;

			virtual bool IsScalable() const = 0;

			virtual int QueryKerning(unsigned int characterSize, char32_t first, char32_t second) const = 0;
			virtual unsigned int QueryLineHeight(unsigned int characterSize) const = 0;
			virtual float QueryUnderlinePosition(unsigned int characterSize) const = 0;
			virtual float QueryUnderlineThickness(unsigned int characterSize) const = 0;

			virtual bool SupportsOutline(float outlineThickness) const = 0;
			virtual bool SupportsStyle(TextStyleFlags style) const = 0;
	};
}

#endif // NAZARA_TEXTRENDERER_FONTDATA_HPP
