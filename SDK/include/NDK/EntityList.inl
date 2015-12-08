// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <Nazara/Core/Error.hpp>
#include <algorithm>

namespace Ndk
{
	inline void EntityList::Clear()
	{
		m_entities.clear();
		m_entityBits.Clear();
	}

	inline bool EntityList::Has(const Entity* entity)
	{
		return entity && entity->IsValid() && Has(entity->GetId());
	}

	inline bool EntityList::Has(EntityId entity)
	{
		return m_entityBits.UnboundedTest(entity);
	}

	inline void EntityList::Insert(Entity* entity)
	{
		if (!Has(entity))
		{
			m_entities.emplace_back(entity);
			m_entityBits.UnboundedSet(entity->GetId(), true);
		}
	}

	inline void EntityList::Remove(Entity* entity)
	{
		if (Has(entity))
		{
			auto it = std::find(m_entities.begin(), m_entities.end(), *entity);
			NazaraAssert(it != m_entities.end(), "Entity should be part of the vector");

			std::swap(*it, m_entities.back());
			m_entities.pop_back(); // On le sort du vector
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
