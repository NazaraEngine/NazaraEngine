// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/MemoryHelper.hpp>
#include <utility>
#include <stdexcept>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::MemoryPool
	* \brief Core class that represents a memory pool
	*/

	/*!
	* \brief Constructs a MemoryPool object
	*
	* \param blockSize Size of blocks that will be allocated
	* \param size Size of the pool
	* \param canGrow Determine if the pool can allocate more memory
	*/

	inline MemoryPool::MemoryPool(unsigned int blockSize, unsigned int size, bool canGrow) :
	m_freeCount(size),
	m_previous(nullptr),
	m_canGrow(canGrow),
	m_blockSize(blockSize),
	m_size(size)
	{
		m_pool.reset(new UInt8[blockSize * size]);
		m_freeList.reset(new void* [size]);

		// Remplissage de la free list
		for (unsigned int i = 0; i < size; ++i)
			m_freeList[i] = &m_pool[m_blockSize * (size-i-1)];
	}

	/*!
	* \brief Constructs a MemoryPool object by move semantic
	*
	* \param pool MemoryPool to move into this
	*/

	inline MemoryPool::MemoryPool(MemoryPool&& pool) noexcept
	{
		operator=(std::move(pool));
	}

	/*!
	* \brief Constructs a MemoryPool object by chaining memory pool
	*
	* \param pool Previous MemoryPool
	*/

	inline MemoryPool::MemoryPool(MemoryPool* pool) :
	MemoryPool(pool->m_blockSize, pool->m_size, pool->m_canGrow)
	{
		m_previous = pool;
	}

	/*!
	* \brief Allocates enough memory for the size and returns a pointer to it
	* \return A pointer to memory allocated
	*
	* \param size Size to allocate
	*
	* \remark If the size is greather than the blockSize of pool, new operator is called
	*/

	inline void* MemoryPool::Allocate(unsigned int size)
	{
		if (size <= m_blockSize)
		{
			if (m_freeCount > 0)
				return m_freeList[--m_freeCount];
			else if (m_canGrow)
			{
				if (!m_next)
					m_next.reset(new MemoryPool(this));

				return m_next->Allocate(size);
			}
		}

		return OperatorNew(size);
	}

	/*!
	* \brief Deletes the memory represented by the poiner
	*
	* Calls the destructor of the object before releasing it
	*
	* \remark If ptr is null, nothing is done
	*/

	template<typename T>
	inline void MemoryPool::Delete(T* ptr)
	{
		if (ptr)
		{
			ptr->~T();
			Free(ptr);
		}
	}

	/*!
	* \brief Frees the memory represented by the poiner
	*
	* If the pool gets empty after the call and we are the child of another pool, we commit suicide. If the pointer does not own to a block of the pool, operator delete is called
	*
	* \remark Throws a std::runtime_error if pointer does not point to an element of the pool with NAZARA_CORE_SAFE defined
	* \remark If ptr is null, nothing is done
	*/

	inline void MemoryPool::Free(void* ptr)
	{
		if (ptr)
		{
			// Does the pointer belong to us ?
			UInt8* freePtr = static_cast<UInt8*>(ptr);
			UInt8* poolPtr = m_pool.get();
			if (freePtr >= poolPtr && freePtr < poolPtr + m_blockSize*m_size)
			{
				#if NAZARA_CORE_SAFE
				if ((freePtr - poolPtr) % m_blockSize != 0)
					throw std::runtime_error("Invalid pointer (does not point to an element of the pool)");
				#endif

				m_freeList[m_freeCount++] = ptr;

				// If we are empty and the extension of another pool, we commit suicide
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
					OperatorDelete(ptr);
			}
		}
	}

	/*!
	* \brief Gets the block size
	* \return Size of the blocks
	*/

	inline unsigned int MemoryPool::GetBlockSize() const
	{
		return m_blockSize;
	}

	/*!
	* \brief Gets the number of free blocks
	* \return Number of free blocks in the pool
	*/

	inline unsigned int MemoryPool::GetFreeBlocks() const
	{
		return m_freeCount;
	}

	/*!
	* \brief Gets the pool size
	* \return Size of the pool
	*/

	inline unsigned int MemoryPool::GetSize() const
	{
		return m_size;
	}

	/*!
	* \brief Creates a new value of type T with arguments
	* \return Pointer to the allocated object
	*
	* \param args Arguments for the new object
	*
	* \remark Constructs inplace in the pool
	*/

	template<typename T, typename... Args>
	inline T* MemoryPool::New(Args&&... args)
	{
		T* object = static_cast<T*>(Allocate(sizeof(T)));
		PlacementNew(object, std::forward<Args>(args)...);

		return object;
	}

	/*!
	* \brief Assigns the content of another pool by move semantic
	* \return A reference to this
	*
	* \param pool Other pool to move into this
	*/

	inline MemoryPool& MemoryPool::operator=(MemoryPool&& pool) noexcept
	{
		m_blockSize = pool.m_blockSize;
		m_canGrow = pool.m_canGrow;
		m_freeCount = pool.m_freeCount.load(std::memory_order_relaxed);
		m_freeList = std::move(pool.m_freeList);
		m_pool = std::move(pool.m_pool);
		m_previous = pool.m_previous;
		m_next = std::move(pool.m_next);
		m_size = pool.m_size;

		// If we have been created by another pool, we must make it point to us again
		if (m_previous)
		{
			m_previous->m_next.release();
			m_previous->m_next.reset(this);
		}

		return *this;
	}
}

#include <Nazara/Core/DebugOff.hpp>
