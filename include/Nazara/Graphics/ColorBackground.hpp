// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_COLORBACKGROUND_HPP
#define NAZARA_COLORBACKGROUND_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Graphics/AbstractBackground.hpp>

class NAZARA_API NzColorBackground : public NzAbstractBackground
{
	public:
		NzColorBackground(const NzColor& color = NzColor::Black);

		void Draw(const NzScene* scene) const;

		nzBackgroundType GetBackgroundType() const;
		NzColor GetColor() const;

		void SetColor(const NzColor& color);

	private:
		NzColor m_color;
};

#endif // NAZARA_COLORBACKGROUND_HPP
