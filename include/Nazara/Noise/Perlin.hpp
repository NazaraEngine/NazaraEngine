// Copyright (C) 2017 Rémi Bèges
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
			Perlin() = default;
			Perlin(unsigned int seed);
			~Perlin() = default;

			float Get(float x, float y, float scale) const override;
			float Get(float x, float y, float z, float scale) const override;
			float Get(float x, float y, float z, float w, float scale) const override;
	};
}

#endif // NAZARA_PERLIN_HPP
