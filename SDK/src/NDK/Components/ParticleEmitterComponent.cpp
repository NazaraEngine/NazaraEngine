// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Components/ParticleEmitterComponent.hpp>
#include <Nazara/Graphics/ParticleGroup.hpp>

namespace Ndk
{
	ParticleEmitterComponent::ParticleEmitterComponent(Nz::ParticleGroup* group) :
	m_particleGroup(group),
	m_isActive(true)
	{
		if (m_particleGroup)
			m_particleGroup->AddEmitter(this);
	}

	ParticleEmitterComponent::ParticleEmitterComponent(const ParticleEmitterComponent& emitter) :
	m_particleGroup(emitter.m_particleGroup),
	m_isActive(emitter.m_isActive)
	{
		if (m_isActive)
			m_particleGroup->AddEmitter(this);
	}

	ParticleEmitterComponent::~ParticleEmitterComponent()
	{
		m_particleGroup->RemoveEmitter(this);
	}

	inline void Ndk::ParticleEmitterComponent::SetActive(bool active)
	{
		if (m_isActive != active)
		{
			if (active)
				m_particleGroup->AddEmitter(this);
			else
				m_particleGroup->RemoveEmitter(this);
		}
	}

	void ParticleEmitterComponent::SetGroup(Nz::ParticleGroup* group)
	{
		if (m_particleGroup)
			m_particleGroup->RemoveEmitter(this);

		m_particleGroup = group;
		if (m_particleGroup && m_isActive)
			m_particleGroup = group;
	}

	inline void ParticleEmitterComponent::SetupParticles(Nz::ParticleMapper& mapper, unsigned int count) const
	{
		if (m_setupFunc)
			m_setupFunc(m_entity, mapper, count);
	}

	ComponentIndex ParticleEmitterComponent::componentIndex;
}
