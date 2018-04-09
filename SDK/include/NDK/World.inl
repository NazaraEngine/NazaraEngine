// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NDK/World.hpp>
#include <Nazara/Core/Error.hpp>
#include <type_traits>

namespace Ndk
{
	/*!
	* \brief Constructs a World object
	*
	* \param addDefaultSystems Should default provided systems be used
	*/

	inline World::World(bool addDefaultSystems) :
	m_orderedSystemsUpdated(false),
	m_isProfilerEnabled(false)
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
		{
			m_systems.resize(index + 1);
			m_profilerData.updateTime.resize(index + 1, 0);
		}

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
	inline World::EntityVector World::CreateEntities(unsigned int count)
	{
		EntityVector list;
		list.reserve(count);

		for (unsigned int i = 0; i < count; ++i)
			list.emplace_back(CreateEntity());

		return list;
	}

	/*!
	* \brief Disables the profiler, clearing up results
	*
	* This is just a shortcut to EnableProfiler(false)
	*
	* \param enable Should the entity be enabled
	*
	* \see EnableProfiler
	*/
	inline void World::DisableProfiler()
	{
		EnableProfiler(false);
	}

	/*!
	* \brief Enables/Disables the internal profiler
	*
	* Worlds come with a built-in profiler, allowing to measure update count along with time passed in refresh and system updates.
	* This is disabled by default as it adds an small overhead to the update process.
	*
	* \param enable Should the profiler be enabled
	*
	* \remark Disabling the profiler clears up results, as if ResetProfiler has been called
	*/
	inline void World::EnableProfiler(bool enable)
	{
		if (m_isProfilerEnabled != enable)
		{
			m_isProfilerEnabled = enable;

			if (enable)
				ResetProfiler();
		}
	}

	/*!
	* \brief Executes a function on every present system
	*
	* Calls iterationFunc on every previously added system, in the same order as their indexes
	*
	* \param iterationFunc Function to be called
	*/
	template<typename F>
	void World::ForEachSystem(const F& iterationFunc)
	{
		for (const auto& systemPtr : m_systems)
		{
			if (systemPtr)
				iterationFunc(*systemPtr);
		}
	}

	/*!
	* \brief Executes a function on every present system
	*
	* Calls iterationFunc on every previously added system, in the same order as their indexes
	*
	* \param iterationFunc Function to be called
	*/
	template<typename F>
	void World::ForEachSystem(const F& iterationFunc) const
	{
		for (const auto& systemPtr : m_systems)
		{
			if (systemPtr)
				iterationFunc(static_cast<const Ndk::BaseSystem&>(*systemPtr)); //< Force const reference
		}
	}

	/*!
	* \brief Gets an entity
	* \return A constant reference to a handle of the entity
	*
	* \param id Identifier of the entity
	*
	* \remark Handle referenced by this function can move in memory when updating the world, do not keep a handle reference from a world update to another
	* \remark If an invalid identifier is provided, an error got triggered and an invalid handle is returned
	*/
	inline const EntityHandle& World::GetEntity(EntityId id)
	{
		if (IsEntityIdValid(id))
			return m_entityBlocks[id]->handle;
		else
		{
			NazaraError("Invalid ID");
			return EntityHandle::InvalidHandle;
		}
	}

	/*!
	* \brief Gets every entities in the world
	* \return A constant reference to the entities
	*/
	inline const EntityList& World::GetEntities() const
	{
		return m_aliveEntities;
	}

	/*!
	* \brief Gets the latest profiler data
	* \return A constant reference to the profiler data
	*/
	inline const World::ProfilerData& World::GetProfilerData() const
	{
		return m_profilerData;
	}

	/*!
	* \brief Gets a system in the world by index
	* \return A reference to the system
	*
	* \param index Index of the system
	*
	* \remark The world must have the system before calling this function
	*/
	inline BaseSystem& World::GetSystem(SystemIndex index)
	{
		NazaraAssert(HasSystem(index), "This system is not part of the world");

		BaseSystem* system = m_systems[index].get();
		NazaraAssert(system, "Invalid system pointer");

		return *system;
	}

	/*!
	* \brief Gets a system in the world by index
	* \return A const reference to the system
	*
	* \param index Index of the system
	*
	* \remark The world must have the system before calling this function
	*/
	inline const BaseSystem& World::GetSystem(SystemIndex index) const
	{
		NazaraAssert(HasSystem(index), "This system is not part of the world");

		const BaseSystem* system = m_systems[index].get();
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
	* \brief Gets a system in the world by type
	* \return A const reference to the system
	*
	* \remark Produces a NazaraAssert if system is not available in this world
	*/
	template<typename SystemType>
	const SystemType& World::GetSystem() const
	{
		static_assert(std::is_base_of<BaseSystem, SystemType>::value, "SystemType is not a system");

		SystemIndex index = GetSystemIndex<SystemType>();
		return static_cast<const SystemType&>(GetSystem(index));
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
	* \brief Marks an entity for deletion
	*
	* \param Pointer to the entity
	*
	* \remark If the entity pointer is invalid, nothing is done
	* \remark For safety, entities are not killed until the next world update
	*/
	inline void World::KillEntity(Entity* entity)
	{
		if (IsEntityValid(entity))
			m_killedEntities.UnboundedSet(entity->GetId(), true);
	}

	/*!
	* \brief Kills a set of entities
	*
	* This function has the same effect as calling KillEntity for every entity contained in the vector
	*
	* \param list Set of entities to kill
	*/
	inline void World::KillEntities(const EntityVector& list)
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
		return id < m_entityBlocks.size() && m_entityBlocks[id]->entity.IsValid();
	}

	/*!
	* \brief Checks whether or not the profiler is enabled
	* \return true If it is the case
	*
	* \see EnableProfiler
	*/
	inline bool World::IsProfilerEnabled() const
	{
		return m_isProfilerEnabled;
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
	* \brief Clear profiler results
	*
	* This reset the profiler results, filling all counters with zero
	*/
	inline void World::ResetProfiler()
	{
		m_profilerData.refreshTime = 0;
		m_profilerData.updateCount = 0;
		std::fill(m_profilerData.updateTime.begin(), m_profilerData.updateTime.end(), 0);
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
	* \brief Moves a world into another world object
	* \return A reference to the object
	*/

	inline World& World::operator=(World&& world) noexcept
	{
		m_aliveEntities         = std::move(world.m_aliveEntities);
		m_dirtyEntities         = std::move(world.m_dirtyEntities);
		m_entityBlocks          = std::move(world.m_entityBlocks);
		m_freeEntityIds         = std::move(world.m_freeEntityIds);
		m_killedEntities        = std::move(world.m_killedEntities);
		m_orderedSystems        = std::move(world.m_orderedSystems);
		m_orderedSystemsUpdated = world.m_orderedSystemsUpdated;
		m_profilerData          = std::move(world.m_profilerData);
		m_isProfilerEnabled     = m_isProfilerEnabled;

		m_entities = std::move(world.m_entities);
		for (EntityBlock& block : m_entities)
			block.entity.SetWorld(this);

		m_waitingEntities = std::move(world.m_waitingEntities);
		for (auto& blockPtr : m_waitingEntities)
			blockPtr->entity.SetWorld(this);

		m_systems = std::move(world.m_systems);
		for (const auto& systemPtr : m_systems)
			if (systemPtr)
				systemPtr->SetWorld(this);

		return *this;
	}

	inline void World::Invalidate()
	{
		m_dirtyEntities.Resize(m_entityBlocks.size(), false);
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
