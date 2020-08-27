// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLUploadPool.hpp>
#include <cassert>
#include <stdexcept>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	auto OpenGLUploadPool::Allocate(UInt64 size) -> Allocation&
	{
		return Allocate(size, 1); //< Alignment doesn't matter
	}

	auto OpenGLUploadPool::Allocate(UInt64 size, UInt64 /*alignment*/) -> Allocation&
	{
		assert(size <= m_blockSize);

		// Try to minimize lost space
		struct
		{
			Block* block = nullptr;
			UInt64 offset = 0;
		} bestBlock;

		for (Block& block : m_blocks)
		{
			UInt64 alignedOffset = block.freeOffset;
			if (block.freeOffset + size > m_blockSize)
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
			Block newBlock;
			newBlock.memory.resize(m_blockSize);

			bestBlock.block = &m_blocks.emplace_back(std::move(newBlock));
			bestBlock.offset = 0;
		}

		Allocation& allocationData = m_allocations.emplace_back();
		allocationData.mappedPtr = static_cast<UInt8*>(bestBlock.block->memory.data()) + bestBlock.offset;
		allocationData.size = size;

		return allocationData;
	}

	void OpenGLUploadPool::Reset()
	{
		for (Block& block : m_blocks)
			block.freeOffset = 0;

		m_allocations.clear();
	}
}
