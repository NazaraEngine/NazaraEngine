// Copyright (C) 2015 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef HYBRIDMULTIFRACTAL4D_HPP
#define HYBRIDMULTIFRACTAL4D_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Noise/ComplexNoiseBase.hpp>
#include <Nazara/Noise/Abstract4DNoise.hpp>

namespace Nz
{
	class NAZARA_NOISE_API HybridMultiFractal4D : public Abstract4DNoise, public ComplexNoiseBase
	{
		public:
			HybridMultiFractal4D(NoiseType source, unsigned int seed);
			~HybridMultiFractal4D();

			float GetValue(float x, float y, float z, float w, float resolution);

		private:
			Abstract4DNoise* m_source;
			float m_value;
			float m_remainder;
			float m_offset;
			float m_weight;
			float m_signal;
			NoiseType m_noiseType;
	};
}

#endif // HYBRIDMULTIFRACTAL4D_HPP

