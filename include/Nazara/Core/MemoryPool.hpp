// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_MEMORYPOOL_HPP
#define NAZARA_CORE_MEMORYPOOL_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Bitset.hpp>
#include <memory>
#include <vector>

namespace Nz
{
	template<typename T, std::size_t Alignment = alignof(T)>
	class MemoryPool
	{
		public:
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

			std::size_t RetrieveEntryIndex(const T* data);

			MemoryPool& operator=(const MemoryPool&) = delete;
			MemoryPool& operator=(MemoryPool&& pool) noexcept = default;

			static constexpr std::size_t InvalidIndex = std::numeric_limits<std::size_t>::max();

		private:
			void AllocateBlock();

			using AlignedStorage = std::aligned_storage_t<sizeof(T), Alignment>;

			struct Block
			{
				std::size_t occupiedEntryCount = 0;
				std::unique_ptr<AlignedStorage[]> memory;
				Bitset<UInt64> freeEntries;
			};

			std::size_t m_blockSize;
			std::vector<Block> m_blocks;
	};
}

#include <Nazara/Core/MemoryPool.inl>

#endif // NAZARA_CORE_MEMORYPOOL_HPP
