// Copyright (C) 2015 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef HYBRIDMULTIFRACTAL3D_HPP
#define HYBRIDMULTIFRACTAL3D_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Noise/ComplexNoiseBase.hpp>
#include <Nazara/Noise/Abstract3DNoise.hpp>

namespace Nz
{
	class NAZARA_NOISE_API HybridMultiFractal3D : public Abstract3DNoise, public ComplexNoiseBase
	{
		public:
			HybridMultiFractal3D(NoiseType source, unsigned int seed);
			~HybridMultiFractal3D();

			float GetValue(float x, float y, float z, float resolution);

		private:
			Abstract3DNoise* m_source;
			float m_value;
			float m_remainder;
			float m_offset;
			float m_weight;
			float m_signal;
			NoiseType m_noiseType;
	};
}

#endif // HYBRIDMULTIFRACTAL3D_HPP

