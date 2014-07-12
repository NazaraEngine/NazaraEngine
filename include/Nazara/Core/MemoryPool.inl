// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// Je ne suis pas fier des cinq lignes qui suivent mais difficile de faire autrement pour le moment...
#ifdef NAZARA_DEBUG_NEWREDEFINITION_DISABLE_REDEFINITION
	#define NAZARA_DEBUG_NEWREDEFINITION_DISABLE_REDEFINITION_DEFINED
#else
	#define NAZARA_DEBUG_NEWREDEFINITION_DISABLE_REDEFINITION
#endif

#include <new>
#include <Nazara/Core/Debug.hpp>

template<unsigned int blockSize, bool canGrow>
NzMemoryPool<blockSize, canGrow>::NzMemoryPool(unsigned int count) :
m_freeCount(count),
m_previous(nullptr),
m_size(count)
{
	m_pool.reset(new nzUInt8[blockSize * count]);
	m_freeList.reset(new void*[count]);

	// Remplissage de la free list
	for (unsigned int i = 0; i < count; ++i)
		m_freeList[i] = &m_pool[blockSize * (count-i-1)];
}

template<unsigned int blockSize, bool canGrow>
NzMemoryPool<blockSize, canGrow>::NzMemoryPool(NzMemoryPool* pool) :
NzMemoryPool(pool->m_size)
{
	m_previous = pool;
}

template<unsigned int blockSize, bool canGrow>
template<typename T>
T* NzMemoryPool<blockSize, canGrow>::Allocate()
{
	static_assert(sizeof(T) <= blockSize, "This type is too large for this memory pool");

	return static_cast<T*>(Allocate(sizeof(T)));
}

template<unsigned int blockSize, bool canGrow>
void* NzMemoryPool<blockSize, canGrow>::Allocate(unsigned int size)
{
	if (size <= blockSize)
	{
		if (m_freeCount > 0)
			return m_freeList[--m_freeCount];
		else if (canGrow)
		{
			if (!m_next)
				m_next.reset(new NzMemoryPool(this));

			return m_next->Allocate(size);
		}
	}

	return operator new(size);
}

template<unsigned int blockSize, bool canGrow>
void NzMemoryPool<blockSize, canGrow>::Free(void* ptr)
{
	if (ptr)
	{
		// Le pointer nous appartient-il ?
		nzUInt8* freePtr = static_cast<nzUInt8*>(ptr);
		nzUInt8* poolPtr = m_pool.get();
		if (freePtr >= poolPtr && freePtr < poolPtr + blockSize*m_size)
		{
			#if NAZARA_CORE_SAFE
			if ((freePtr - poolPtr) % blockSize != 0)
			{
				throw std::runtime_error("Pointer does not belong to memory pool");
				return;
			}
			#endif

			m_freeList[m_freeCount++] = ptr;

			// Si nous sommes vide et l'extension d'un autre pool, on se suicide
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
				operator delete(ptr);
		}
	}
}

template<unsigned int blockSize, bool canGrow>
unsigned int NzMemoryPool<blockSize, canGrow>::GetFreeBlocks() const
{
	return m_freeCount;
}

template<unsigned int blockSize, bool canGrow>
unsigned int NzMemoryPool<blockSize, canGrow>::GetSize() const
{
	return m_size;
}

#include <Nazara/Core/DebugOff.hpp>

// Si c'est nous qui avons défini la constante, alors il nous faut l'enlever (Pour éviter que le moteur entier n'en souffre)
#ifndef NAZARA_DEBUG_NEWREDEFINITION_DISABLE_REDEFINITION_DEFINED
	#undef NAZARA_DEBUG_NEWREDEFINITION_DISABLE_REDEFINITION
#endif
