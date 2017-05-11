// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/World.hpp>
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
		if (!m_freeIdList.empty())
		{
			// We get an identifier
			id = m_freeIdList.back();
			m_freeIdList.pop_back();

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

				m_entities.push_back(Entity(this, id)); //< We can't use emplace_back due to the scope
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
		// First, destruction of entities, then handles
		// This is made to avoid that handle warn uselessly entities before their destruction
		m_entities.clear();
		m_entityBlocks.clear();

		m_aliveEntities.Clear();
		m_dirtyEntities.Clear();
		m_killedEntities.Clear();
	}

	/*!
	* \brief Clones the entity
	* \return The clone newly created
	*
	* \param id Identifier of the entity
	*
	* \remark Produces a NazaraError if the entity to clone does not exist
	*/

	const EntityHandle& World::CloneEntity(EntityId id)
	{
		EntityHandle original = GetEntity(id);
		if (!original)
		{
			NazaraError("Invalid entity ID");
			return EntityHandle::InvalidHandle;
		}

		const EntityHandle& clone = CreateEntity();

		const Nz::Bitset<>& componentBits = original->GetComponentBits();
		for (std::size_t i = componentBits.FindFirst(); i != componentBits.npos; i = componentBits.FindNext(i))
		{
			std::unique_ptr<BaseComponent> component(original->GetComponent(ComponentIndex(i)).Clone());
			clone->AddComponent(std::move(component));
		}

		return clone;
	}

	/*!
	* \brief Updates the world
	*
	* \remark Produces a NazaraAssert if an entity is invalid
	*/

	void World::Update()
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
			m_freeIdList.push_back(entity->GetId());
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
