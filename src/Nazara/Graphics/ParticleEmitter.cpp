// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ParticleEmitter.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Graphics/ParticleMapper.hpp>
#include <Nazara/Graphics/ParticleGroup.hpp>
#include <cstdlib>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup graphics
	* \class Nz::ParticleEmitter
	* \brief Graphics class that represents an emitter of particles
	*/

	/*!
	* \brief Constructs a ParticleEmitter object by default
	*/

	ParticleEmitter::ParticleEmitter() :
	m_lagCompensationEnabled(false),
	m_emissionAccumulator(0.f),
	m_emissionRate(0.f),
	m_emissionCount(1)
	{
	}

	ParticleEmitter::ParticleEmitter(const ParticleEmitter& emitter) :
	m_lagCompensationEnabled(emitter.m_lagCompensationEnabled),
	m_emissionAccumulator(0.f),
	m_emissionRate(emitter.m_emissionRate),
	m_emissionCount(emitter.m_emissionCount)
	{
	}

	ParticleEmitter::ParticleEmitter(ParticleEmitter&& emitter) :
	m_lagCompensationEnabled(emitter.m_lagCompensationEnabled),
	m_emissionAccumulator(0.f),
	m_emissionRate(emitter.m_emissionRate),
	m_emissionCount(emitter.m_emissionCount)
	{
		OnParticleEmitterMove(&emitter, this);
	}

	ParticleEmitter::~ParticleEmitter()
	{
		OnParticleEmitterRelease(this);
	}

	/*!
	* \brief Emits particles according to the delta time between the previous frame
	*
	* \param system Particle system to work on
	* \param elapsedTime Delta time between the previous frame
	*/

	void ParticleEmitter::Emit(ParticleGroup& system, float elapsedTime) const
	{
		if (m_emissionRate > 0.f)
		{
			// We accumulate the real part (to avoid that a high emission rate prevents particles to form)
			m_emissionAccumulator += elapsedTime * m_emissionRate;

			float emissionCount = std::floor(m_emissionAccumulator); // The number of emissions in this update
			m_emissionAccumulator -= emissionCount; // We get rid off the integer part

			if (emissionCount >= 1.f)
			{
				// We compute the maximum number of particles which can be emitted
				std::size_t emissionCountInt = static_cast<std::size_t>(emissionCount);
				std::size_t maxParticleCount = emissionCountInt * m_emissionCount;

				// We get the number of particles that we are able to create (depending on the free space)
				std::size_t particleCount = std::min(maxParticleCount, system.GetMaxParticleCount() - system.GetParticleCount());
				if (particleCount == 0)
					return;

				// And we emit our particles
				void* particles = system.GenerateParticles(particleCount);
				ParticleMapper mapper(particles, system.GetDeclaration());

				SetupParticles(mapper, particleCount);

				if (m_lagCompensationEnabled)
				{
					// We will now apply our controllers
					float invEmissionRate = 1.f / m_emissionRate;
					for (unsigned int i = 1; i <= emissionCountInt; ++i)
						system.ApplyControllers(mapper, std::min(m_emissionCount * i, particleCount), invEmissionRate);
				}
			}
		}
	}

	/*!
	* \brief Enables the lag compensation
	*
	* \param enable Should lag compensation be enabled
	*/

	void ParticleEmitter::EnableLagCompensation(bool enable)
	{
		m_lagCompensationEnabled = enable;
	}

	/*!
	* \brief Gets the emission count
	* \return Current emission count
	*/

	std::size_t ParticleEmitter::GetEmissionCount() const
	{
		return m_emissionCount;
	}

	/*!
	* \brief Gets the emission rate
	* \return Current emission rate
	*/

	float ParticleEmitter::GetEmissionRate() const
	{
		return m_emissionRate;
	}

	/*!
	* \brief Checks whether the lag compensation is enabled
	* \return true If it is the case
	*/

	bool ParticleEmitter::IsLagCompensationEnabled() const
	{
		return m_lagCompensationEnabled;
	}

	/*!
	* \brief Sets the emission count
	*
	* \param count Emission count
	*/

	void ParticleEmitter::SetEmissionCount(std::size_t count)
	{
		m_emissionCount = count;
	}

	/*!
	* \brief Sets the emission rate
	*
	* \param rate Emission rate
	*/

	void ParticleEmitter::SetEmissionRate(float rate)
	{
		m_emissionRate = rate;
	}

	ParticleEmitter& ParticleEmitter::operator=(ParticleEmitter && emitter)
	{
		m_emissionCount = emitter.m_emissionCount;
		m_emissionRate = emitter.m_emissionRate;
		m_lagCompensationEnabled = emitter.m_lagCompensationEnabled;

		OnParticleEmitterMove(&emitter, this);
		return *this;
	}
}
