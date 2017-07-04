// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FONTGLYPH_HPP
#define NAZARA_FONTGLYPH_HPP

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

#endif // NAZARA_FONTGLYPH_HPP
