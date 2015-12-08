// Copyright (C) 2015 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef FBM4D_HPP
#define FBM4D_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Noise/ComplexNoiseBase.hpp>
#include <Nazara/Noise/Abstract4DNoise.hpp>

namespace Nz
{
	class NAZARA_NOISE_API FBM4D : public Abstract4DNoise, public ComplexNoiseBase
	{
		public:
			FBM4D(NoiseType source, unsigned int seed);
			float GetValue(float x, float y, float z, float w, float resolution);
			~FBM4D();

		private:
			Abstract4DNoise* m_source;
			float m_value;
			float m_remainder;
			NoiseType m_noiseType;
	};
}

#endif // FBM4D_HPP

