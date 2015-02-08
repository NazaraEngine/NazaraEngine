// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_WORLD_HPP
#define NDK_WORLD_HPP

#include <Nazara/Core/NonCopyable.hpp>
#include <NDK/Prerequesites.hpp>
#include <NDK/Entity.hpp>
#include <vector>

namespace Ndk
{
	class NDK_API World : NzNonCopyable
	{
		public:
			using EntityList = std::vector<Entity>;

			World();
			~World() = default;

			Entity CreateEntity();
			EntityList CreateEntities(unsigned int count);

			void Clear();

			void KillEntity(Entity& entity);
			void KillEntities(EntityList& list);

			Entity GetEntity(Entity::Id id);

			bool IsEntityValid(const Entity& entity) const;
			bool IsEntityIdValid(Entity::Id id) const;

			void Update();

		private:
			std::vector<nzUInt32> m_entitiesCounter;
			std::vector<Entity::Id> m_freeIdList;
			EntityList m_aliveEntities;
			EntityList m_killedEntities;
			nzUInt32 m_nextIndex;
	};
}

#include <NDK/World.inl>

#endif // NDK_WORLD_HPP
