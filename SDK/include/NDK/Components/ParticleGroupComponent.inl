// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Components/ParticleGroupComponent.hpp>
#include <NDK/Components/ParticleEmitterComponent.hpp>
#include <Nazara/Core/Error.hpp>

namespace Ndk
{
	/*!
	* \ingroup NDK
	* \class Ndk::ParticleGroupComponent
	* \brief NDK class that represents the component for a group of particles
	*/

	/*!
	* \brief Constructs a ParticleGroupComponent object with a maximal number of particles and a layout
	*
	* \param maxParticleCount Maximum number of particles to generate
	* \param layout Enumeration for the layout of data information for the particles
	*/

	inline ParticleGroupComponent::ParticleGroupComponent(unsigned int maxParticleCount, Nz::ParticleLayout layout) :
	ParticleGroup(maxParticleCount, layout)
	{
	}

	/*!
	* \brief Constructs a ParticleGroupComponent object with a maximal number of particles and a particle declaration
	*
	* \param maxParticleCount Maximum number of particles to generate
	* \param declaration Data information for the particles
	*/

	inline ParticleGroupComponent::ParticleGroupComponent(unsigned int maxParticleCount, Nz::ParticleDeclarationConstRef declaration) :
	ParticleGroup(maxParticleCount, std::move(declaration))
	{
	}

	/*!
	* \brief Adds an emitter to the particles
	*
	* \param emitter Emitter for the particles
	*
	* \remark Produces a NazaraAssert if emitter is invalid
	* \remark Produces a NazaraAssert if entity has no component of type ParticleEmitterComponent
	*/

	inline void ParticleGroupComponent::AddEmitter(Entity* emitter)
	{
		NazaraAssert(emitter && emitter->IsValid(), "Invalid entity");
		NazaraAssert(emitter->HasComponent<ParticleEmitterComponent>(), "Entity must have a NodeComponent");

		auto& emitterComponent = emitter->GetComponent<ParticleEmitterComponent>();
		ParticleGroup::AddEmitter(&emitterComponent);
	}


	/*!
	* \brief Removes an emitter to the particles
	*
	* \param emitter Emitter for the particles to remove
	*
	* \remark Produces a NazaraAssert if emitter is invalid
	* \remark Produces a NazaraAssert if entity has no component of type ParticleEmitterComponent
	*/

	inline void ParticleGroupComponent::RemoveEmitter(Entity* emitter)
	{
		NazaraAssert(emitter && emitter->IsValid(), "Invalid entity");
		NazaraAssert(emitter->HasComponent<ParticleEmitterComponent>(), "Entity must have a NodeComponent");

		auto& emitterComponent = emitter->GetComponent<ParticleEmitterComponent>();
		ParticleGroup::RemoveEmitter(&emitterComponent);
	}
}
