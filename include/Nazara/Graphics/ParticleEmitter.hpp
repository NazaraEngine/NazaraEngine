// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PARTICLEEMITTER_HPP
#define NAZARA_PARTICLEEMITTER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Graphics/Config.hpp>

namespace Nz
{
	class ParticleMapper;
	class ParticleGroup;

	class NAZARA_GRAPHICS_API ParticleEmitter
	{
		public:
			ParticleEmitter();
			ParticleEmitter(const ParticleEmitter& emitter);
			ParticleEmitter(ParticleEmitter&& emitter);
			virtual ~ParticleEmitter();

			virtual void Emit(ParticleGroup& system, float elapsedTime) const;

			void EnableLagCompensation(bool enable);

			std::size_t GetEmissionCount() const;
			float GetEmissionRate() const;

			bool IsLagCompensationEnabled() const;

			void SetEmissionCount(std::size_t count);
			void SetEmissionRate(float rate);

			ParticleEmitter& operator=(const ParticleEmitter& emitter) = default;
			ParticleEmitter& operator=(ParticleEmitter&& emitter);

			// Signals:
			NazaraSignal(OnParticleEmitterMove, ParticleEmitter* /*oldParticleEmitter*/, ParticleEmitter* /*newParticleEmitter*/);
			NazaraSignal(OnParticleEmitterRelease, const ParticleEmitter* /*particleEmitter*/);

		private:
			virtual void SetupParticles(ParticleMapper& mapper, unsigned int count) const = 0;

			bool m_lagCompensationEnabled;
			mutable float m_emissionAccumulator;
			float m_emissionRate;
			std::size_t m_emissionCount;
	};
}

#endif // NAZARA_PARTICLEEMITTER_HPP
