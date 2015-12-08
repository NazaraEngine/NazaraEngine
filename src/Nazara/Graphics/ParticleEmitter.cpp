// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ParticleEmitter.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Graphics/ParticleMapper.hpp>
#include <Nazara/Graphics/ParticleSystem.hpp>
#include <cstdlib>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	ParticleEmitter::ParticleEmitter() :
	m_lagCompensationEnabled(false),
	m_emissionAccumulator(0.f),
	m_emissionRate(0.f),
	m_emissionCount(1)
	{
	}

	ParticleEmitter::~ParticleEmitter() = default;

	void ParticleEmitter::Emit(ParticleSystem& system, float elapsedTime) const
	{
		if (m_emissionRate > 0.f)
		{
			// On accumule la partie réelle (pour éviter qu'un taux d'update élevé empêche des particules de se former)
			m_emissionAccumulator += elapsedTime*m_emissionRate;

			float emissionCount = std::floor(m_emissionAccumulator); // Le nombre d'émissions de cette mise à jour
			m_emissionAccumulator -= emissionCount; // On enlève la partie entière

			if (emissionCount >= 1.f)
			{
				// On calcule le nombre maximum de particules pouvant être émises cette fois-ci
				unsigned int emissionCountInt = static_cast<unsigned int>(emissionCount);
				unsigned int maxParticleCount = emissionCountInt*m_emissionCount;

				// On récupère le nombre de particules qu'il est possible de créer selon l'espace libre
				unsigned int particleCount = std::min(maxParticleCount, system.GetMaxParticleCount() - system.GetParticleCount());
				if (particleCount == 0)
					return;

				// Et on émet nos particules
				void* particles = system.GenerateParticles(particleCount);
				ParticleMapper mapper(particles, system.GetDeclaration());

				SetupParticles(mapper, particleCount);

				if (m_lagCompensationEnabled)
				{
					// On va maintenant appliquer les contrôleurs
					float invEmissionRate = 1.f/m_emissionRate;
					for (unsigned int i = 1; i <= emissionCountInt; ++i)
						system.ApplyControllers(mapper, std::min(m_emissionCount*i, particleCount), invEmissionRate);
				}
			}
		}
	}

	void ParticleEmitter::EnableLagCompensation(bool enable)
	{
		m_lagCompensationEnabled = enable;
	}

	unsigned int ParticleEmitter::GetEmissionCount() const
	{
		return m_emissionCount;
	}

	float ParticleEmitter::GetEmissionRate() const
	{
		return m_emissionRate;
	}

	bool ParticleEmitter::IsLagCompensationEnabled() const
	{
		return m_lagCompensationEnabled;
	}

	void ParticleEmitter::SetEmissionCount(unsigned int count)
	{
		m_emissionCount = count;
	}

	void ParticleEmitter::SetEmissionRate(float rate)
	{
		m_emissionRate = rate;
	}
}
