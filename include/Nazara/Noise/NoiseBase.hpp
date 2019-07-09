// Copyright (C) 2017 Rémi Bèges
// This file is part of the "Nazara Engine - Noise module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#ifndef NAZARA_NOISEBASE_HPP
#define NAZARA_NOISEBASE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Math/Vector4.hpp>
#include <Nazara/Noise/Config.hpp>
#include <array>
#include <random>

namespace Nz
{
	class NAZARA_NOISE_API NoiseBase
	{
		public:
			NoiseBase(unsigned int seed = 0);
			virtual ~NoiseBase() = default;

			virtual float Get(float x, float y, float scale) const = 0;
			virtual float Get(float x, float y, float z, float scale) const = 0;
			virtual float Get(float x, float y, float z, float w, float scale) const = 0;
			float GetScale();

			void SetScale(float scale);
			void SetSeed(unsigned int seed);

			void Shuffle();

		protected:
			std::array<std::size_t, 3 * 256> m_permutations;
			float m_scale;

			static std::array<Vector2f, 2 * 2 * 2>         s_gradients2;
			static std::array<Vector3f, 2 * 2 * 2 * 2>     s_gradients3;
			static std::array<Vector4f, 2 * 2 * 2 * 2 * 2> s_gradients4;

		private:
			std::mt19937 m_randomEngine;
	};
}

#endif // NAZARA_NOISEBASE_HPP
