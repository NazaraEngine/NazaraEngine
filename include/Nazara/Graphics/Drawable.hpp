// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DRAWABLE_HPP
#define NAZARA_DRAWABLE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/Config.hpp>

namespace Nz
{
	class NAZARA_GRAPHICS_API Drawable
	{
		public:
			Drawable() = default;
			virtual ~Drawable();

			virtual void Draw() const = 0;
	};
}

#endif // NAZARA_DRAWABLE_HPP
