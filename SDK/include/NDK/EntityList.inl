// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/EntityList.hpp>
#include <Nazara/Core/Error.hpp>
#include <algorithm>
#include "EntityList.hpp"

namespace Ndk
{
	/*!
	* \ingroup NDK
	* \class Ndk::EntityList
	* \brief NDK class that represents a set of entities to help performing batch operations
	*/

	/*!
	* \brief Construct a new entity list
	*/
	inline EntityList::EntityList() :
	m_world(nullptr)
	{
	}

	/*!
	* \brief Construct a new entity list by copying another one
	*/
	inline EntityList::EntityList(const EntityList& entityList) :
	m_entityBits(entityList.m_entityBits),
	m_world(entityList.m_world)
	{
	}

	/*!
	* \brief Construct a new entity list by moving a list into this one
	*/
	inline EntityList::EntityList(EntityList&& entityList) noexcept :
	m_entityBits(std::move(entityList.m_entityBits)),
	m_world(entityList.m_world)
	{
	}


	/*!
	* \brief Clears the set from every entities
	*
	* \remark This resets the implicit world member, allowing you to insert entities from a different world than previously
	*/
	inline void EntityList::Clear()
	{
		m_entityBits.Clear();
		m_world = nullptr;
	}

	/*!
	* \brief Checks whether or not the EntityList contains the entity
	* \return true If it is the case
	*
	* \param entity Pointer to the entity
	*
	* \remark If the Insert function was called since the EntityList construction (or last call to Clear), the entity passed by parameter must belong to the same world as the previously inserted entities.
	*/
	inline bool EntityList::Has(const Entity* entity) const
	{
		NazaraAssert(!m_world || !entity || entity->GetWorld() == m_world, "Incompatible world");

		return entity && entity->IsValid() && Has(entity->GetId());
	}

	/*!
	* \brief Checks whether or not the set contains the entity by id
	* \return true If it is the case
	*
	* \param id Identifier of the entity
	*/
	inline bool EntityList::Has(EntityId entity) const
	{
		return m_entityBits.UnboundedTest(entity);
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
	* \remark It's up to the programmer to remove an entity from this list before its deletion
	*/
	inline void EntityList::Insert(Entity* entity)
	{
		NazaraAssert(entity, "Invalid entity");
		NazaraAssert(!m_world || entity->GetWorld() == m_world, "Incompatible world");

		m_entityBits.UnboundedSet(entity->GetId(), true);
		m_world = entity->GetWorld();
	}

	/*!
	* \brief Removes the entity from the set
	*
	* \param entity Pointer to the entity
	*
	* \remark If entity is not contained, no action is performed
	* \remark This function never resets the implicit world member, even if it empties the list. Use the Clear method if you want to reset it.
	*
	* \see Clear
	*/
	inline void EntityList::Remove(Entity* entity)
	{
		m_entityBits.UnboundedSet(entity->GetId(), false);
	}

	// STL Interface
	inline EntityList::iterator EntityList::begin() const
	{
		return EntityList::iterator(this, m_entityBits.FindFirst());
	}

	inline bool EntityList::empty() const
	{
		return !m_entityBits.TestAny();
	}

	inline EntityList::iterator EntityList::end() const
	{
		return EntityList::iterator(this, m_entityBits.npos);
	}

	inline EntityList::size_type EntityList::size() const
	{
		return m_entityBits.Count();
	}

	inline EntityList& EntityList::operator=(const EntityList& entityList)
	{
		m_entityBits = entityList.m_entityBits;
		m_world = entityList.m_world;

		return *this;
	}

	inline EntityList& EntityList::operator=(EntityList && entityList) noexcept
	{
		m_entityBits = std::move(entityList.m_entityBits);
		m_world = entityList.m_world;

		return *this;
	}

	inline std::size_t EntityList::FindNext(std::size_t currentId) const
	{
		return m_entityBits.FindNext(currentId);
	}

	inline World* EntityList::GetWorld() const
	{
		return m_world;
	}


	inline EntityList::iterator::iterator(const EntityList* list, std::size_t nextId) :
	m_nextEntityId(nextId),
	m_list(list)
	{
	}

	inline EntityList::iterator::iterator(const iterator& iterator) :
	m_nextEntityId(iterator.m_nextEntityId),
	m_list(iterator.m_list)
	{
	}

	inline EntityList::iterator& EntityList::iterator::operator=(const iterator& iterator)
	{
		m_nextEntityId = iterator.m_nextEntityId;
		m_list = iterator.m_list;

		return *this;
	}

	inline EntityList::iterator& EntityList::iterator::operator++()
	{
		m_nextEntityId = m_list->FindNext(m_nextEntityId);

		return *this;
	}

	inline EntityList::iterator EntityList::iterator::operator++(int)
	{
		std::size_t previousId = m_nextEntityId;

		m_nextEntityId = m_list->FindNext(m_nextEntityId);

		return iterator(m_list, previousId);
	}

	inline bool operator==(const EntityList::iterator& lhs, const EntityList::iterator& rhs)
	{
		NazaraAssert(lhs.m_list == rhs.m_list, "Cannot compare iterator coming from different lists");

		return lhs.m_nextEntityId == rhs.m_nextEntityId;
	}

	inline bool operator!=(const EntityList::iterator& lhs, const EntityList::iterator& rhs)
	{
		return !operator==(lhs, rhs);
	}

	inline void swap(EntityList::iterator& lhs, EntityList::iterator& rhs)
	{
		NazaraAssert(lhs.m_list == rhs.m_list, "Cannot compare iterator coming from different lists");

		using std::swap;

		std::swap(lhs.m_nextEntityId, rhs.m_nextEntityId);
	}
}
