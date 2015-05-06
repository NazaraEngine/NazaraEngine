// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_WORLD_HPP
#define NDK_WORLD_HPP

#include <Nazara/Core/Bitset.hpp>
#include <Nazara/Core/NonCopyable.hpp>
#include <NDK/Entity.hpp>
#include <NDK/EntityHandle.hpp>
#include <NDK/System.hpp>
#include <algorithm>
#include <memory>
#include <vector>
#include <unordered_map>

namespace Ndk
{
	class NDK_API World : NzNonCopyable
	{
		friend Entity;

		public:
			using EntityList = std::vector<EntityHandle>;

			World(bool addDefaultSystems = true);
			~World();

			void AddDefaultSystems();

			BaseSystem& AddSystem(std::unique_ptr<BaseSystem>&& system);
			template<typename SystemType, typename... Args> SystemType& AddSystem(Args&&... args);

			const EntityHandle& CreateEntity();
			EntityList CreateEntities(unsigned int count);

			void Clear();

			const EntityHandle& GetEntity(EntityId id);
			const EntityList& GetEntities();
			BaseSystem& GetSystem(SystemIndex index);
			template<typename SystemType> SystemType& GetSystem();

			bool HasSystem(SystemIndex index) const;
			template<typename SystemType> bool HasSystem() const;

			void KillEntity(Entity* entity);
			void KillEntities(const EntityList& list);

			bool IsEntityValid(const Entity* entity) const;
			bool IsEntityIdValid(EntityId id) const;

			void RemoveAllSystems();
			void RemoveSystem(SystemIndex index);
			template<typename SystemType> void RemoveSystem();

			void Update();

		private:
			void Invalidate();
			void Invalidate(EntityId id);

			struct EntityBlock
			{
				EntityBlock(Entity&& e) :
				entity(std::move(e)),
				aliveIndex(e.aliveIndex)
				{
				}

				Entity entity;
				unsigned int aliveIndex;
			};

			std::vector<std::unique_ptr<BaseSystem>> m_systems;
			std::vector<EntityBlock> m_entities;
			std::vector<EntityId> m_freeIdList;
			EntityList m_aliveEntities;
			NzBitset<nzUInt64> m_dirtyEntities;
			NzBitset<nzUInt64> m_killedEntities;
	};
}

#include <NDK/World.inl>

#endif // NDK_WORLD_HPP
