// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Entity.hpp>
#include <NDK/BaseComponent.hpp>
#include <NDK/World.hpp>

namespace Ndk
{
	/*!
	* \ingroup NDK
	* \class Ndk::Entity
	* \brief NDK class that represents an entity in a world
	*/

	/*!
	* \brief Constructs a Entity object by move semantic
	*
	* \param entity Entity to move into this
	*/

	Entity::Entity(Entity&& entity) :
	HandledObject(std::move(entity)),
	m_components(std::move(entity.m_components)),
	m_componentBits(std::move(entity.m_componentBits)),
	m_systemBits(std::move(entity.m_systemBits)),
	m_id(entity.m_id),
	m_world(entity.m_world),
	m_enabled(entity.m_enabled),
	m_valid(entity.m_valid)
	{
	}

	/*!
	* \brief Constructs a Entity object linked to a world and with an id
	*
	* \param world World in which the entity interact
	* \param id Identifier of the entity
	*/

	Entity::Entity(World* world, EntityId id) :
	m_id(id),
	m_world(world)
	{
	}

	/*!
	* \brief Destructs the object and calls Destroy
	*
	* \see Destroy
	*/

	Entity::~Entity()
	{
		Destroy();
	}

	/*!
	* \brief Adds a component to the entity
	* \return A reference to the newly added component
	*
	* \param componentPtr Component to add to the entity
	*
	* \remark Produces a NazaraAssert if component is nullptr
	*/

	BaseComponent& Entity::AddComponent(std::unique_ptr<BaseComponent>&& componentPtr)
	{
		NazaraAssert(componentPtr, "Component must be valid");

		ComponentIndex index = componentPtr->GetIndex();

		// We ensure that the vector has enough space
		if (index >= m_components.size())
			m_components.resize(index + 1);

		// Affectation and return of the component
		m_components[index] = std::move(componentPtr);
		m_componentBits.UnboundedSet(index);
		m_removedComponentBits.UnboundedReset(index);

		Invalidate();

		// We get the new component and we alert other existing components of the new one
		BaseComponent& component = *m_components[index].get();
		component.SetEntity(this);

		for (std::size_t i = m_componentBits.FindFirst(); i != m_componentBits.npos; i = m_componentBits.FindNext(i))
		{
			if (i != index)
				m_components[i]->OnComponentAttached(component);
		}

		return component;
	}

	/*!
	* \brief Clones the entity
	* \return The clone newly created
	*
	* \remark The close is enable by default, even if the original is disabled
	* \remark Produces a NazaraAssert if the entity is not valid
	*/

	const EntityHandle& Entity::Clone() const
	{
		NazaraAssert(IsValid(), "Invalid entity");

		return m_world->CloneEntity(m_id);
	}

	/*!
	* \brief Kills the entity
	*/

	void Entity::Kill()
	{
		m_world->KillEntity(this);
	}

	/*!
	* \brief Invalidates the entity
	*/

	void Entity::Invalidate()
	{
		// We alert everyone that we have been updated
		m_world->Invalidate(m_id);
	}

	/*!
	* \brief Creates the entity
	*/

	void Entity::Create()
	{
		m_enabled = true;
		m_valid = true;
	}

	/*!
	* \brief Destroys the entity
	*/

	void Entity::Destroy()
	{
		// We prepare components for entity destruction (some components needs this to handle some final callbacks while the entity is still valid)
		for (std::size_t i = m_componentBits.FindFirst(); i != m_componentBits.npos; i = m_componentBits.FindNext(i))
			m_components[i]->OnEntityDestruction();

		// We alert each system
		for (std::size_t index = m_systemBits.FindFirst(); index != m_systemBits.npos; index = m_systemBits.FindNext(index))
		{
			auto sysIndex = static_cast<Ndk::SystemIndex>(index);

			if (m_world->HasSystem(sysIndex))
			{
				BaseSystem& system = m_world->GetSystem(sysIndex);
				system.RemoveEntity(this);
			}
		}
		m_systemBits.Clear();

		// We properly destroy each component
		for (std::size_t i = m_componentBits.FindFirst(); i != m_componentBits.npos; i = m_componentBits.FindNext(i))
			m_components[i]->SetEntity(nullptr);

		m_components.clear();
		m_componentBits.Reset();

		// And then free every handle
		UnregisterAllHandles();

		m_valid = false;
	}

	/*!
	* \brief Destroys a component by index
	*
	* \param index Index of the component
	*
	* \remark If component is not available, no action is performed
	*/

	void Entity::DestroyComponent(ComponentIndex index)
	{
		if (HasComponent(index))
		{
			// We get the component and we alert existing components of the deleted one
			BaseComponent& component = *m_components[index].get();
			for (std::size_t i = m_componentBits.FindFirst(); i != m_componentBits.npos; i = m_componentBits.FindNext(i))
			{
				if (i != index)
					m_components[i]->OnComponentDetached(component);
			}

			component.SetEntity(nullptr);

			m_components[index].reset();
			m_componentBits.Reset(index);
		}
	}

}
