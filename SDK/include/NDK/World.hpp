// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NDK_WORLD_HPP
#define NDK_WORLD_HPP

#include <Nazara/Core/Bitset.hpp>
#include <Nazara/Core/HandledObject.hpp>
#include <NDK/Entity.hpp>
#include <NDK/EntityList.hpp>
#include <NDK/System.hpp>
#include <algorithm>
#include <memory>
#include <vector>

namespace Ndk
{
	class World;

	using WorldHandle = Nz::ObjectHandle<World>;

	class NDK_API World : public Nz::HandledObject<World>
	{
		friend BaseSystem;
		friend Entity;

		public:
			using EntityVector = std::vector<EntityHandle>;

			inline World(bool addDefaultSystems = true);
			World(const World&) = delete;
			inline World(World&& world) noexcept;
			~World() noexcept;

			void AddDefaultSystems();

			inline BaseSystem& AddSystem(std::unique_ptr<BaseSystem>&& system);
			template<typename SystemType, typename... Args> SystemType& AddSystem(Args&&... args);

			const EntityHandle& CreateEntity();
			inline EntityVector CreateEntities(unsigned int count);

			void Clear() noexcept;
			const EntityHandle& CloneEntity(EntityId id);

			inline const EntityHandle& GetEntity(EntityId id);
			inline const EntityList& GetEntities() const;
			inline BaseSystem& GetSystem(SystemIndex index);
			template<typename SystemType> SystemType& GetSystem();

			inline bool HasSystem(SystemIndex index) const;
			template<typename SystemType> bool HasSystem() const;

			inline void KillEntity(Entity* entity);
			inline void KillEntities(const EntityVector& list);

			inline bool IsEntityValid(const Entity* entity) const;
			inline bool IsEntityIdValid(EntityId id) const;

			inline void RemoveAllSystems();
			inline void RemoveSystem(SystemIndex index);
			template<typename SystemType> void RemoveSystem();

			void Update();
			inline void Update(float elapsedTime);

			World& operator=(const World&) = delete;
			inline World& operator=(World&& world) noexcept;

		private:
			inline void Invalidate();
			inline void Invalidate(EntityId id);
			inline void InvalidateSystemOrder();
			void ReorderSystems();

			struct EntityBlock
			{
				EntityBlock(Entity&& e) :
				entity(std::move(e)),
				handle(&entity)
				{
				}

				EntityBlock(EntityBlock&& block) = default;

				Entity entity;
				EntityHandle handle;
			};

			std::vector<std::unique_ptr<BaseSystem>> m_systems;
			std::vector<BaseSystem*> m_orderedSystems;
			std::vector<EntityBlock> m_entities;
			std::vector<EntityBlock*> m_entityBlocks;
			std::vector<std::unique_ptr<EntityBlock>> m_waitingEntities;
			std::vector<EntityId> m_freeIdList;
			EntityList m_aliveEntities;
			Nz::Bitset<Nz::UInt64> m_dirtyEntities;
			Nz::Bitset<Nz::UInt64> m_killedEntities;
			bool m_orderedSystemsUpdated;
	};
}

#include <NDK/World.inl>

#endif // NDK_WORLD_HPP
