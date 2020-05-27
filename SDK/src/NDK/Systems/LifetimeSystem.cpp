// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NDK/Systems/LifetimeSystem.hpp>
#include <NDK/Components/LifetimeComponent.hpp>

namespace Ndk
{
	LifetimeSystem::LifetimeSystem()
	{
		Requires<LifetimeComponent>();
	}

	void LifetimeSystem::OnUpdate(float elapsedTime)
	{
		for (const Ndk::EntityHandle& entity : GetEntities())
		{
			auto& lifetime = entity->GetComponent<LifetimeComponent>();

			if (lifetime.UpdateLifetime(elapsedTime))
				entity->Kill();
		}
	}

	SystemIndex LifetimeSystem::systemIndex;
}
