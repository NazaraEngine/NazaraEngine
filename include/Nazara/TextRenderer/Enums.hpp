// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Text renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_TEXTRENDERER_ENUMS_HPP
#define NAZARA_TEXTRENDERER_ENUMS_HPP

#include <NazaraUtils/Flags.hpp>

namespace Nz
{
	enum class TextAlign
	{
		Left,
		Middle,
		Right,

		Max = Right
	};

	enum class TextStyle
	{
		Bold,
		Italic,
		OutlineOnly,
		StrikeThrough,
		Underlined,

		Max = Underlined
	};

	template<>
	struct EnumAsFlags<TextStyle>
	{
		static constexpr TextStyle max = TextStyle::Max;
	};

	using TextStyleFlags = Flags<TextStyle>;

	constexpr TextStyleFlags TextStyle_Regular = TextStyleFlags{};
}

#endif // NAZARA_TEXTRENDERER_ENUMS_HPP
