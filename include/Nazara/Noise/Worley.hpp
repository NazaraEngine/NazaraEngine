// Copyright (C) 2016 Rémi Bèges
// This file is part of the "Nazara Engine - Noise module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#ifndef NAZARA_WORLEY_HPP
#define NAZARA_WORLEY_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Noise/Config.hpp>
#include <Nazara/Noise/Enums.hpp>
#include <Nazara/Noise/NoiseBase.hpp>
#include <initializer_list>
#include <map>

namespace Nz
{
	class NAZARA_NOISE_API Worley : public NoiseBase
	{
		public:
		  Worley();
		  Worley(unsigned int seed);
		  ~Worley() = default;

		  void Set(WorleyFunction func);

		  float Get(std::initializer_list<float> coordinates, float scale) const;

		protected:
		  float _2D(std::initializer_list<float> coordinates, float scale) const;
		  float _3D(std::initializer_list<float> coordinates, float scale) const;
		  float _4D(std::initializer_list<float> coordinates, float scale) const;
		  void _SquareTest(int xi, int yi, float x, float y, std::map<float, Vector2f> & featurePoints) const;

		private:
		  const float scales[4];
		  WorleyFunction function;
	};
}

#endif // NAZARA_WORLEY_HPP
