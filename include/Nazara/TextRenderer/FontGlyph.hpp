// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Text renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TEXTRENDERER_FONTGLYPH_HPP
#define NAZARA_TEXTRENDERER_FONTGLYPH_HPP

#include <Nazara/Core/Image.hpp>

namespace Nz
{
	struct FontGlyph
	{
		Image image;
		Recti aabb;
		int advance;
	};
}

#endif // NAZARA_TEXTRENDERER_FONTGLYPH_HPP
