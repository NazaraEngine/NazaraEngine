// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <Nazara/Core/Error.hpp>
#include <algorithm>

namespace Ndk
{
	/*!
	* \ingroup NDK
	* \class Ndk::EntityList
	* \brief NDK class that represents a set of entities to help performing batch operations
	*/

	/*!
	* \brief Clears the set from every entities
	*/

	inline void EntityList::Clear()
	{
		m_entities.clear();
		m_entityBits.Clear();
	}

	/*!
	* \brief Checks whether or not the set contains the entity
	* \return true If it is the case
	*
	* \param entity Pointer to the entity
	*/

	inline bool EntityList::Has(const Entity* entity)
	{
		return entity && entity->IsValid() && Has(entity->GetId());
	}

	/*!
	* \brief Checks whether or not the set contains the entity by id
	* \return true If it is the case
	*
	* \param id Identifier of the entity
	*/

	inline bool EntityList::Has(EntityId entity)
	{
		return m_entityBits.UnboundedTest(entity);
	}

	/*!
	* \brief Inserts the entity into the set
	*
	* \param entity Pointer to the entity
	*
	* \remark If entity is already contained, no action is performed
	*/

	inline void EntityList::Insert(Entity* entity)
	{
		if (!Has(entity))
		{
			m_entities.emplace_back(entity);
			m_entityBits.UnboundedSet(entity->GetId(), true);
		}
	}

	/*!
	* \brief Removes the entity from the set
	*
	* \param entity Pointer to the entity
	*
	* \remark If entity is not contained, no action is performed
	*/

	inline void EntityList::Remove(Entity* entity)
	{
		if (Has(entity))
		{
			auto it = std::find(m_entities.begin(), m_entities.end(), *entity);
			NazaraAssert(it != m_entities.end(), "Entity should be part of the vector");

			std::swap(*it, m_entities.back());
			m_entities.pop_back(); // We get it out of the vector
			m_entityBits.UnboundedSet(entity->GetId(), false);
		}
	}

	// Nz::Interface STD
	inline EntityList::Container::iterator EntityList::begin()
	{
		return m_entities.begin();
	}

	inline EntityList::Container::const_iterator EntityList::begin() const
	{
		return m_entities.begin();
	}

	inline EntityList::Container::const_iterator EntityList::cbegin() const
	{
		return m_entities.cbegin();
	}

	inline EntityList::Container::const_iterator EntityList::cend() const
	{
		return m_entities.cend();
	}

	inline EntityList::Container::const_reverse_iterator EntityList::crbegin() const
	{
		return m_entities.crbegin();
	}

	inline EntityList::Container::const_reverse_iterator EntityList::crend() const
	{
		return m_entities.crend();
	}

	inline bool EntityList::empty() const
	{
		return m_entities.empty();
	}

	inline EntityList::Container::iterator EntityList::end()
	{
		return m_entities.end();
	}

	inline EntityList::Container::const_iterator EntityList::end() const
	{
		return m_entities.end();
	}

	inline EntityList::Container::reverse_iterator EntityList::rbegin()
	{
		return m_entities.rbegin();
	}

	inline EntityList::Container::const_reverse_iterator EntityList::rbegin() const
	{
		return m_entities.rbegin();
	}

	inline EntityList::Container::reverse_iterator EntityList::rend()
	{
		return m_entities.rend();
	}

	inline EntityList::Container::const_reverse_iterator EntityList::rend() const
	{
		return m_entities.rend();
	}

	inline EntityList::Container::size_type EntityList::size() const
	{
		return m_entities.size();
	}
}
