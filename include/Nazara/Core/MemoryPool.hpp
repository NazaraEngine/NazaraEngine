// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_MEMORYPOOL_HPP
#define NAZARA_CORE_MEMORYPOOL_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Bitset.hpp>
#include <iterator>
#include <memory>
#include <vector>

namespace Nz
{
	template<typename T, std::size_t Alignment = alignof(T)>
	class MemoryPool
	{
		public:
			class iterator;
			friend iterator;

			MemoryPool(std::size_t blockSize);
			MemoryPool(const MemoryPool&) = delete;
			MemoryPool(MemoryPool&&) noexcept = default;
			~MemoryPool();

			template<typename... Args> T* Allocate(std::size_t& index, Args&&... args);

			void Clear();

			void Free(std::size_t index);

			std::size_t GetAllocatedEntryCount() const;
			std::size_t GetBlockCount() const;
			std::size_t GetBlockSize() const;
			std::size_t GetFreeEntryCount() const;

			void Reset();

			T* RetrieveFromIndex(std::size_t index);
			std::size_t RetrieveEntryIndex(const T* data);

			// std interface
			iterator begin();
			iterator end();
			std::size_t size();

			MemoryPool& operator=(const MemoryPool&) = delete;
			MemoryPool& operator=(MemoryPool&& pool) noexcept = default;

			static constexpr std::size_t InvalidIndex = std::numeric_limits<std::size_t>::max();

		private:
			void AllocateBlock();
			T* GetAllocatedPointer(std::size_t blockIndex, std::size_t localIndex);
			std::pair<std::size_t, std::size_t> GetFirstAllocatedEntry() const;
			std::pair<std::size_t, std::size_t> GetFirstAllocatedEntryFromBlock(std::size_t blockIndex) const;
			std::pair<std::size_t, std::size_t> GetNextAllocatedEntry(std::size_t blockIndex, std::size_t localIndex) const;

			using AlignedStorage = std::aligned_storage_t<sizeof(T), Alignment>;

			struct Block
			{
				std::size_t occupiedEntryCount = 0;
				std::unique_ptr<AlignedStorage[]> memory;
				Bitset<UInt64> freeEntries;
				Bitset<UInt64> occupiedEntries; //< Opposite of freeEntries
			};

			std::size_t m_blockSize;
			std::vector<Block> m_blocks;
	};

	template<typename T, std::size_t Alignment>
	class MemoryPool<T, Alignment>::iterator
	{
		friend MemoryPool;

		public:
			using iterator_category = std::input_iterator_tag;
			using value_type = T;
			using difference_type = std::ptrdiff_t;
			using pointer = T*;
			using reference = T&;

			iterator(const iterator&) = default;
			iterator(iterator&&) = default;

			iterator& operator=(const iterator&) = default;
			iterator& operator=(iterator&&) = default;

			iterator operator++(int);
			iterator& operator++();

			bool operator==(const iterator& rhs) const;
			bool operator!=(const iterator& rhs) const;
			reference operator*() const;

		private:
			iterator(MemoryPool* owner, std::size_t blockIndex, std::size_t localIndex);

			std::size_t m_blockIndex;
			std::size_t m_localIndex;
			MemoryPool* m_owner;
	};
}

#include <Nazara/Core/MemoryPool.inl>

#endif // NAZARA_CORE_MEMORYPOOL_HPP
