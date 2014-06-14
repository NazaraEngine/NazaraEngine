// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MEMORYPOOL_HPP
#define NAZARA_MEMORYPOOL_HPP

#include <Nazara/Prerequesites.hpp>
#include <atomic>
#include <memory>

template<unsigned int blockSize, bool canGrow = true>
class NzMemoryPool
{
	public:
		NzMemoryPool(unsigned int size = 1024);
		~NzMemoryPool() = default;

		template<typename T> T* Allocate();
		void* Allocate(unsigned int size);

		void Free(void* ptr);

		unsigned int GetFreeBlocks() const;
		unsigned int GetSize() const;

	private:
		NzMemoryPool(NzMemoryPool* pool);

		std::unique_ptr<void*[]> m_freeList;
		std::unique_ptr<nzUInt8[]> m_pool;
		std::unique_ptr<NzMemoryPool> m_next;
		std::atomic_uint m_freeCount;
		NzMemoryPool* m_previous;
		unsigned int m_size;
};

#include <Nazara/Core/MemoryPool.inl>

#endif // NAZARA_MEMORYPOOL_HPP
