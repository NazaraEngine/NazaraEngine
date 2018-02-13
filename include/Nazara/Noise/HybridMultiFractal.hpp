// Copyright (C) 2017 Rémi Bèges
// This file is part of the "Nazara Engine - Noise module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#ifndef NAZARA_HYBRIDMULTIFRACTAL_HPP
#define NAZARA_HYBRIDMULTIFRACTAL_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Noise/MixerBase.hpp>

namespace Nz
{
	class NAZARA_NOISE_API HybridMultiFractal : public MixerBase
	{
		public:
			HybridMultiFractal(const NoiseBase & source);
			HybridMultiFractal(const HybridMultiFractal&) = delete;
			~HybridMultiFractal() = default;

			float Get(float x, float y, float scale) const override;
			float Get(float x, float y, float z, float scale) const override;
			float Get(float x, float y, float z, float w, float scale) const override;

			HybridMultiFractal& operator=(const HybridMultiFractal&) = delete;

		private:
			const NoiseBase& m_source;
	};
}

#endif // NAZARA_HYBRIDMULTIFRACTAL_HPP
