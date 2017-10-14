// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

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
		SetFixedUpdateRate(0);
		SetMaximumUpdateRate(30);
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

	inline const EntityList& BaseSystem::GetEntities() const
	{
		return m_entities;
	}

	/*!
	* \brief Gets the maximum rate of update of the system
	* \return Update rate
	*/
	inline float BaseSystem::GetFixedUpdateRate() const
	{
		return (m_fixedUpdateRate > 0.f) ? 1.f / m_fixedUpdateRate : 0.f;
	}

	/*!
	* \brief Gets the maximum rate of update of the system
	* \return Update rate
	*/
	inline float BaseSystem::GetMaximumUpdateRate() const
	{
		return (m_maxUpdateRate > 0.f) ? 1.f / m_maxUpdateRate : 0.f;
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
		return m_entities.Has(entity);
	}

	/*!
	* \brief Sets the fixed update rate for the system
	*
	* \param updatePerSecond Update rate, 0 means update rate is not fixed
	*/
	inline void BaseSystem::SetFixedUpdateRate(float updatePerSecond)
	{
		m_updateCounter = 0.f;
		m_fixedUpdateRate = (updatePerSecond > 0.f) ? 1.f / updatePerSecond : 0.f; // 0.f means no limit
	}

	/*!
	* \brief Sets the maximum update rate for the system
	*
	* \param updatePerSecond Update rate, 0 means as much as possible
	*/
	inline void BaseSystem::SetMaximumUpdateRate(float updatePerSecond)
	{
		m_updateCounter = 0.f;
		m_maxUpdateRate = (updatePerSecond > 0.f) ? 1.f / updatePerSecond : 0.f; // 0.f means no limit
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

		m_updateCounter += elapsedTime;

		if (m_maxUpdateRate > 0.f)
		{
			if (m_updateCounter >= m_maxUpdateRate)
			{
				if (m_fixedUpdateRate > 0.f)
				{
					while (m_updateCounter >= m_fixedUpdateRate)
					{
						OnUpdate(m_fixedUpdateRate);
						m_updateCounter -= m_fixedUpdateRate;
					}
				}
				else
				{
					OnUpdate(m_maxUpdateRate);
					m_updateCounter -= m_maxUpdateRate;
				}
			}
		}
		else
		{
			if (m_fixedUpdateRate > 0.f)
			{
				while (m_updateCounter >= m_fixedUpdateRate)
				{
					OnUpdate(m_fixedUpdateRate);
					m_updateCounter -= m_fixedUpdateRate;
				}
			}
			else
				OnUpdate(elapsedTime);
		}
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

		m_entities.Insert(entity);
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

		m_entities.Remove(entity);
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
	* \brief Uninitialize the BaseSystem
	*/

	inline void BaseSystem::Uninitialize()
	{
		// Nothing to do
	}
}
