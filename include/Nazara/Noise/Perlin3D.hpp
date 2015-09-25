// Copyright (C) 2015 Rémi Bèges
// This file is part of the "Nazara Engine - Noise module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef PERLIN3D_HPP
#define PERLIN3D_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Noise/NoiseBase.hpp>
#include <Nazara/Noise/Abstract3DNoise.hpp>
#include <Nazara/Math/Vector3.hpp>

namespace Nz
{
	class NAZARA_NOISE_API Perlin3D : public Abstract3DNoise
	{
		public:
			Perlin3D();
			Perlin3D(unsigned int seed);
			~Perlin3D() = default;

			float GetValue(float x, float y, float z, float resolution);

		private:
			int x0,y0,z0;
			int gi0,gi1,gi2,gi3,gi4,gi5,gi6,gi7;
			int ii,jj,kk;
			float gradient3[16][3];
			float Li1,Li2,Li3,Li4,Li5,Li6;
			float s[2],t[2],u[2],v[2];
			float Cx,Cy,Cz;
			float nx,ny,nz;
			float tmp;
			Vector3<float> temp;
	};
}

#endif // PERLIN3D_HPP
