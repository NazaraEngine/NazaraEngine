// Copyright (C) 2015 Rémi Bèges
// This file is part of the "Nazara Engine - Noise module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef PERLIN2D_HPP
#define PERLIN2D_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Noise/NoiseBase.hpp>
#include <Nazara/Noise/Abstract2DNoise.hpp>
#include <Nazara/Math/Vector2.hpp>

namespace Nz
{
	class NAZARA_NOISE_API Perlin2D : public Abstract2DNoise
	{
		public:
			Perlin2D();
			Perlin2D(unsigned int seed);
			~Perlin2D() = default;

			float GetValue(float x, float y, float resolution);

		private:
			int x0, y0;
			int gi0,gi1,gi2,gi3;
			int ii, jj;
			float gradient2[8][2];
			float s,t,u,v;
			float Cx,Cy;
			float Li1, Li2;
			Vector2<float> temp;
	};
}

#endif // PERLIN2D_HPP

