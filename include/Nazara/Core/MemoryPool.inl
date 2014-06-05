// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <new>
#include <Nazara/Core/Debug.hpp>

template<unsigned int typeSize, unsigned int count, bool canGrow>
NzMemoryPool<typeSize, count, canGrow>::NzMemoryPool() :
m_previous(nullptr),
m_freeCount(count)
{
	m_pool.reset(new nzUInt8[typeSize * count]);
	m_freeList.reset(new void*[count]);

	// Remplissage de la free list
	for (unsigned int i = 0; i < count; ++i)
		m_freeList[i] = &m_pool[typeSize * (count-i-1)];
}

template<unsigned int typeSize, unsigned int count, bool canGrow>
NzMemoryPool<typeSize, count, canGrow>::NzMemoryPool(NzMemoryPool* pool) :
NzMemoryPool()
{
	m_previous = pool;
}

template<unsigned int typeSize, unsigned int count, bool canGrow>
template<typename T>
T* NzMemoryPool<typeSize, count, canGrow>::Allocate()
{
	static_assert(sizeof(T) <= typeSize, "This type is too large for this memory pool");

	return static_cast<T*>(Allocate(sizeof(T)));
}

template<unsigned int typeSize, unsigned int count, bool canGrow>
void* NzMemoryPool<typeSize, count, canGrow>::Allocate(unsigned int size)
{
	if (size > typeSize)
	{
		throw std::bad_alloc();
		return nullptr;
	}

	if (m_freeCount > 0)
		return m_freeList[--m_freeCount];
	else if (canGrow)
	{
		if (!m_next)
			m_next.reset(new NzMemoryPool(this));

		return m_next->Allocate(size);
	}

	throw std::bad_alloc();
	return nullptr;
}

template<unsigned int typeSize, unsigned int count, bool canGrow>
void NzMemoryPool<typeSize, count, canGrow>::Free(void* ptr)
{
	if (ptr)
	{
		// Le pointer nous appartient-il ?
		nzUInt8* freePtr = static_cast<nzUInt8*>(ptr);
		nzUInt8* poolPtr = m_pool.get();
		if (freePtr >= poolPtr && freePtr < poolPtr + typeSize*count)
		{
			#if NAZARA_CORE_SAFE
			if ((freePtr - poolPtr) % typeSize != 0)
			{
				throw std::runtime_error("Pointer does not belong to memory pool");
				return;
			}
			#endif

			m_freeList[m_freeCount++] = ptr;

			// Si nous sommes vide et l'extension d'un autre pool, on se suicide
			if (m_freeCount == count && m_previous && !m_next)
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
				throw std::runtime_error("Pointer does not belong to memory pool");
		}
	}
}

#include <Nazara/Core/DebugOff.hpp>
