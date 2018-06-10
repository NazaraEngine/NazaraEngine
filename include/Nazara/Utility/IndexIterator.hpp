// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_INDEXITERATOR_HPP
#define NAZARA_INDEXITERATOR_HPP

#include <Nazara/Prerequisites.hpp>

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

			Reference operator[](std::size_t index) const;

			IndexIterator& operator=(const IndexIterator& iterator);

			IndexIterator operator+(std::size_t indexCount) const;
			IndexIterator operator-(std::size_t indexCount) const;

			IndexIterator& operator+=(std::size_t indexCount);
			IndexIterator& operator-=(std::size_t indexCount);

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
			IndexIterator(IndexMapper* mapper, std::size_t index);

			IndexMapper* m_mapper;
			std::size_t m_index;
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
			Reference(IndexMapper* mapper, std::size_t index);

			IndexMapper* m_mapper;
			std::size_t m_index;
	};
}

#include <Nazara/Utility/IndexIterator.inl>

#endif // NAZARA_INDEXITERATOR_HPP
