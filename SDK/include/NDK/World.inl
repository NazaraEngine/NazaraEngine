// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <Nazara/Core/Error.hpp>

namespace Ndk
{
	inline World::EntityList World::CreateEntities(unsigned int count)
	{
		EntityList list;
		list.reserve(count);

		for (unsigned int i = 0; i < count; ++i)
			list.emplace_back(CreateEntity());

		return list;
	}

	inline void World::KillEntities(const EntityList& list)
	{
		for (const EntityHandle& entity : list)
			KillEntity(entity);
	}

	inline bool World::IsEntityValid(const EntityHandle& entity) const
	{
		return entity.IsValid() && entity->GetWorld() == this && IsEntityIdValid(entity->GetId());
	}

	inline bool World::IsEntityIdValid(EntityId id) const
	{
		return id < m_entities.size() && m_entities[id].entity.IsValid();
	}
}
