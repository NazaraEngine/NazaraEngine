// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/BaseSystem.hpp>

namespace Ndk
{
	BaseSystem::~BaseSystem()
	{
		for (const EntityHandle& entity : m_entities)
			entity->UnregisterSystem(m_systemId);
	}

	bool BaseSystem::Filters(const Entity* entity) const
	{
		if (!entity)
			return false;

		for (ComponentId component : m_requiredComponents)
		{
			if (!entity->HasComponent(component))
				return false; // Au moins un component requis n'est pas présent
		}

		for (ComponentId component : m_excludedComponents)
		{
			if (entity->HasComponent(component))
				return false; // Au moins un component exclu est présent
		}

		return true;
	}

	void BaseSystem::OnEntityAdded(Entity* entity)
	{
		NazaraUnused(entity);
	}

	void BaseSystem::OnEntityRemoved(Entity* entity)
	{
		NazaraUnused(entity);
	}
}
