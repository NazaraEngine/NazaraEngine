// Copyright (C) 2015 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef FBM3D_HPP
#define FBM3D_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Noise/ComplexNoiseBase.hpp>
#include <Nazara/Noise/Abstract3DNoise.hpp>

namespace Nz
{
	class NAZARA_NOISE_API FBM3D : public Abstract3DNoise, public ComplexNoiseBase
	{
		public:
			FBM3D(NoiseType source, unsigned int seed);
			float GetValue(float x, float y, float z, float resolution);
			~FBM3D();

		private:
			Abstract3DNoise* m_source;
			float m_value;
			float m_remainder;
			NoiseType m_noiseType;
	};
}

#endif // FBM3D_HPP

