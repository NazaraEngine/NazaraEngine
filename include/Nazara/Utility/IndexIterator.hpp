// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_INDEXITERATOR_HPP
#define NAZARA_INDEXITERATOR_HPP

#include <Nazara/Prerequesites.hpp>

namespace Nz
{
	class IndexMapper;

	class IndexIterator
	{
		friend IndexMapper;

		public:
			class Reference;

			IndexIterator();
			IndexIterator(const IndexIterator& iterator);
			~IndexIterator() = default;

			Reference operator*() const;

			Reference operator[](unsigned int index) const;

			IndexIterator& operator=(const IndexIterator& iterator);

			IndexIterator operator+(unsigned int indexCount) const;
			IndexIterator operator-(unsigned int indexCount) const;

			IndexIterator& operator+=(unsigned int indexCount);
			IndexIterator& operator-=(unsigned int indexCount);

			IndexIterator& operator++();
			IndexIterator operator++(int);

			IndexIterator& operator--();
			IndexIterator operator--(int);

			friend bool operator==(const IndexIterator& lhs, const IndexIterator& rhs);
			friend bool operator!=(const IndexIterator& lhs, const IndexIterator& rhs);
			friend bool operator<(const IndexIterator& lhs, const IndexIterator& rhs);
			friend bool operator<=(const IndexIterator& lhs, const IndexIterator& rhs);
			friend bool operator>(const IndexIterator& lhs, const IndexIterator& rhs);
			friend bool operator>=(const IndexIterator& lhs, const IndexIterator& rhs);

		private:
			IndexIterator(IndexMapper* mapper, unsigned int index);

			IndexMapper* m_mapper;
			unsigned int m_index;
	};

	class IndexIterator::Reference
	{
		friend IndexIterator;

		public:
			Reference(const Reference& reference) = default;
			~Reference() = default;

			Reference& operator=(UInt32 value);
			Reference& operator=(const Reference& reference);

			operator UInt32() const;

		private:
			Reference(IndexMapper* mapper, unsigned int index);

			IndexMapper* m_mapper;
			unsigned int m_index;
	};
}

#include <Nazara/Utility/IndexIterator.inl>

#endif // NAZARA_INDEXITERATOR_HPP
