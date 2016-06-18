// Copyright (C) 2016 Rémi Bèges
// This file is part of the "Nazara Engine - Noise module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#ifndef NAZARA_NOISEBASE_HPP
#define NAZARA_NOISEBASE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Noise/Config.hpp>
#include <random>

namespace Nz
{
	class NAZARA_NOISE_API NoiseBase
	{
		public:
			NoiseBase(unsigned int seed = 0);
			~NoiseBase() = default;

			virtual float Get(std::initializer_list<float> coordinates, float scale) const = 0;
			float GetScale();

			void SetScale(float scale);
			void SetSeed(unsigned int seed);

			void Shuffle();
			void Shuffle(unsigned int amount);

		protected:
			unsigned int perm[512];
			float m_scale;

		private:
			std::default_random_engine generator;
	};
}

#endif // NAZARA_NOISEBASE_HPP
