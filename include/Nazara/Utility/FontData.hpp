// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FONTDATA_HPP
#define NAZARA_FONTDATA_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Utility/Config.hpp>

namespace Nz
{
	struct FontGlyph;

	class NAZARA_UTILITY_API FontData
	{
		public:
			FontData() = default;
			virtual ~FontData();

			virtual bool ExtractGlyph(unsigned int characterSize, char32_t character, UInt32 style, FontGlyph* dst) = 0;

			virtual String GetFamilyName() const = 0;
			virtual String GetStyleName() const = 0;

			virtual bool HasKerning() const = 0;

			virtual bool IsScalable() const = 0;

			virtual int QueryKerning(unsigned int characterSize, char32_t first, char32_t second) const = 0;
			virtual unsigned int QueryLineHeight(unsigned int characterSize) const = 0;
			virtual float QueryUnderlinePosition(unsigned int characterSize) const = 0;
			virtual float QueryUnderlineThickness(unsigned int characterSize) const = 0;

			virtual bool SupportsStyle(UInt32 style) const = 0;
	};
}

#endif // NAZARA_FONTDATA_HPP
