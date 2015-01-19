// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PARTICLEEMITTER_HPP
#define NAZARA_PARTICLEEMITTER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Utility/Node.hpp>

class NzParticleMapper;
class NzParticleSystem;

class NAZARA_API NzParticleEmitter : public NzNode
{
	public:
		NzParticleEmitter();
		NzParticleEmitter(const NzParticleEmitter& emitter) = default;
		NzParticleEmitter(NzParticleEmitter&& emitter) = default;
		virtual ~NzParticleEmitter();

		virtual void Emit(NzParticleSystem& system, float elapsedTime) const;

		unsigned int GetEmissionCount() const;
		float GetEmissionRate() const;

		void SetEmissionCount(unsigned int count);
		void SetEmissionRate(float rate);

		NzParticleEmitter& operator=(const NzParticleEmitter& emitter) = default;
		NzParticleEmitter& operator=(NzParticleEmitter&& emitter) = default;

	private:
		virtual void SetupParticles(NzParticleMapper& mapper, unsigned int count) const = 0;

		mutable float m_emissionAccumulator;
		float m_emissionRate;
		unsigned int m_emissionCount;
};

#endif // NAZARA_PARTICLEEMITTER_HPP
