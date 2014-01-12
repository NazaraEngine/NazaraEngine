// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_INDEXITERATOR_HPP
#define NAZARA_INDEXITERATOR_HPP

#include <Nazara/Prerequesites.hpp>

class NzIndexMapper;

class NzIndexIterator
{
	friend NzIndexMapper;

	public:
		class Reference;

		NzIndexIterator();
		NzIndexIterator(const NzIndexIterator& iterator);
		~NzIndexIterator() = default;

		Reference operator*() const;

		Reference operator[](unsigned int index) const;

		NzIndexIterator& operator=(const NzIndexIterator& iterator);

		NzIndexIterator operator+(unsigned int indexCount);
		NzIndexIterator operator-(unsigned int indexCount);

		NzIndexIterator& operator+=(unsigned int indexCount);
		NzIndexIterator& operator-=(unsigned int indexCount);

		NzIndexIterator& operator++();
		NzIndexIterator operator++(int);

		NzIndexIterator& operator--();
		NzIndexIterator operator--(int);

		friend bool operator==(const NzIndexIterator& lhs, const NzIndexIterator& rhs);
		friend bool operator!=(const NzIndexIterator& lhs, const NzIndexIterator& rhs);
		friend bool operator<(const NzIndexIterator& lhs, const NzIndexIterator& rhs);
		friend bool operator<=(const NzIndexIterator& lhs, const NzIndexIterator& rhs);
		friend bool operator>(const NzIndexIterator& lhs, const NzIndexIterator& rhs);
		friend bool operator>=(const NzIndexIterator& lhs, const NzIndexIterator& rhs);

	private:
		NzIndexIterator(NzIndexMapper* mapper, unsigned int index);

		NzIndexMapper* m_mapper;
		unsigned int m_index;
};

class NzIndexIterator::Reference
{
	friend NzIndexIterator;

	public:
		Reference(const Reference& reference) = default;
		~Reference() = default;

		Reference& operator=(nzUInt32 value);
		Reference& operator=(const Reference& reference);

		operator nzUInt32() const;

	private:
		Reference(NzIndexMapper* mapper, unsigned int index);

		NzIndexMapper* m_mapper;
		unsigned int m_index;
};

#include <Nazara/Utility/IndexIterator.inl>

#endif // NAZARA_INDEXITERATOR_HPP
