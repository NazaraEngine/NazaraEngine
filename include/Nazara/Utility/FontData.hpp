// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FONTDATA_HPP
#define NAZARA_FONTDATA_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/String.hpp>

struct NzFontGlyph;

class NAZARA_API NzFontData
{
	public:
		NzFontData() = default;
		virtual ~NzFontData();

		virtual bool ExtractGlyph(unsigned int characterSize, char32_t character, nzUInt32 style, NzFontGlyph* dst) = 0;

		virtual NzString GetFamilyName() const = 0;
		virtual NzString GetStyleName() const = 0;

		virtual bool HasKerning() const = 0;

		virtual bool IsScalable() const = 0;

		virtual int QueryKerning(unsigned int characterSize, char32_t first, char32_t second) const = 0;
		virtual unsigned int QueryLineHeight(unsigned int characterSize) const = 0;
		virtual float QueryUnderlinePosition(unsigned int characterSize) const = 0;
		virtual float QueryUnderlineThickness(unsigned int characterSize) const = 0;

		virtual bool SupportsStyle(nzUInt32 style) const = 0;
};

#endif // NAZARA_FONTDATA_HPP
