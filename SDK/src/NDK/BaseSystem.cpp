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

	bool BaseSystem::Filters(const EntityHandle& entity) const
	{
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

	void BaseSystem::OnEntityAdded(const EntityHandle& entity)
	{
		NazaraUnused(entity);
	}

	void BaseSystem::OnEntityRemoved(const EntityHandle& entity)
	{
		NazaraUnused(entity);
	}
}
