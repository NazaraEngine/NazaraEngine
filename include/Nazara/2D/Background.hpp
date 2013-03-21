// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - 2D Module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_BACKGROUND_HPP
#define NAZARA_BACKGROUND_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/2D/Enums.hpp>

class NAZARA_API NzBackground
{
	public:
		NzBackground() = default;
		~NzBackground();

		virtual void Draw() const = 0;

		virtual nzBackgroundType GetBackgroundType() const = 0;
};

#endif // NAZARA_BACKGROUND_HPP
