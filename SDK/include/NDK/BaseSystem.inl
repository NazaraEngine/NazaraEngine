// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/BaseSystem.hpp>
#include <Nazara/Core/Error.hpp>
#include <type_traits>

namespace Ndk
{
	/*!
	* \brief Constructs a BaseSystem object with an index
	*
	* \param systemId Index of the system
	*/

	inline BaseSystem::BaseSystem(SystemIndex systemId) :
	m_systemIndex(systemId),
	m_world(nullptr),
	m_updateEnabled(true),
	m_updateOrder(0)
	{
		SetUpdateRate(30);
	}

	/*!
	* \brief Constructs a BaseSystem object by copy semantic
	*
	* \param system System to copy
	*/

	inline BaseSystem::BaseSystem(const BaseSystem& system) :
	m_excludedComponents(system.m_excludedComponents),
	m_requiredComponents(system.m_requiredComponents),
	m_systemIndex(system.m_systemIndex),
	m_updateEnabled(system.m_updateEnabled),
	m_updateCounter(0.f),
	m_updateRate(system.m_updateRate),
	m_updateOrder(system.m_updateOrder)
	{
	}

	/*!
	* \brief Enables the system
	*
	* \param enable Should the system be enabled
	*/

	inline void BaseSystem::Enable(bool enable)
	{
		m_updateEnabled = enable;
	}

	/*!
	* \brief Gets every entities that system handle
	* \return A constant reference to the list of entities
	*/

	inline const std::vector<EntityHandle>& BaseSystem::GetEntities() const
	{
		return m_entities;
	}

	/*!
	* \brief Gets the index of the system
	* \return Index of the system
	*/

	inline SystemIndex BaseSystem::GetIndex() const
	{
		return m_systemIndex;
	}

	/*!
	* \brief Gets the update order of the system
	* \return Update order
	*
	* \see SetUpdateOrder
	*/
	inline int BaseSystem::GetUpdateOrder() const
	{
		return m_updateOrder;
	}

	/*!
	* \brief Gets the rate of update of the system
	* \return Update rate
	*/

	inline float BaseSystem::GetUpdateRate() const
	{
		return (m_updateRate > 0.f) ? 1.f / m_updateRate : 0.f;
	}

	/*!
	* \brief Gets the world on which the system operate
	* \return World in which the system is
	*/

	inline World& BaseSystem::GetWorld() const
	{
		return *m_world;
	}

	/*!
	* \brief Checks whether or not the system is enabled
	* \return true If it is the case
	*/

	inline bool BaseSystem::IsEnabled() const
	{
		return m_updateEnabled;
	}

	/*!
	* \brief Checks whether or not the system has the entity
	* \return true If it is the case
	*
	* \param entity Pointer to the entity
	*/

	inline bool BaseSystem::HasEntity(const Entity* entity) const
	{
		if (!entity)
			return false;

		return m_entityBits.UnboundedTest(entity->GetId());
	}

	/*!
	* \brief Sets the rate of update for the system
	*
	* \param updatePerSecond Update rate, 0 means as much as possible
	*/

	inline void BaseSystem::SetUpdateRate(float updatePerSecond)
	{
		m_updateCounter = 0.f;
		m_updateRate = (updatePerSecond > 0.f) ? 1.f / updatePerSecond : 0.f; // 0.f means no limit
	}

	/*!
	* \brief Updates the system
	*
	* \param elapsedTime Delta time used for the update
	*/

	inline void BaseSystem::Update(float elapsedTime)
	{
		if (!IsEnabled())
			return;

		if (m_updateRate > 0.f)
		{
			m_updateCounter += elapsedTime;

			while (m_updateCounter >= m_updateRate)
			{
				OnUpdate(m_updateRate);
				m_updateCounter -= m_updateRate;
			}
		}
		else
			OnUpdate(elapsedTime);
	}

	/*!
	* \brief Excludes some component from the system
	*/

	template<typename ComponentType>
	void BaseSystem::Excludes()
	{
		static_assert(std::is_base_of<BaseComponent, ComponentType>::value , "ComponentType is not a component");

		ExcludesComponent(GetComponentIndex<ComponentType>());
	}

	/*!
	* \brief Excludes some components from the system
	*/

	template<typename ComponentType1, typename ComponentType2, typename... Rest>
	void BaseSystem::Excludes()
	{
		Excludes<ComponentType1>();
		Excludes<ComponentType2, Rest...>();
	}

	/*!
	* \brief Excludes some component from the system by index
	*
	* \param index Index of the component
	*/

	inline void BaseSystem::ExcludesComponent(ComponentIndex index)
	{
		m_excludedComponents.UnboundedSet(index);
	}

	/*!
	* \brief Gets the next index for the system
	* \return Next unique index for the system
	*/

	inline SystemIndex BaseSystem::GetNextIndex()
	{
		return s_nextIndex++;
	}

	/*!
	* \brief Requires some component from the system
	*/

	template<typename ComponentType>
	void BaseSystem::Requires()
	{
		static_assert(std::is_base_of<BaseComponent, ComponentType>::value, "ComponentType is not a component");

		RequiresComponent(GetComponentIndex<ComponentType>());
	}

	/*!
	* \brief Requires some components from the system
	*/

	template<typename ComponentType1, typename ComponentType2, typename... Rest>
	void BaseSystem::Requires()
	{
		Requires<ComponentType1>();
		Requires<ComponentType2, Rest...>();
	}

	/*!
	* \brief Requires some component for the system by index
	*
	* \param index Index of the component
	*/

	inline void BaseSystem::RequiresComponent(ComponentIndex index)
	{
		m_requiredComponents.UnboundedSet(index);
	}

	/*!
	* \brief Requires any component from the system
	*/

	template<typename ComponentType>
	void BaseSystem::RequiresAny()
	{
		static_assert(std::is_base_of<BaseComponent, ComponentType>::value, "ComponentType is not a component");

		RequiresAnyComponent(GetComponentIndex<ComponentType>());
	}

	/*!
	* \brief Requires any components from the system
	*/

	template<typename ComponentType1, typename ComponentType2, typename... Rest>
	void BaseSystem::RequiresAny()
	{
		RequiresAny<ComponentType1>();
		RequiresAny<ComponentType2, Rest...>();
	}

	/*!
	* \brief Requires any component for the system by index
	*
	* \param index Index of the component
	*/

	inline void BaseSystem::RequiresAnyComponent(ComponentIndex index)
	{
		m_requiredAnyComponents.UnboundedSet(index);
	}

	/*!
	* \brief Adds an entity to a system
	*
	* \param entity Pointer to the entity
	*
	* \remark Produces a NazaraAssert if entity is invalid
	*/

	inline void BaseSystem::AddEntity(Entity* entity)
	{
		NazaraAssert(entity, "Invalid entity");

		m_entities.emplace_back(entity);
		m_entityBits.UnboundedSet(entity->GetId(), true);

		entity->RegisterSystem(m_systemIndex);

		OnEntityAdded(entity);
	}

	/*!
	* \brief Removes an entity to a system
	*
	* \param entity Pointer to the entity
	*
	* \remark Produces a NazaraAssert if entity is invalid
	*/

	inline void BaseSystem::RemoveEntity(Entity* entity)
	{
		NazaraAssert(entity, "Invalid entity");

		auto it = std::find(m_entities.begin(), m_entities.end(), *entity);
		NazaraAssert(it != m_entities.end(), "Entity is not part of this system");

		// To avoid moving a lot of handles, we swap and pop
		std::swap(*it, m_entities.back());
		m_entities.pop_back(); // We get it out of the vector

		m_entityBits.Reset(entity->GetId());
		entity->UnregisterSystem(m_systemIndex);

		OnEntityRemoved(entity); // And we alert our callback
	}

	/*!
	* \brief Validates an entity to a system
	*
	* \param entity Pointer to the entity
	* \param justAdded Is the entity newly added
	*
	* \remark Produces a NazaraAssert if entity is invalid or if system does not hold this entity
	*/

	inline void BaseSystem::ValidateEntity(Entity* entity, bool justAdded)
	{
		NazaraAssert(entity, "Invalid entity");
		NazaraAssert(HasEntity(entity), "Entity should be part of system");

		OnEntityValidation(entity, justAdded);
	}

	/*!
	* \brief Sets the world on which the system operates
	*/

	inline void BaseSystem::SetWorld(World* world) noexcept
	{
		m_world = world;
	}

	/*!
	* \brief Initializes the BaseSystem
	* \return true
	*/

	inline bool BaseSystem::Initialize()
	{
		s_nextIndex = 0;

		return true;
	}

	/*!
	* \brief Uninitializes the BaseSystem
	*/

	inline void BaseSystem::Uninitialize()
	{
		// Nothing to do
	}
}
