// Copyright (C) 2017 Rémi Bèges
// This file is part of the "Nazara Engine - Noise module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#ifndef NAZARA_FBM_HPP
#define NAZARA_FBM_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Noise/MixerBase.hpp>

namespace Nz
{
	class NAZARA_NOISE_API FBM : public MixerBase
	{
		public:
			FBM(const NoiseBase& source);
			FBM(const FBM&) = delete;
			~FBM() = default;

			float Get(float x, float y, float scale) const override;
			float Get(float x, float y, float z, float scale) const override;
			float Get(float x, float y, float z, float w, float scale) const override;

			FBM& operator=(const FBM&) = delete;

		private:
			const NoiseBase& m_source;
	};
}

#endif // NAZARA_FBM_HPP
