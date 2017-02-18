// Copyright (C) 2017 Rémi Bèges
// This file is part of the "Nazara Engine - Noise module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#ifndef NAZARA_MIXERBASE_HPP
#define NAZARA_MIXERBASE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Noise/NoiseBase.hpp>
#include <array>

namespace Nz
{
	class NAZARA_NOISE_API MixerBase
	{
		public:
			MixerBase();
			~MixerBase() = default;

			virtual float Get(float x, float y, float scale) const = 0;
			virtual float Get(float x, float y, float z, float scale) const = 0;
			virtual float Get(float x, float y, float z, float w, float scale) const = 0;

			float GetHurstParameter() const;
			float GetLacunarity() const;
			float GetOctaveNumber() const;

			void SetParameters(float hurst, float lacunarity, float octaves);

		protected:
			float m_hurst;
			float m_lacunarity;
			float m_octaves;
			std::vector<float> m_exponent_array;
			float m_sum;

		private:
			void Recompute();
	};
}

#endif // NAZARA_MIXERBASE_HPP
