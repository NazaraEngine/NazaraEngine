// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MEMORYPOOL_HPP
#define NAZARA_MEMORYPOOL_HPP

#include <Nazara/Prerequisites.hpp>
#include <atomic>
#include <memory>

namespace Nz
{
	class MemoryPool
	{
		public:
			MemoryPool(unsigned int blockSize, unsigned int size = 1024, bool canGrow = true);
			MemoryPool(const MemoryPool&) = delete;
			MemoryPool(MemoryPool&& pool) noexcept;
			~MemoryPool() = default;

			void* Allocate(unsigned int size);

			template<typename T> void Delete(T* ptr);

			void Free(void* ptr);

			inline unsigned int GetBlockSize() const;
			inline unsigned int GetFreeBlocks() const;
			inline unsigned int GetSize() const;

			template<typename T, typename... Args> T* New(Args&&... args);

			MemoryPool& operator=(const MemoryPool&) = delete;
			MemoryPool& operator=(MemoryPool&& pool) noexcept;

		private:
			MemoryPool(MemoryPool* pool);

			std::unique_ptr<void* []> m_freeList;
			std::unique_ptr<UInt8[]> m_pool;
			std::unique_ptr<MemoryPool> m_next;
			std::atomic_uint m_freeCount;
			MemoryPool* m_previous;
			bool m_canGrow;
			unsigned int m_blockSize;
			unsigned int m_size;
	};
}

#include <Nazara/Core/MemoryPool.inl>

#endif // NAZARA_MEMORYPOOL_HPP
