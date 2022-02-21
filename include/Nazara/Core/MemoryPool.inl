// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/MemoryPool.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/MemoryHelper.hpp>
#include <stdexcept>
#include <utility>
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
	*/
	template<typename T, std::size_t Alignment>
	MemoryPool<T, Alignment>::MemoryPool(std::size_t blockSize) :
	m_blockSize(blockSize)
	{
		// Allocate one block by default
		AllocateBlock();
	}

	/*!
	* \brief Destroy the memory pool, calling the destructor for every allocated object and desallocating blocks
	*/
	template<typename T, std::size_t Alignment>
	MemoryPool<T, Alignment>::~MemoryPool()
	{
		Reset();
	}

	/*!
	* \brief Allocates enough memory for the size and returns a pointer to it
	* \return A pointer to memory allocated
	*
	* \param index Output entry index (which can be used for deallocation)
	*
	* \remark If the size is greater than the blockSize of pool, new operator is called
	*/
	template<typename T, std::size_t Alignment>
	template<typename... Args>
	T* MemoryPool<T, Alignment>::Allocate(std::size_t& index, Args&&... args)
	{
		std::size_t blockIndex = 0;
		std::size_t localIndex = InvalidIndex;
		for (; blockIndex < m_blocks.size(); ++blockIndex)
		{
			auto& block = m_blocks[blockIndex];
			if (block.occupiedEntryCount == m_blockSize)
				continue;

			localIndex = block.freeEntries.FindFirst();
			assert(localIndex != block.freeEntries.npos);
			break;
		}

		if (blockIndex >= m_blocks.size())
		{
			// No more room, allocate a new block
			blockIndex = m_blocks.size();
			localIndex = 0;

			AllocateBlock();
		}

		assert(localIndex != InvalidIndex);

		auto& block = m_blocks[blockIndex];
		block.freeEntries.Reset(localIndex);
		block.occupiedEntries.Set(localIndex);
		block.occupiedEntryCount++;

		T* entry = reinterpret_cast<T*>(&block.memory[localIndex]);
		PlacementNew(entry, std::forward<Args>(args)...);

		index = blockIndex * m_blockSize + localIndex;

		return entry;
	}

	/*!
	* \brief Clears the memory pool
	*
	* This is call the destructor of every active entry and invalidate every entry index, and will free every allocated block
	*
	* \see Reset
	*/
	template<typename T, std::size_t Alignment>
	void MemoryPool<T, Alignment>::Clear()
	{
		Reset();

		m_blocks.clear();
	}

	/*!
	* \brief Returns an object memory to the memory pool
	*
	* Calls the destructor of the target object and returns its memory to the pool
	*
	* \param index Index of the allocated object
	*
	* \see Reset
	*/
	template<typename T, std::size_t Alignment>
	void MemoryPool<T, Alignment>::Free(std::size_t index)
	{
		std::size_t blockIndex = index / m_blockSize;
		std::size_t localIndex = index % m_blockSize;

		T* entry = GetAllocatedPointer(blockIndex, localIndex);
		PlacementDestroy(entry);

		auto& block = m_blocks[blockIndex];
		assert(block.occupiedEntryCount > 0);
		block.occupiedEntryCount--;

		block.freeEntries.Set(localIndex);
		block.occupiedEntries.Reset(localIndex);
	}

	/*!
	* \brief Returns the number of allocated entries
	* \return How many entries are currently allocated
	*/
	template<typename T, std::size_t Alignment>
	std::size_t MemoryPool<T, Alignment>::GetAllocatedEntryCount() const
	{
		std::size_t count = 0;
		for (auto& block : m_blocks)
			count += block.occupiedEntryCount;

		return count;
	}

	/*!
	* \brief Gets the block count
	* \return How many block are currently allocated for this memory pool
	*/
	template<typename T, std::size_t Alignment>
	std::size_t MemoryPool<T, Alignment>::GetBlockCount() const
	{
		return m_blocks.size();
	}

	/*!
	* \brief Gets the block size
	* \return Size of each block (i.e. how many items can fit in a block)
	*/
	template<typename T, std::size_t Alignment>
	std::size_t MemoryPool<T, Alignment>::GetBlockSize() const
	{
		return m_blockSize;
	}

	/*!
	* \brief Returns the number of free entries
	* \return How many entries are currently freed
	*/
	template<typename T, std::size_t Alignment>
	std::size_t MemoryPool<T, Alignment>::GetFreeEntryCount() const
	{
		std::size_t count = m_blocks.size() * m_blockSize;
		return count - GetAllocatedEntryCount();
	}

	/*!
	* \brief Resets the memory pool
	*
	* This is call the destructor of every active entry and invalidate every entry index, returning the pool to full capacity
	* Note that memory is not freed
	*
	* \see Clear
	*/
	template<typename T, std::size_t Alignment>
	void MemoryPool<T, Alignment>::Reset()
	{
		for (std::size_t blockIndex = 0; blockIndex < m_blocks.size(); ++blockIndex)
		{
			auto& block = m_blocks[blockIndex];
			if (block.occupiedEntryCount == 0)
				continue;

			for (std::size_t localIndex = block.occupiedEntries.FindFirst(); localIndex != block.occupiedEntries.npos; localIndex = block.occupiedEntries.FindNext(localIndex))
			{
				T* entry = reinterpret_cast<T*>(&m_blocks[blockIndex].memory[localIndex]);
				PlacementDestroy(entry);
			}

			block.freeEntries.Reset(true);
			block.occupiedEntries.Reset(false);
			block.occupiedEntryCount = 0;
		}
	}

	/*!
	* \brief Retrieve an allocated pointer based on a valid entry index
	*
	* \param index Entry index
	*
	* \return Pointer to the allocated entry
	*
	* \remark index must be valid
	*/
	template<typename T, std::size_t Alignment>
	T* MemoryPool<T, Alignment>::RetrieveFromIndex(std::size_t index)
	{
		std::size_t blockIndex = index / m_blockSize;
		std::size_t localIndex = index % m_blockSize;

		return GetAllocatedPointer(blockIndex, localIndex);
	}

	/*!
	* \brief Retrieve an entry index based on an allocated pointer
	*
	* \param data Allocated entry pointed
	* 
	* \return Corresponding index, or InvalidIndex if it's not part of this pool
	*/
	template<typename T, std::size_t Alignment>
	std::size_t MemoryPool<T, Alignment>::RetrieveEntryIndex(const T* data)
	{
		std::size_t blockIndex = 0;
		std::size_t localIndex = InvalidIndex;
		for (; blockIndex < m_blocks.size(); ++blockIndex)
		{
			auto& block = m_blocks[blockIndex];
			const T* startPtr = reinterpret_cast<const T*>(&block.memory[0]);
			if (data >= startPtr && data < startPtr + m_blockSize)
			{
				// Part of block
				localIndex = SafeCast<std::size_t>(data - startPtr);
				assert(data == reinterpret_cast<const T*>(&block.memory[localIndex]));

				break;
			}
		}

		if (blockIndex == m_blocks.size())
			return InvalidIndex;

		assert(localIndex != InvalidIndex);

		return blockIndex * m_blockSize + localIndex;
	}

	template<typename T, std::size_t Alignment>
	auto MemoryPool<T, Alignment>::begin() -> iterator
	{
		auto [blockIndex, localIndex] = GetFirstAllocatedEntry();
		return iterator(this, blockIndex, localIndex);
	}

	template<typename T, std::size_t Alignment>
	auto MemoryPool<T, Alignment>::end() -> iterator
	{
		return iterator(this, InvalidIndex, InvalidIndex);
	}

	template<typename T, std::size_t Alignment>
	std::size_t MemoryPool<T, Alignment>::size()
	{
		return GetAllocatedEntryCount();
	}

	template<typename T, std::size_t Alignment>
	void MemoryPool<T, Alignment>::AllocateBlock()
	{
		auto& block = m_blocks.emplace_back();
		block.freeEntries.Resize(m_blockSize, true);
		block.occupiedEntries.Resize(m_blockSize, false);
		block.memory = std::make_unique<AlignedStorage[]>(m_blockSize);
	}

	template<typename T, std::size_t Alignment>
	T* MemoryPool<T, Alignment>::GetAllocatedPointer(std::size_t blockIndex, std::size_t localIndex)
	{
		assert(blockIndex < m_blocks.size());
		auto& block = m_blocks[blockIndex];
		assert(block.occupiedEntries.Test(localIndex));

		return reinterpret_cast<T*>(&block.memory[localIndex]);
	}

	template<typename T, std::size_t Alignment>
	std::pair<std::size_t, std::size_t> MemoryPool<T, Alignment>::GetFirstAllocatedEntry() const
	{
		return GetFirstAllocatedEntryFromBlock(0);
	}

	template<typename T, std::size_t Alignment>
	std::pair<std::size_t, std::size_t> MemoryPool<T, Alignment>::GetFirstAllocatedEntryFromBlock(std::size_t blockIndex) const
	{
		// Search in next block
		std::size_t localIndex = InvalidIndex;
		for (; blockIndex < m_blocks.size(); ++blockIndex)
		{
			auto& block = m_blocks[blockIndex];
			if (block.occupiedEntryCount == 0)
				continue;

			localIndex = block.occupiedEntries.FindFirst();
			assert(localIndex != block.occupiedEntries.npos);
			break;
		}

		if (blockIndex >= m_blocks.size())
			return { InvalidIndex, InvalidIndex };

		return { blockIndex, localIndex };
	}

	template<typename T, std::size_t Alignment>
	std::pair<std::size_t, std::size_t> MemoryPool<T, Alignment>::GetNextAllocatedEntry(std::size_t blockIndex, std::size_t localIndex) const
	{
		assert(blockIndex < m_blocks.size());
		auto& block = m_blocks[blockIndex];
		std::size_t nextLocalIndex = block.occupiedEntries.FindNext(localIndex);
		if (nextLocalIndex != block.occupiedEntries.npos)
			return { blockIndex, nextLocalIndex };

		// Search in next block
		return GetFirstAllocatedEntryFromBlock(blockIndex + 1);
	}


	template<typename T, std::size_t Alignment>
	MemoryPool<T, Alignment>::iterator::iterator(MemoryPool* owner, std::size_t blockIndex, std::size_t localIndex) :
	m_blockIndex(blockIndex),
	m_localIndex(localIndex),
	m_owner(owner)
	{
	}

	template<typename T, std::size_t Alignment>
	auto MemoryPool<T, Alignment>::iterator::operator++(int) -> iterator
	{
		iterator copy(*this);
		operator++();
		return copy;
	}

	template<typename T, std::size_t Alignment>
	auto MemoryPool<T, Alignment>::iterator::operator++() -> iterator&
	{
		auto [blockIndex, localIndex] = m_owner->GetNextAllocatedEntry(m_blockIndex, m_localIndex);
		m_blockIndex = blockIndex;
		m_localIndex = localIndex;

		return *this;
	}

	template<typename T, std::size_t Alignment>
	bool MemoryPool<T, Alignment>::iterator::operator==(const iterator& rhs) const
	{
		assert(m_owner == rhs.m_owner);
		return m_blockIndex == rhs.m_blockIndex && m_localIndex == rhs.m_localIndex;
	}

	template<typename T, std::size_t Alignment>
	bool MemoryPool<T, Alignment>::iterator::operator!=(const iterator& rhs) const
	{
		return !operator==(rhs);
	}

	template<typename T, std::size_t Alignment>
	auto MemoryPool<T, Alignment>::iterator::operator*() const -> reference
	{
		return *m_owner->GetAllocatedPointer(m_blockIndex, m_localIndex);
	}
}

#include <Nazara/Core/DebugOff.hpp>
