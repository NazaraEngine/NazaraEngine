// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MEMORYPOOL_HPP
#define NAZARA_MEMORYPOOL_HPP

#include <Nazara/Prerequesites.hpp>
#include <memory>

template<unsigned int typeSize, unsigned int count = 1000, bool canGrow = true>
class NzMemoryPool
{
	public:
		NzMemoryPool();
		~NzMemoryPool() = default;

		template<typename T> T* Allocate();
		void* Allocate(unsigned int size);
		void Free(void* ptr);

	private:
		NzMemoryPool(NzMemoryPool* pool);

		std::unique_ptr<void*[]> m_freeList;
		std::unique_ptr<nzUInt8[]> m_pool;
		std::unique_ptr<NzMemoryPool> m_next;
		NzMemoryPool* m_previous;
		unsigned int m_freeCount;
};

#include <Nazara/Core/MemoryPool.inl>

#endif // NAZARA_MEMORYPOOL_HPP
