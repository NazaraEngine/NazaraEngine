// Copyright (C) 2015 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ABSTRACT2DNOISE_HPP
#define NAZARA_ABSTRACT2DNOISE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Noise/MappedNoiseBase.hpp>

namespace Nz
{
	class NAZARA_NOISE_API Abstract2DNoise : public MappedNoiseBase
	{
		public:
			virtual ~Abstract2DNoise();

			float GetBasicValue(float x, float y);
			float GetMappedValue(float x, float y);
			virtual float GetValue(float x, float y, float resolution) = 0;
	};
}

#endif // NAZARA_ABSTRACT2DNOISE_HPP
