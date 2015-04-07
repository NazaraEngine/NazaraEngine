// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/BaseSystem.hpp>

namespace Ndk
{
	BaseSystem::~BaseSystem()
	{
		for (const EntityHandle& entity : m_entities)
			entity->UnregisterSystem(m_systemIndex);
	}

	bool BaseSystem::Filters(const Entity* entity) const
	{
		if (!entity)
			return false;

        const NzBitset<>& components = entity->GetComponentBits();

        m_filterResult.PerformsAND(m_requiredComponents, components);
        if (m_filterResult != m_requiredComponents)
            return false; // Au moins un component requis n'est pas présent

        m_filterResult.PerformsAND(m_excludedComponents, components);
        if (m_filterResult.TestAny())
            return false; // Au moins un component exclu est présent

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

	SystemIndex BaseSystem::s_nextIndex;
}
