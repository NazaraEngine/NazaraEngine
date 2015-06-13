// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DRAWABLE_HPP
#define NAZARA_DRAWABLE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/Config.hpp>

class NAZARA_GRAPHICS_API NzDrawable
{
	public:
		NzDrawable() = default;
		virtual ~NzDrawable();

		virtual void Draw() const = 0;
};

#endif // NAZARA_DRAWABLE_HPP
