// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NDK/Systems/ParticleSystem.hpp>
#include <NDK/Components/ParticleGroupComponent.hpp>

namespace Ndk
{
	/*!
	* \ingroup NDK
	* \class Ndk::ParticleSystem
	* \brief NDK class that represents the particle system
	*
	* \remark This system is enabled if the entity has the trait: NodeComponent and any of these two: ParticleGroupComponent
	*/

	/*!
	* \brief Constructs an ParticleSystem object by default
	*/

	ParticleSystem::ParticleSystem()
	{
		Requires<ParticleGroupComponent>();
	}

	/*!
	* \brief Operation to perform when system is updated
	*
	* \param elapsedTime Delta time used for the update
	*/

	void ParticleSystem::OnUpdate(float elapsedTime)
	{
		for (const Ndk::EntityHandle& entity : GetEntities())
		{
			ParticleGroupComponent& group = entity->GetComponent<ParticleGroupComponent>();

			group.Update(elapsedTime);
		}
	}

	SystemIndex ParticleSystem::systemIndex;
}
