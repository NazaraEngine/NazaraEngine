// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/World.hpp>
#include <Nazara/Core/Error.hpp>
#include <NDK/BaseComponent.hpp>
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
		if (!m_freeIdList.empty())
		{
			// We get an identifier
			id = m_freeIdList.back();
			m_freeIdList.pop_back();
		}
		else
		{
			// We allocate a new entity
			id = m_entities.size();

			// We can't use emplace_back due to the scope
			m_entities.push_back(Entity(this, id));
		}

		// We initialise the entity and we add it to the list of alive entities
		Entity& entity = m_entities[id].entity;
		entity.Create();

		m_aliveEntities.emplace_back(&entity);
		m_entities[id].aliveIndex = m_aliveEntities.size() - 1;

		return m_aliveEntities.back();
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

		m_aliveEntities.clear();
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

		EntityHandle clone = CreateEntity();

		const Nz::Bitset<>& componentBits = original->GetComponentBits();
		for (std::size_t i = componentBits.FindFirst(); i != componentBits.npos; i = componentBits.FindNext(i))
		{
			std::unique_ptr<BaseComponent> component(original->GetComponent(ComponentIndex(i)).Clone());
			clone->AddComponent(std::move(component));
		}

		return GetEntity(clone->GetId());
	}

	/*!
	* \brief Kills an entity
	*
	* \param Pointer to the entity
	*
	* \remark No change is done if entity is invalid
	*/

	void World::KillEntity(Entity* entity)
	{
		if (IsEntityValid(entity))
			m_killedEntities.UnboundedSet(entity->GetId(), true);
	}

	/*!
	* \brief Gets an entity
	* \return A constant reference to the modified entity
	*
	* \param id Identifier of the entity
	*
	* \remark Produces a NazaraError if entity identifier is not valid
	*/

	const EntityHandle& World::GetEntity(EntityId id)
	{
		if (IsEntityIdValid(id))
			return m_aliveEntities[m_entities[id].aliveIndex];
		else
		{
			NazaraError("Invalid ID");
			return EntityHandle::InvalidHandle;
		}
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

		// Handle killed entities before last call
		for (std::size_t i = m_killedEntities.FindFirst(); i != m_killedEntities.npos; i = m_killedEntities.FindNext(i))
		{
			EntityBlock& block = m_entities[i];
			Entity& entity = block.entity;

			NazaraAssert(entity.IsValid(), "Entity must be valid");

			// Send back the identifier of the entity to the free queue
			m_freeIdList.push_back(entity.GetId());

			// Destruction of the entity (invalidation of handle by the same way)
			entity.Destroy();

			// We take out the handle from the list of alive entities
			// With the idiom swap and pop

			NazaraAssert(block.aliveIndex < m_aliveEntities.size(), "Alive index out of range");

			if (block.aliveIndex < m_aliveEntities.size() - 1) // If it's not the last handle
			{
				EntityHandle& lastHandle = m_aliveEntities.back();
				EntityHandle& myHandle = m_aliveEntities[block.aliveIndex];

				myHandle = std::move(lastHandle);

				// We don't forget to update the index associated to the entity
				m_entities[myHandle->GetId()].aliveIndex = block.aliveIndex;
			}
			m_aliveEntities.pop_back();
		}
		m_killedEntities.Reset();

		// Handle of entities which need an update from the systems
		for (std::size_t i = m_dirtyEntities.FindFirst(); i != m_dirtyEntities.npos; i = m_dirtyEntities.FindNext(i))
		{
			NazaraAssert(i < m_entities.size(), "Entity index out of range");

			Entity* entity = &m_entities[i].entity;

			// Check entity validity (as it could have been reported as dirty and killed during the same iteration)
			if (!entity->IsValid())
				continue;

			Nz::Bitset<>& removedComponents = entity->GetRemovedComponentBits();
			for (std::size_t j = removedComponents.FindFirst(); j != m_dirtyEntities.npos; j = removedComponents.FindNext(j))
				entity->DestroyComponent(j);
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
					// No, it shouldn't, remove it if it's part of the system
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
