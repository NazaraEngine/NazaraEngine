// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ABSTRACTBACKGROUND_HPP
#define NAZARA_ABSTRACTBACKGROUND_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/Enums.hpp>

class NzAbstractViewer;

class NAZARA_GRAPHICS_API NzAbstractBackground
{
	public:
		NzAbstractBackground() = default;
		virtual ~NzAbstractBackground();

		virtual void Draw(const NzAbstractViewer* viewer) const = 0;

		virtual nzBackgroundType GetBackgroundType() const = 0;
};

#endif // NAZARA_ABSTRACTBACKGROUND_HPP
