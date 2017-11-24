// Copyright (C) 2017 Rémi Bèges
// This file is part of the "Nazara Engine - Noise module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#ifndef NAZARA_WORLEY_HPP
#define NAZARA_WORLEY_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Noise/Config.hpp>
#include <Nazara/Noise/Enums.hpp>
#include <Nazara/Noise/NoiseBase.hpp>
#include <map>

namespace Nz
{
	class NAZARA_NOISE_API Worley : public NoiseBase
	{
		public:
			Worley();
			Worley(unsigned int seed);
			~Worley() = default;

			float Get(float x, float y, float scale) const override;
			float Get(float x, float y, float z, float scale) const override;
			float Get(float x, float y, float z, float w, float scale) const override;

			void Set(WorleyFunction func);

		private:
			void SquareTest(int xi, int yi, float x, float y, std::map<float, Vector2f> & featurePoints) const;

			WorleyFunction m_function;
	};
}

#endif // NAZARA_WORLEY_HPP
