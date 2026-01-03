// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/OpenGLRenderer/OpenGLUploadPool.hpp>
#include <cassert>
#include <stdexcept>

namespace Nz
{
	auto OpenGLUploadPool::Allocate(UInt64 size) -> Allocation&
	{
		return Allocate(size, 1); //< Alignment doesn't matter
	}

	auto OpenGLUploadPool::Allocate(UInt64 size, UInt64 /*alignment*/) -> Allocation&
	{
		// Try to minimize lost space
		struct
		{
			Block* block = nullptr;
			UInt64 offset = 0;
		} bestBlock;

		for (Block& block : m_blocks)
		{
			if (block.freeOffset + size > block.size)
				continue; //< Not enough space

			if (!bestBlock.block)
			{
				bestBlock.block = &block;
				bestBlock.offset = block.freeOffset;
				break; //< Since we have no alignment constraint, the first block is good
			}
		}

		// No block found, allocate a new one
		if (!bestBlock.block)
		{
			// Handle really big allocations (TODO: Handle them separately as they shouldn't be common and can consume a lot of memory)
			UInt64 blockSize = std::max(m_blockSize, size);

			Block newBlock;
			newBlock.size = blockSize;

			newBlock.memory.resize(blockSize);

			bestBlock.block = &m_blocks.emplace_back(std::move(newBlock));
			bestBlock.offset = 0;
		}

		// Now find the proper allocation buffer
		std::size_t allocationBlockIndex = m_nextAllocationIndex / AllocationPerBlock;
		std::size_t allocationIndex = m_nextAllocationIndex % AllocationPerBlock;

		if (allocationBlockIndex >= m_allocationBlocks.size())
		{
			assert(allocationBlockIndex == m_allocationBlocks.size());
			m_allocationBlocks.emplace_back(std::make_unique<AllocationBlock>());
		}

		auto& allocationBlock = *m_allocationBlocks[allocationBlockIndex];

		Allocation& allocationData = allocationBlock[allocationIndex];
		allocationData.mappedPtr = static_cast<UInt8*>(bestBlock.block->memory.data()) + bestBlock.offset;
		allocationData.size = size;

		bestBlock.block->freeOffset += size;
		m_nextAllocationIndex++;

		return allocationData;
	}

	void OpenGLUploadPool::Reset()
	{
		for (Block& block : m_blocks)
			block.freeOffset = 0;

		m_nextAllocationIndex = 0;
	}
}
