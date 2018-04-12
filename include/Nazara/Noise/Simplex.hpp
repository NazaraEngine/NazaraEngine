// Copyright (C) 2017 Rémi Bèges
// This file is part of the "Nazara Engine - Noise module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#ifndef NAZARA_SIMPLEX_HPP
#define NAZARA_SIMPLEX_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Noise/Config.hpp>
#include <Nazara/Noise/NoiseBase.hpp>

namespace Nz
{
	class NAZARA_NOISE_API Simplex : public NoiseBase
	{
		public:
			Simplex() = default;
			Simplex(unsigned int seed);
			~Simplex() = default;

			float Get(float x, float y, float scale) const override;
			float Get(float x, float y, float z, float scale) const override;
			float Get(float x, float y, float z, float w, float scale) const override;
	};
}

#endif // NAZARA_SIMPLEX_HPP
