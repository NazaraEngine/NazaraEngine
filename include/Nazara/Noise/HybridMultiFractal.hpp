// Copyright (C) 2016 Rémi Bèges
// This file is part of the "Nazara Engine - Noise module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#ifndef NAZARA_HYBRIDMULTIFRACTAL_HPP
#define NAZARA_HYBRIDMULTIFRACTAL_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Noise/MixerBase.hpp>

namespace Nz
{
	class NAZARA_NOISE_API HybridMultiFractal : public MixerBase
	{
		public:
			HybridMultiFractal(const NoiseBase & source);
			HybridMultiFractal(const HybridMultiFractal&) = delete;
			~HybridMultiFractal() = default;

			float Get(std::initializer_list<float> coordinates, float scale) const;

			HybridMultiFractal& operator=(const HybridMultiFractal&) = delete;

		private:
			const NoiseBase& m_source;
			float m_value;
			float m_remainder;
			float m_offset;
			float m_weight;
			float m_signal;
	};
}

#endif // NAZARA_HYBRIDMULTIFRACTAL_HPP
