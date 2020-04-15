// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#if 0

#include <Nazara/OpenGLRenderer/OpenGLUploadPool.hpp>
#include <cassert>
#include <stdexcept>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	auto OpenGLUploadPool::Allocate(UInt64 size) -> OpenGLAllocation&
	{
		const auto& deviceProperties = m_device.GetPhysicalDeviceInfo().properties;
		UInt64 preferredAlignement = deviceProperties.limits.optimalBufferCopyOffsetAlignment;

		return Allocate(size, preferredAlignement);
	}

	auto OpenGLUploadPool::Allocate(UInt64 size, UInt64 alignment) -> OpenGLAllocation&
	{
		assert(size <= m_blockSize);

		// Try to minimize lost space
		struct
		{
			Block* block = nullptr;
			UInt64 alignedOffset = 0;
			UInt64 lostSpace = 0;
		} bestBlock;

		for (Block& block : m_blocks)
		{
			UInt64 alignedOffset = AlignPow2(block.freeOffset, alignment);
			if (alignedOffset + size > m_blockSize)
				continue; //< Not enough space

			UInt64 lostSpace = alignedOffset - block.freeOffset;

			if (!bestBlock.block || lostSpace < bestBlock.lostSpace)
			{
				bestBlock.block = &block;
				bestBlock.alignedOffset = alignedOffset;
				bestBlock.lostSpace = lostSpace;
			}
		}

		// No block found, allocate a new one
		if (!bestBlock.block)
		{
			Block newBlock;
			if (!newBlock.buffer.Create(m_device, 0U, m_blockSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT))
				throw std::runtime_error("Failed to create block buffer: " + TranslateOpenGLError(newBlock.buffer.GetLastErrorCode()));

			VkMemoryRequirements requirement = newBlock.buffer.GetMemoryRequirements();

			if (!newBlock.blockMemory.Create(m_device, requirement.size, requirement.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
				throw std::runtime_error("Failed to allocate block memory: " + TranslateOpenGLError(newBlock.blockMemory.GetLastErrorCode()));

			if (!newBlock.buffer.BindBufferMemory(newBlock.blockMemory))
				throw std::runtime_error("Failed to bind buffer memory: " + TranslateOpenGLError(newBlock.buffer.GetLastErrorCode()));

			if (!newBlock.blockMemory.Map())
				throw std::runtime_error("Failed to map buffer memory: " + TranslateOpenGLError(newBlock.buffer.GetLastErrorCode()));

			bestBlock.block = &m_blocks.emplace_back(std::move(newBlock));
			bestBlock.alignedOffset = 0;
			bestBlock.lostSpace = 0;
		}

		OpenGLAllocation& allocationData = m_allocations.emplace_back();
		allocationData.buffer = bestBlock.block->buffer;
		allocationData.mappedPtr = static_cast<UInt8*>(bestBlock.block->blockMemory.GetMappedPointer()) + bestBlock.alignedOffset;
		allocationData.offset = bestBlock.alignedOffset;
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

#endif
