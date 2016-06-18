// Copyright (C) 2016 Rémi Bèges
// This file is part of the "Nazara Engine - Noise module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#ifndef NAZARA_PERLIN_HPP
#define NAZARA_PERLIN_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Noise/Config.hpp>
#include <Nazara/Noise/NoiseBase.hpp>
#include <initializer_list>

namespace Nz
{
	class NAZARA_NOISE_API Perlin : public NoiseBase
	{
		public:
		  Perlin();
		  Perlin(unsigned int seed);
		  ~Perlin() = default;

		  float Get(std::initializer_list<float> coordinates, float scale) const;

		protected:
		  float _2D(std::initializer_list<float> coordinates, float scale) const;
		  float _3D(std::initializer_list<float> coordinates, float scale) const;
		  float _4D(std::initializer_list<float> coordinates, float scale) const;

		private:
		  const float gradient2[8][2];
		  const float gradient3[16][3];
		  const float gradient4[32][4];
	};
}

#endif // NAZARA_PERLIN_HPP
