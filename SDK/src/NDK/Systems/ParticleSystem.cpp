// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Systems/ParticleSystem.hpp>
#include <NDK/Components/ParticleGroupComponent.hpp>

namespace Ndk
{
	ParticleSystem::ParticleSystem()
	{
		Requires<ParticleGroupComponent>();
	}

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
