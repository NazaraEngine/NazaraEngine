// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NDK/World.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Core/Error.hpp>
#include <NDK/BaseComponent.hpp>
#include <NDK/Systems/PhysicsSystem2D.hpp>
#include <NDK/Systems/PhysicsSystem3D.hpp>
#include <NDK/Systems/VelocitySystem.hpp>

#ifndef NDK_SERVER
#include <NDK/Systems/ListenerSystem.hpp>
#include <NDK/Systems/ParticleSystem.hpp>
#include <NDK/Systems/RenderSystem.hpp>
#endif

namespace Ndk
{
	/*!
	* \ingroup NDK
	* \class Ndk::World
	* \brief NDK class that represents a world
	*/

	/*!
	* \brief Destructs the object and calls Clear
	*
	* \see Clear
	*/

	World::~World() noexcept
	{
		// The destruct must be done in an ordered way
		Clear();
	}

	/*!
	* \brief Adds default systems to the world
	*/

	void World::AddDefaultSystems()
	{
		AddSystem<PhysicsSystem2D>();
		AddSystem<PhysicsSystem3D>();
		AddSystem<VelocitySystem>();

		#ifndef NDK_SERVER
		AddSystem<ListenerSystem>();
		AddSystem<ParticleSystem>();
		AddSystem<RenderSystem>();
		#endif
	}

	/*!
	* \brief Creates an entity in the world
	* \return The entity created
	*/

	const EntityHandle& World::CreateEntity()
	{
		EntityId id;
		EntityBlock* entBlock;

		std::size_t freeEntityId = m_freeEntityIds.FindFirst();
		if (freeEntityId != m_freeEntityIds.npos)
		{
			// We get an identifier
			m_freeEntityIds.Reset(freeEntityId); //< Remove id from free entity id

			id = static_cast<EntityId>(freeEntityId);

			entBlock = &m_entities[id];
			entBlock->handle.Reset(&entBlock->entity); //< Reset handle (as it was reset when entity got destroyed)

			m_entityBlocks[id] = entBlock;
		}
		else
		{
			// We allocate a new entity
			id = static_cast<Ndk::EntityId>(m_entityBlocks.size());

			if (m_entities.capacity() > m_entities.size())
			{
				NazaraAssert(m_waitingEntities.empty(), "There should be no waiting entities if space is available in main container");

				m_entities.emplace_back(Entity(this, id)); //< We can't make our vector create the entity due to the scope
				entBlock = &m_entities.back();
			}
			else
			{
				// Pushing to entities would reallocate vector and thus, invalidate EntityHandles (which we don't want until world update)
				// To prevent this, allocate them into a separate vector and move them at update
				// For now, we are counting on m_entities grow strategy to keep allocation frequency low
				m_waitingEntities.emplace_back(std::make_unique<EntityBlock>(Entity(this, id)));
				entBlock = m_waitingEntities.back().get();
			}

			if (id >= m_entityBlocks.size())
				m_entityBlocks.resize(id + 1);

			m_entityBlocks[id] = entBlock;
		}

		// We initialize the entity and we add it to the list of alive entities
		entBlock->entity.Create();

		m_aliveEntities.Insert(&entBlock->entity);

		return entBlock->handle;
	}

	/*!
	* \brief Clears the world from every entities
	*
	* \remark Every handles are correctly invalidated, entities are immediately invalidated
	*/

	void World::Clear() noexcept
	{
		// Destroy every valid entity first, to ensure entities are still accessible by ID while being destroyed
		for (EntityBlock* entBlock : m_entityBlocks)
		{
			if (entBlock->entity.IsValid())
				entBlock->entity.Destroy();
		}
		m_entityBlocks.clear();

		m_entities.clear();
		m_waitingEntities.clear();

		m_aliveEntities.Clear();
		m_dirtyEntities.Clear();
		m_freeEntityIds.Clear();
		m_killedEntities.Clear();
	}

	/*!
	* \brief Clones the entity
	* \return The clone newly created
	*
	* \param id Identifier of the entity
	*
	* \remark Cloning a disabled entity will produce an enabled clone
	*/
	const EntityHandle& World::CloneEntity(EntityId id)
	{
		const EntityHandle& original = GetEntity(id);
		if (!original)
		{
			NazaraError("Invalid entity ID");
			return EntityHandle::InvalidHandle;
		}

		const EntityHandle& clone = CreateEntity();
		if (!original->IsEnabled())
			clone->Disable();

		const Nz::Bitset<>& componentBits = original->GetComponentBits();
		for (std::size_t i = componentBits.FindFirst(); i != componentBits.npos; i = componentBits.FindNext(i))
		{
			std::unique_ptr<BaseComponent> component(original->GetComponent(ComponentIndex(i)).Clone());
			clone->AddComponent(std::move(component));
		}

		clone->Enable();

		return clone;
	}

	/*!
	* \brief Refreshes the world
	*
	* This function will perform all pending operations in the following order:
	* - Reorder systems according to their update order if needed
	* - Moving newly created entities (whose which allocate never-used id) data and handles to normal entity list, this will invalidate references to world EntityHandle
	* - Destroying dead entities and allowing their ids to be used by newly created entities
	* - Update dirty entities, destroying their removed components and filtering them along systems
	*
	* \remark Calling this outside of Update will not increase the profiler values
	*
	* \see GetProfilerData
	* \see Update
	*/
	void World::Refresh()
	{
		if (!m_orderedSystemsUpdated)
			ReorderSystems();

		// Move waiting entities to entity list
		if (!m_waitingEntities.empty())
		{
			constexpr std::size_t MinEntityCapacity = 10; //< We want to be able to grow maximum entity count by at least ten without going to the waiting list

			m_entities.reserve(m_entities.size() + m_waitingEntities.size() + MinEntityCapacity);
			for (auto& blockPtr : m_waitingEntities)
				m_entities.push_back(std::move(*blockPtr));

			m_waitingEntities.clear();

			// Update entity blocks pointers
			for (std::size_t i = 0; i < m_entities.size(); ++i)
				m_entityBlocks[i] = &m_entities[i];
		}

		// Handle killed entities before last call
		for (std::size_t i = m_killedEntities.FindFirst(); i != m_killedEntities.npos; i = m_killedEntities.FindNext(i))
		{
			NazaraAssert(i < m_entityBlocks.size(), "Entity index out of range");

			Entity* entity = &m_entityBlocks[i]->entity;

			// Destruction of the entity (invalidation of handle by the same way)
			entity->Destroy();

			// Send back the identifier of the entity to the free queue
			m_freeEntityIds.UnboundedSet(entity->GetId());
		}
		m_killedEntities.Reset();

		// Handle of entities which need an update from the systems
		for (std::size_t i = m_dirtyEntities.FindFirst(); i != m_dirtyEntities.npos; i = m_dirtyEntities.FindNext(i))
		{
			NazaraAssert(i < m_entityBlocks.size(), "Entity index out of range");

			Entity* entity = &m_entityBlocks[i]->entity;

			// Check entity validity (as it could have been reported as dirty and killed during the same iteration)
			if (!entity->IsValid())
				continue;

			Nz::Bitset<>& removedComponents = entity->GetRemovedComponentBits();
			for (std::size_t j = removedComponents.FindFirst(); j != m_dirtyEntities.npos; j = removedComponents.FindNext(j))
				entity->DestroyComponent(static_cast<Ndk::ComponentIndex>(j));
			removedComponents.Reset();

			for (auto& system : m_orderedSystems)
			{
				// Is our entity already part of this system?
				bool partOfSystem = system->HasEntity(entity);

				// Should it be part of it?
				if (entity->IsEnabled() && system->Filters(entity))
				{
					// Yes it should, add it to the system if not already done and validate it (again)
					if (!partOfSystem)
						system->AddEntity(entity);

					system->ValidateEntity(entity, !partOfSystem);
				}
				else
				{
					// No it shouldn't, remove it if it's part of the system
					if (partOfSystem)
						system->RemoveEntity(entity);
				}
			}
		}
		m_dirtyEntities.Reset();
	}

	/*!
	* \brief Updates the world
	* \param elapsedTime Delta time used for the update
	*
	* This function Refreshes the world and calls the Update function of every active system part of it with the elapsedTime value.
	* It also increase the profiler data with the elapsed time passed in Refresh and every system update.
	*/
	void World::Update(float elapsedTime)
	{
		if (m_isProfilerEnabled)
		{
			Nz::UInt64 t1 = Nz::GetElapsedMicroseconds();
			Refresh();
			Nz::UInt64 t2 = Nz::GetElapsedMicroseconds();

			m_profilerData.refreshTime += t2 - t1;

			for (auto& systemPtr : m_orderedSystems)
			{
				systemPtr->Update(elapsedTime);

				Nz::UInt64 t3 = Nz::GetElapsedMicroseconds();
				m_profilerData.updateTime[systemPtr->GetIndex()] += t3 - t2;
				t2 = t3;
			}

			m_profilerData.updateCount++;
		}
		else
		{
			Refresh();

			for (auto& systemPtr : m_orderedSystems)
				systemPtr->Update(elapsedTime);
		}
	}

	void World::ReorderSystems()
	{
		m_orderedSystems.clear();

		for (auto& systemPtr : m_systems)
		{
			if (systemPtr)
				m_orderedSystems.push_back(systemPtr.get());
		}

		std::sort(m_orderedSystems.begin(), m_orderedSystems.end(), [] (BaseSystem* first, BaseSystem* second)
		{
			return first->GetUpdateOrder() < second->GetUpdateOrder();
		});

		m_orderedSystemsUpdated = true;
	}
}
