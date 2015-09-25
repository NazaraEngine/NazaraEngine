// Copyright (C) 2015 Rémi Bèges
// This file is part of the "Nazara Engine - Noise module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef SIMPLEX3D_HPP
#define SIMPLEX3D_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Noise/NoiseBase.hpp>
#include <Nazara/Noise/Abstract3DNoise.hpp>
#include <Nazara/Math/Vector3.hpp>

namespace Nz
{
	class NAZARA_NOISE_API Simplex3D : public Abstract3DNoise
	{
		public:
			Simplex3D();
			Simplex3D(unsigned int seed);
			~Simplex3D() = default;

			float GetValue(float x, float y, float z, float resolution);

		private:
			int ii,jj,kk;
			int gi0,gi1,gi2,gi3;
			Vector3i skewedCubeOrigin,off1,off2;
			float n1,n2,n3,n4;
			float c1,c2,c3,c4;
			float gradient3[12][3];
			float UnskewCoeff3D;
			float SkewCoeff3D;
			float sum;
			Vector3<float> unskewedCubeOrigin, unskewedDistToOrigin;
			Vector3<float> d1,d2,d3,d4;
	};
}

#endif // SIMPLEX3D_HPP

