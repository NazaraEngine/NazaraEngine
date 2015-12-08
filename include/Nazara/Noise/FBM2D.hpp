// Copyright (C) 2015 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef FBM2D_HPP
#define FBM2D_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Noise/ComplexNoiseBase.hpp>
#include <Nazara/Noise/Abstract2DNoise.hpp>

namespace Nz
{
	class NAZARA_NOISE_API FBM2D : public Abstract2DNoise, public ComplexNoiseBase
	{
		public:
			FBM2D(NoiseType source, unsigned int seed);
			float GetValue(float x, float y, float resolution);
			~FBM2D();

		private:
			Abstract2DNoise* m_source;
			float m_value;
			float m_remainder;
			NoiseType m_noiseType;
	};
}

#endif // FBM2D_HPP
