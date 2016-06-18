// Copyright (C) 2016 Rémi Bèges
// This file is part of the "Nazara Engine - Noise module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#ifndef SIMPLEX_HPP
#define SIMPLE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Noise/Config.hpp>
#include <Nazara/Noise/NoiseBase.hpp>
#include <initializer_list>

namespace Nz
{
	class NAZARA_NOISE_API Simplex : public NoiseBase
	{
		public:
		  Simplex();
		  Simplex(unsigned int seed);
		  ~Simplex() = default;

		  float Get(std::initializer_list<float> coordinates, float scale) const;

		protected:
		  float _2D(std::initializer_list<float> coordinates, float scale) const;
		  float _3D(std::initializer_list<float> coordinates, float scale) const;
		  float _4D(std::initializer_list<float> coordinates, float scale) const;

		private:
		  const float gradient2[8][2];
		  const float gradient3[16][3];
		  const float gradient4[32][4];
		  const float UnskewCoeff2D;
		  const float SkewCoeff2D;
		  const float UnskewCoeff3D;
		  const float SkewCoeff3D;
		  const float UnskewCoeff4D;
		  const float SkewCoeff4D;
		  const int lookupTable4D[64][4];
	};
}

#endif // SIMPLEX_HPP
