// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_WORLD_HPP
#define NDK_WORLD_HPP

#include <Nazara/Core/NonCopyable.hpp>
#include <NDK/Entity.hpp>
#include <NDK/EntityHandle.hpp>
#include <vector>

namespace Ndk
{
	class EntityHandle;

	class NDK_API World : NzNonCopyable
	{
		public:
			using EntityList = std::vector<EntityHandle>;

			World() = default;
			~World();

			EntityHandle CreateEntity();
			EntityList CreateEntities(unsigned int count);

			void Clear();

			void KillEntity(const EntityHandle& entity);
			void KillEntities(const EntityList& list);

			EntityHandle GetEntity(Entity::Id id);

			bool IsEntityValid(const EntityHandle& entity) const;
			bool IsEntityIdValid(Entity::Id id) const;

			void Update();

		private:
			std::vector<Entity::Id> m_freeIdList;
			std::vector<Entity> m_entities;
			EntityList m_aliveEntities;
			EntityList m_killedEntities;
	};
}

#include <NDK/World.inl>

#endif // NDK_WORLD_HPP
