// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_WORLD_HPP
#define NDK_WORLD_HPP

#include <Nazara/Core/Bitset.hpp>
#include <NDK/Entity.hpp>
#include <NDK/EntityHandle.hpp>
#include <NDK/System.hpp>
#include <algorithm>
#include <memory>
#include <vector>
#include <unordered_map>

namespace Ndk
{
	class NDK_API World
	{
		friend Entity;

		public:
			using EntityList = std::vector<EntityHandle>;

			inline World(bool addDefaultSystems = true);
			World(const World&) = delete;
			World(World&&) = delete; ///TODO
			~World();

			void AddDefaultSystems();

			inline BaseSystem& AddSystem(std::unique_ptr<BaseSystem>&& system);
			template<typename SystemType, typename... Args> SystemType& AddSystem(Args&&... args);

			const EntityHandle& CreateEntity();
			inline EntityList CreateEntities(unsigned int count);

			void Clear();

			const EntityHandle& GetEntity(EntityId id);
			inline const EntityList& GetEntities();
			inline BaseSystem& GetSystem(SystemIndex index);
			template<typename SystemType> SystemType& GetSystem();

			inline bool HasSystem(SystemIndex index) const;
			template<typename SystemType> bool HasSystem() const;

			void KillEntity(Entity* entity);
			inline void KillEntities(const EntityList& list);

			inline bool IsEntityValid(const Entity* entity) const;
			inline bool IsEntityIdValid(EntityId id) const;

			inline void RemoveAllSystems();
			inline void RemoveSystem(SystemIndex index);
			template<typename SystemType> void RemoveSystem();

			void Update();
			inline void Update(float elapsedTime);

			World& operator=(const World&) = delete;
			World& operator=(World&&) = delete; ///TODO

		private:
			inline void Invalidate();
			inline void Invalidate(EntityId id);

			struct EntityBlock
			{
				EntityBlock(Entity&& e) :
				entity(std::move(e))
				{
				}

				EntityBlock(EntityBlock&& block) = default;

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
