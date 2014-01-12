// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/IndexMapper.hpp>
#include <Nazara/Utility/Debug.hpp>

inline NzIndexIterator::NzIndexIterator() :
m_mapper(nullptr),
m_index(0)
{
}

inline NzIndexIterator::NzIndexIterator(const NzIndexIterator& iterator) :
m_mapper(iterator.m_mapper),
m_index(iterator.m_index)
{
}

inline NzIndexIterator::NzIndexIterator(NzIndexMapper* mapper, unsigned int index) :
m_mapper(mapper),
m_index(index)
{
}

inline NzIndexIterator::Reference NzIndexIterator::operator*() const
{
	return Reference(m_mapper, m_index);
}

inline NzIndexIterator::Reference NzIndexIterator::operator[](unsigned int index) const
{
	return Reference(m_mapper, m_index+index);
}

inline NzIndexIterator& NzIndexIterator::operator=(const NzIndexIterator& iterator)
{
	m_mapper = iterator.m_mapper;
	m_index = iterator.m_index;

	return *this;
}

inline NzIndexIterator NzIndexIterator::operator+(unsigned int indexCount)
{
	return NzIndexIterator(m_mapper, m_index + indexCount);
}

inline NzIndexIterator NzIndexIterator::operator-(unsigned int indexCount)
{
	return NzIndexIterator(m_mapper, m_index - indexCount);
}

inline NzIndexIterator& NzIndexIterator::operator+=(unsigned int indexCount)
{
	m_index += indexCount;

	return *this;
}

inline NzIndexIterator& NzIndexIterator::operator-=(unsigned int indexCount)
{
	m_index += indexCount;

	return *this;
}

inline NzIndexIterator& NzIndexIterator::operator++()
{
	m_index++;

	return *this;
}

inline NzIndexIterator NzIndexIterator::operator++(int)
{
	NzIndexIterator iterator(*this);
	operator++();

	return iterator;
}

inline NzIndexIterator& NzIndexIterator::operator--()
{
	m_index--;

	return *this;
}

inline NzIndexIterator NzIndexIterator::operator--(int)
{
	NzIndexIterator iterator(*this);
	operator--();

	return iterator;
}

inline bool operator==(const NzIndexIterator& lhs, const NzIndexIterator& rhs)
{
	return lhs.m_mapper == rhs.m_mapper && lhs.m_index == rhs.m_index;
}

inline bool operator!=(const NzIndexIterator& lhs, const NzIndexIterator& rhs)
{
	return !operator==(lhs, rhs);
}

inline bool operator<(const NzIndexIterator& lhs, const NzIndexIterator& rhs)
{
	if (lhs.m_mapper == rhs.m_mapper)
		return lhs.m_index < rhs.m_index;
	else
		return lhs.m_mapper < rhs.m_mapper;
}

inline bool operator<=(const NzIndexIterator& lhs, const NzIndexIterator& rhs)
{
	return !operator<(rhs, lhs);
}

inline bool operator>(const NzIndexIterator& lhs, const NzIndexIterator& rhs)
{
	return operator<(rhs, lhs);
}

inline bool operator>=(const NzIndexIterator& lhs, const NzIndexIterator& rhs)
{
	return !operator<(lhs, rhs);
}

/**************************NzIndexIterator::Reference*************************/

inline NzIndexIterator::Reference::Reference(NzIndexMapper* mapper, unsigned int index) :
m_mapper(mapper),
m_index(index)
{
}

inline NzIndexIterator::Reference& NzIndexIterator::Reference::operator=(nzUInt32 value)
{
	m_mapper->Set(m_index, value);

	return *this;
}

inline NzIndexIterator::Reference& NzIndexIterator::Reference::operator=(const NzIndexIterator::Reference& reference)
{
	m_mapper->Set(m_index, reference); // Conversion implicite en UInt32

	return *this;
}

inline NzIndexIterator::Reference::operator nzUInt32() const
{
	return m_mapper->Get(m_index);
}

#include <Nazara/Core/DebugOff.hpp>
