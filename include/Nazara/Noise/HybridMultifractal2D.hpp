// Copyright (C) 2015 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef HYBRIDMULTIFRACTAL2D_HPP
#define HYBRIDMULTIFRACTAL2D_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Noise/ComplexNoiseBase.hpp>
#include <Nazara/Noise/Abstract2DNoise.hpp>

namespace Nz
{
	class NAZARA_NOISE_API HybridMultiFractal2D : public Abstract2DNoise, public ComplexNoiseBase
	{
		public:
			HybridMultiFractal2D(NoiseType source, unsigned int seed);
			~HybridMultiFractal2D();

			float GetValue(float x, float y, float resolution);

		private:
			Abstract2DNoise* m_source;
			float m_value;
			float m_remainder;
			float m_offset;
			float m_weight;
			float m_signal;
			NoiseType m_noiseType;
	};
}

#endif // HYBRIDMULTIFRACTAL2D_HPP

