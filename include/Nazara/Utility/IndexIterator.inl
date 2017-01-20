// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/IndexMapper.hpp>
#include <iterator>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	inline IndexIterator::IndexIterator() :
	m_mapper(nullptr),
	m_index(0)
	{
	}

	inline IndexIterator::IndexIterator(const IndexIterator& iterator) :
	m_mapper(iterator.m_mapper),
	m_index(iterator.m_index)
	{
	}

	inline IndexIterator::IndexIterator(IndexMapper* mapper, unsigned int index) :
	m_mapper(mapper),
	m_index(index)
	{
	}

	inline IndexIterator::Reference IndexIterator::operator*() const
	{
		return Reference(m_mapper, m_index);
	}

	inline IndexIterator::Reference IndexIterator::operator[](unsigned int index) const
	{
		return Reference(m_mapper, m_index+index);
	}

	inline IndexIterator& IndexIterator::operator=(const IndexIterator& iterator)
	{
		m_mapper = iterator.m_mapper;
		m_index = iterator.m_index;

		return *this;
	}

	inline IndexIterator IndexIterator::operator+(unsigned int indexCount) const
	{
		return IndexIterator(m_mapper, m_index + indexCount);
	}

	inline IndexIterator IndexIterator::operator-(unsigned int indexCount) const
	{
		return IndexIterator(m_mapper, m_index - indexCount);
	}

	inline IndexIterator& IndexIterator::operator+=(unsigned int indexCount)
	{
		m_index += indexCount;

		return *this;
	}

	inline IndexIterator& IndexIterator::operator-=(unsigned int indexCount)
	{
		m_index += indexCount;

		return *this;
	}

	inline IndexIterator& IndexIterator::operator++()
	{
		m_index++;

		return *this;
	}

	inline IndexIterator IndexIterator::operator++(int)
	{
		IndexIterator iterator(*this);
		operator++();

		return iterator;
	}

	inline IndexIterator& IndexIterator::operator--()
	{
		m_index--;

		return *this;
	}

	inline IndexIterator IndexIterator::operator--(int)
	{
		IndexIterator iterator(*this);
		operator--();

		return iterator;
	}

	inline bool operator==(const IndexIterator& lhs, const IndexIterator& rhs)
	{
		return lhs.m_mapper == rhs.m_mapper && lhs.m_index == rhs.m_index;
	}

	inline bool operator!=(const IndexIterator& lhs, const IndexIterator& rhs)
	{
		return !operator==(lhs, rhs);
	}

	inline bool operator<(const IndexIterator& lhs, const IndexIterator& rhs)
	{
		if (lhs.m_mapper == rhs.m_mapper)
			return lhs.m_index < rhs.m_index;
		else
			return lhs.m_mapper < rhs.m_mapper;
	}

	inline bool operator<=(const IndexIterator& lhs, const IndexIterator& rhs)
	{
		return !operator<(rhs, lhs);
	}

	inline bool operator>(const IndexIterator& lhs, const IndexIterator& rhs)
	{
		return operator<(rhs, lhs);
	}

	inline bool operator>=(const IndexIterator& lhs, const IndexIterator& rhs)
	{
		return !operator<(lhs, rhs);
	}

	/**************************IndexIterator::Reference*************************/

	inline IndexIterator::Reference::Reference(IndexMapper* mapper, unsigned int index) :
	m_mapper(mapper),
	m_index(index)
	{
	}

	inline IndexIterator::Reference& IndexIterator::Reference::operator=(UInt32 value)
	{
		m_mapper->Set(m_index, value);

		return *this;
	}

	inline IndexIterator::Reference& IndexIterator::Reference::operator=(const IndexIterator::Reference& reference)
	{
		m_mapper->Set(m_index, reference); // Conversion implicite en UInt32

		return *this;
	}

	inline IndexIterator::Reference::operator UInt32() const
	{
		return m_mapper->Get(m_index);
	}
}

namespace std
{
	template<>
	struct iterator_traits<Nz::IndexIterator>
	{
		using difference_type = ptrdiff_t;
		using iterator_category = random_access_iterator_tag;
		using reference = const Nz::IndexIterator::Reference&;
		using pointer = const Nz::IndexIterator::Reference*;
		using value_type = Nz::IndexIterator::Reference;
	};
}

#include <Nazara/Core/DebugOff.hpp>
