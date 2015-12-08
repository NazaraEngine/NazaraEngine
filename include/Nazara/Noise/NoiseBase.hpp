// Copyright (C) 2015 Rémi Bèges
// This file is part of the "Nazara Engine - Noise module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NOISEBASE_HPP
#define NOISEBASE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Noise/Config.hpp>

namespace Nz
{
	enum NoiseType
	{
		PERLIN,
		SIMPLEX,
		CELL
	};

	class NAZARA_NOISE_API NoiseBase
	{
		public:
			NoiseBase(unsigned int seed = 0);
			~NoiseBase() = default;

			void SetNewSeed(unsigned int seed);

			void ShufflePermutationTable();

			unsigned int GetUniformRandomValue();

			int fastfloor(float n);
			int JenkinsHash(int a, int b, int c);

		protected:
			unsigned int perm[512];

		private:
			unsigned int Ua, Uc, Um;
			unsigned int UcurrentSeed;
			unsigned int Uprevious, Ulast;

	};
}

#endif // NOISEBASE_HPP
