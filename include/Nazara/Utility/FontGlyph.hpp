// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UTILITY_FONTGLYPH_HPP
#define NAZARA_UTILITY_FONTGLYPH_HPP

#include <Nazara/Utility/Image.hpp>

namespace Nz
{
	struct FontGlyph
	{
		Image image;
		Recti aabb;
		int advance;
	};
}

#endif // NAZARA_UTILITY_FONTGLYPH_HPP
