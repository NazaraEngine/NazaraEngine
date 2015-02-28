// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/MemoryHelper.hpp>
#include <utility>
#include <Nazara/Core/Debug.hpp>

inline NzMemoryPool::NzMemoryPool(unsigned int blockSize, unsigned int size, bool canGrow) :
m_freeCount(size),
m_previous(nullptr),
m_canGrow(canGrow),
m_blockSize(blockSize),
m_size(size)
{
	m_pool.reset(new nzUInt8[blockSize * size]);
	m_freeList.reset(new void*[size]);

	// Remplissage de la free list
	for (unsigned int i = 0; i < size; ++i)
		m_freeList[i] = &m_pool[m_blockSize * (size-i-1)];
}

inline NzMemoryPool::NzMemoryPool(NzMemoryPool&& pool) noexcept
{
	operator=(std::move(pool));
}

inline NzMemoryPool::NzMemoryPool(NzMemoryPool* pool) :
NzMemoryPool(pool->m_blockSize, pool->m_size, pool->m_canGrow)
{
	m_previous = pool;
}

inline void* NzMemoryPool::Allocate(unsigned int size)
{
	if (size <= m_blockSize)
	{
		if (m_freeCount > 0)
			return m_freeList[--m_freeCount];
		else if (m_canGrow)
		{
			if (!m_next)
				m_next.reset(new NzMemoryPool(this));

			return m_next->Allocate(size);
		}
	}

	return NzOperatorNew(size);
}

template<typename T>
inline void NzMemoryPool::Delete(T* ptr)
{
	if (ptr)
	{
		ptr->~T();
		Free(ptr);
	}
}

inline void NzMemoryPool::Free(void* ptr)
{
	if (ptr)
	{
		// Le pointeur nous appartient-il ?
		nzUInt8* freePtr = static_cast<nzUInt8*>(ptr);
		nzUInt8* poolPtr = m_pool.get();
		if (freePtr >= poolPtr && freePtr < poolPtr + m_blockSize*m_size)
		{
			#if NAZARA_CORE_SAFE
			if ((freePtr - poolPtr) % m_blockSize != 0)
				throw std::runtime_error("Invalid pointer (does not point to an element of the pool)");
			#endif

			m_freeList[m_freeCount++] = ptr;

			// Si nous sommes vide et l'extension d'un autre pool, nous nous suicidons
			if (m_freeCount == m_size && m_previous && !m_next)
			{
				m_previous->m_next.release();
				delete this; // Suicide
			}
		}
		else
		{
			if (m_next)
				m_next->Free(ptr);
			else
				NzOperatorDelete(ptr);
		}
	}
}

inline unsigned int NzMemoryPool::GetBlockSize() const
{
	return m_blockSize;
}

inline unsigned int NzMemoryPool::GetFreeBlocks() const
{
	return m_freeCount;
}

inline unsigned int NzMemoryPool::GetSize() const
{
	return m_size;
}

template<typename T, typename... Args>
inline T* NzMemoryPool::New(Args&&... args)
{
	T* object = static_cast<T*>(Allocate(sizeof(T)));
	NzPlacementNew<T>(object, std::forward<Args>(args)...);

	return object;
}

inline NzMemoryPool& NzMemoryPool::operator=(NzMemoryPool&& pool) noexcept
{
	m_blockSize = m_blockSize;
	m_canGrow = m_canGrow;
	m_freeCount = m_freeCount.load(std::memory_order_relaxed);
	m_freeList = std::move(m_freeList);
	m_pool = std::move(m_pool);
	m_previous = m_previous;
	m_next = std::move(m_next);
	m_size = m_size;

	// Si nous avons été créés par un autre pool, nous devons le faire pointer vers nous de nouveau
	if (m_previous)
	{
		m_previous->m_next.release();
		m_previous->m_next.reset(this);
	}

	return *this;
}

#include <Nazara/Core/DebugOff.hpp>
