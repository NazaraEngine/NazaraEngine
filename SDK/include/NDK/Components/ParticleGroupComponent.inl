// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Components/ParticleGroupComponent.hpp>
#include <NDK/Components/ParticleEmitterComponent.hpp>
#include <Nazara/Core/Error.hpp>

namespace Ndk
{
	inline ParticleGroupComponent::ParticleGroupComponent(unsigned int maxParticleCount, Nz::ParticleLayout layout) :
	ParticleGroup(maxParticleCount, layout)
	{
	}

	inline ParticleGroupComponent::ParticleGroupComponent(unsigned int maxParticleCount, Nz::ParticleDeclarationConstRef declaration) :
	ParticleGroup(maxParticleCount, std::move(declaration))
	{
	}

	inline void ParticleGroupComponent::AddEmitter(Entity* emitter)
	{
		NazaraAssert(emitter && emitter->IsValid(), "Invalid entity");
		NazaraAssert(emitter->HasComponent<ParticleEmitterComponent>(), "Entity must have a NodeComponent");

		auto& emitterComponent = emitter->GetComponent<ParticleEmitterComponent>();
		ParticleGroup::AddEmitter(&emitterComponent);
	}

	inline void ParticleGroupComponent::RemoveEmitter(Entity* emitter)
	{
		NazaraAssert(emitter && emitter->IsValid(), "Invalid entity");
		NazaraAssert(emitter->HasComponent<ParticleEmitterComponent>(), "Entity must have a NodeComponent");

		auto& emitterComponent = emitter->GetComponent<ParticleEmitterComponent>();
		ParticleGroup::RemoveEmitter(&emitterComponent);
	}
}
