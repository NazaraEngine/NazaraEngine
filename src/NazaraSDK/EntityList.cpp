// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NazaraSDK/EntityList.hpp>
#include <NazaraSDK/World.hpp>

namespace Ndk
{
	/*!
	* \brief Construct a new entity list by copying another one
	*/
	EntityList::EntityList(const EntityList& entityList) :
	m_entityBits(entityList.m_entityBits),
	m_world(entityList.m_world)
	{
		for (const Ndk::EntityHandle& entity : *this)
			entity->RegisterEntityList(this);

		if (m_world)
			m_world->RegisterEntityList(this);
	}

	/*!
	* \brief Construct a new entity list by moving a list into this one
	*/
	EntityList::EntityList(EntityList&& entityList) noexcept :
	m_entityBits(std::move(entityList.m_entityBits)),
	m_world(entityList.m_world)
	{
		for (const Ndk::EntityHandle& entity : *this)
		{
			entity->UnregisterEntityList(&entityList);
			entity->RegisterEntityList(this);
		}

		if (m_world)
		{
			m_world->UnregisterEntityList(&entityList);
			m_world->RegisterEntityList(this);

			entityList.m_world = nullptr;
		}
	}

	EntityList::~EntityList()
	{
		for (const Ndk::EntityHandle& entity : *this)
			entity->UnregisterEntityList(this);

		if (m_world)
			m_world->UnregisterEntityList(this);
	}


	/*!
	* \brief Clears the set from every entities
	*
	* \remark This resets the implicit world member, allowing you to insert entities from a different world than previously
	*/
	void EntityList::Clear()
	{
		for (const Ndk::EntityHandle& entity : *this)
			entity->UnregisterEntityList(this);

		m_entityBits.Clear();

		if (m_world)
		{
			m_world->UnregisterEntityList(this);
			m_world = nullptr;
		}
	}

	/*!
	* \brief Inserts the entity into the set
	*
	* Marks an entity as present in this entity list, it must belongs to the same world as others entities contained in this list.
	*
	* \param entity Valid pointer to an entity
	*
	* \remark If entity is already contained, no action is performed
	* \remark If any entity has been inserted since construction (or last Clear call), the entity must belong to the same world as the previously inserted entities
	*/
	void EntityList::Insert(Entity* entity)
	{
		NazaraAssert(entity, "Invalid entity");

		if (!Has(entity))
		{
			entity->RegisterEntityList(this);

			m_entityBits.UnboundedSet(entity->GetId(), true);
			if (!m_world)
			{
				m_world = entity->GetWorld();
				m_world->RegisterEntityList(this);
			}
		}
	}

	EntityList& EntityList::operator=(const EntityList& entityList)
	{
		if (m_world)
			m_world->UnregisterEntityList(this);

		for (const Ndk::EntityHandle& entity : *this)
			entity->UnregisterEntityList(this);

		m_entityBits = entityList.m_entityBits;
		m_world = entityList.m_world;

		for (const Ndk::EntityHandle& entity : *this)
			entity->RegisterEntityList(this);

		if (m_world)
			m_world->RegisterEntityList(this);

		return *this;
	}

	EntityList& EntityList::operator=(EntityList&& entityList) noexcept
	{
		if (this == &entityList)
			return *this;

		if (m_world)
			m_world->UnregisterEntityList(this);

		for (const Ndk::EntityHandle& entity : *this)
			entity->UnregisterEntityList(this);

		m_entityBits = std::move(entityList.m_entityBits);
		m_world = entityList.m_world;

		if (m_world)
		{
			m_world->UnregisterEntityList(&entityList);
			m_world->RegisterEntityList(this);

			entityList.m_world = nullptr;
		}

		for (const Ndk::EntityHandle& entity : *this)
		{
			entity->UnregisterEntityList(&entityList);
			entity->RegisterEntityList(this);
		}

		return *this;
	}

	const EntityHandle& EntityList::iterator::operator*() const
	{
		return m_list->GetWorld()->GetEntity(static_cast<EntityId>(m_nextEntityId));
	}
}
