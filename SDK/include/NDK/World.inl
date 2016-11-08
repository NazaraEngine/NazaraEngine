// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <Nazara/Core/Error.hpp>
#include <type_traits>

namespace Ndk
{
	/*!
	* \brief Constructs a World object
	*
	* \param addDefaultSystems Should default provided systems be used
	*/

	inline World::World(bool addDefaultSystems)
	{
		if (addDefaultSystems)
			AddDefaultSystems();
	}

	/*!
	* \brief Constructs a World object by move semantic
	*
	* \param world World to move into this
	*/

	inline World::World(World&& world) noexcept :
	HandledObject(std::move(world))
	{
		operator=(std::move(world));
	}

	/*!
	* \brief Adds a system to the world
	* \return A reference to the newly created system
	*
	* \param system System to add to the world
	*/

	inline BaseSystem& World::AddSystem(std::unique_ptr<BaseSystem>&& system)
	{
		NazaraAssert(system, "System must be valid");

		SystemIndex index = system->GetIndex();

		// We must ensure that the vector is big enough to hold the new system
		if (index >= m_systems.size())
			m_systems.resize(index + 1);

		// Affectation and return of system
		m_systems[index] = std::move(system);
		m_systems[index]->SetWorld(this);

		Invalidate(); // We force an update for every entities
		InvalidateSystemOrder(); // And regenerate the system update list

		return *m_systems[index].get();
	}

	/*!
	* \brief Adds a system to the world
	* \return A reference to the newly created system
	*
	* \param args Arguments used to create the system
	*/

	template<typename SystemType, typename... Args>
	SystemType& World::AddSystem(Args&&... args)
	{
		static_assert(std::is_base_of<BaseSystem, SystemType>::value, "SystemType is not a component");

		// Allocation and affectation of the system
		std::unique_ptr<SystemType> ptr(new SystemType(std::forward<Args>(args)...));
		return static_cast<SystemType&>(AddSystem(std::move(ptr)));
	}

	/*!
	* \brief Creates multiple entities in the world
	* \return The set of entities created
	*
	* \param count Number of entities to create
	*/

	inline World::EntityList World::CreateEntities(unsigned int count)
	{
		EntityList list;
		list.reserve(count);

		for (unsigned int i = 0; i < count; ++i)
			list.emplace_back(CreateEntity());

		return list;
	}

	/*!
	* \brief Gets every entities in the world
	* \return A constant reference to the entities
	*/

	inline const World::EntityList& World::GetEntities()
	{
		return m_aliveEntities;
	}

	/*!
	* \brief Gets a system in the world by index
	* \return A reference to the system
	*
	* \param index Index of the system
	*
	* \remark Produces a NazaraAssert if system is not available in this world
	*/

	inline BaseSystem& World::GetSystem(SystemIndex index)
	{
		NazaraAssert(HasSystem(index), "This system is not part of the world");

		BaseSystem* system = m_systems[index].get();
		NazaraAssert(system, "Invalid system pointer");

		return *system;
	}

	/*!
	* \brief Gets a system in the world by type
	* \return A reference to the system
	*
	* \remark Produces a NazaraAssert if system is not available in this world
	*/

	template<typename SystemType>
	SystemType& World::GetSystem()
	{
		static_assert(std::is_base_of<BaseSystem, SystemType>::value, "SystemType is not a system");

		SystemIndex index = GetSystemIndex<SystemType>();
		return static_cast<SystemType&>(GetSystem(index));
	}

	/*!
	* \brief Checks whether or not a system is present in the world by index
	* \return true If it is the case
	*
	* \param index Index of the system
	*/

	inline bool World::HasSystem(SystemIndex index) const
	{
		return index < m_systems.size() && m_systems[index];
	}

	/*!
	* \brief Checks whether or not a system is present in the world by type
	* \return true If it is the case
	*/

	template<typename SystemType>
	bool World::HasSystem() const
	{
		static_assert(std::is_base_of<BaseSystem, SystemType>::value, "SystemType is not a component");

		SystemIndex index = GetSystemIndex<SystemType>();
		return HasSystem(index);
	}

	/*!
	* \brief Kills a set of entities
	*
	* \param list Set of entities to kill
	*/

	inline void World::KillEntities(const EntityList& list)
	{
		for (const EntityHandle& entity : list)
			KillEntity(entity);
	}

	/*!
	* \brief Checks whether or not an entity is valid
	* \return true If it is the case
	*
	* \param entity Pointer to the entity
	*/

	inline bool World::IsEntityValid(const Entity* entity) const
	{
		return entity && entity->GetWorld() == this && IsEntityIdValid(entity->GetId());
	}

	/*!
	* \brief Checks whether or not an entity is valid
	* \return true If it is the case
	*
	* \param id Identifier of the entity
	*/

	inline bool World::IsEntityIdValid(EntityId id) const
	{
		return id < m_entities.size() && m_entities[id].entity.IsValid();
	}

	/*!
	* \brief Removes each system from the world
	*/

	inline void World::RemoveAllSystems()
	{
		m_systems.clear();

		InvalidateSystemOrder();
	}

	/*!
	* \brief Removes a system from the world by index
	*
	* \param index Index of the system
	*
	* \remark No change is done if system is not present
	*/

	inline void World::RemoveSystem(SystemIndex index)
	{
		if (HasSystem(index))
		{
			m_systems[index].reset();

			InvalidateSystemOrder();
		}
	}

	/*!
	* \brief Removes a system from the world by type
	*/

	template<typename SystemType>
	void World::RemoveSystem()
	{
		static_assert(std::is_base_of<BaseSystem, SystemType>(), "SystemType is not a system");

		SystemIndex index = GetSystemIndex<SystemType>();
		RemoveSystem(index);
	}

	/*!
	* \brief Updates the world
	*
	* \param elapsedTime Delta time used for the update
	*/

	inline void World::Update(float elapsedTime)
	{
		Update(); //< Update entities

		// And then update systems
		if (!m_orderedSystemsUpdated)
			ReorderSystems();

		for (auto& systemPtr : m_orderedSystems)
			systemPtr->Update(elapsedTime);
	}

	/*!
	* \brief Moves a world into another world object
	* \return A reference to the object
	*/

	inline World& World::operator=(World&& world) noexcept
	{
		m_aliveEntities         = std::move(world.m_aliveEntities);
		m_dirtyEntities         = std::move(world.m_dirtyEntities);
		m_freeIdList            = std::move(world.m_freeIdList);
		m_killedEntities        = std::move(world.m_killedEntities);
		m_orderedSystems        = std::move(world.m_orderedSystems);
		m_orderedSystemsUpdated = world.m_orderedSystemsUpdated;

		m_entities = std::move(world.m_entities);
		for (EntityBlock& block : m_entities)
			block.entity.SetWorld(this);

		m_systems = std::move(world.m_systems);
		for (const auto& systemPtr : m_systems)
			systemPtr->SetWorld(this);

		return *this;
	}

	inline void World::Invalidate()
	{
		m_dirtyEntities.Resize(m_entities.size(), false);
		m_dirtyEntities.Set(true); // Activation of all bits
	}

	inline void World::Invalidate(EntityId id)
	{
		m_dirtyEntities.UnboundedSet(id, true);
	}

	inline void World::InvalidateSystemOrder()
	{
		m_orderedSystemsUpdated = false;
	}
}
