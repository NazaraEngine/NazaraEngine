// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/VulkanRenderer/VulkanUploadPool.hpp>
#include <cassert>
#include <stdexcept>

namespace Nz
{
	auto VulkanUploadPool::Allocate(UInt64 size) -> VulkanAllocation&
	{
		const auto& deviceProperties = m_device.GetPhysicalDeviceInfo().properties;
		UInt64 preferredAlignement = deviceProperties.limits.optimalBufferCopyOffsetAlignment;

		return Allocate(size, preferredAlignement);
	}

	auto VulkanUploadPool::Allocate(UInt64 size, UInt64 alignment) -> VulkanAllocation&
	{
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
			if (alignedOffset + size > block.size)
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
			// Handle really big allocations (TODO: Handle them separately as they shouldn't be common and can consume a lot of memory)
			UInt64 blockSize = std::max(m_blockSize, size);

			Block newBlock;
			newBlock.size = blockSize;

			if (!newBlock.buffer.Create(m_device, 0U, blockSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT))
				throw std::runtime_error("failed to create block buffer: " + TranslateVulkanError(newBlock.buffer.GetLastErrorCode()));

			VkMemoryRequirements requirement = newBlock.buffer.GetMemoryRequirements();

			if (!newBlock.blockMemory.Create(m_device, requirement.size, requirement.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
				throw std::runtime_error("failed to allocate block memory: " + TranslateVulkanError(newBlock.blockMemory.GetLastErrorCode()));

			if (!newBlock.buffer.BindBufferMemory(newBlock.blockMemory))
				throw std::runtime_error("failed to bind buffer memory: " + TranslateVulkanError(newBlock.buffer.GetLastErrorCode()));

			if (!newBlock.blockMemory.Map())
				throw std::runtime_error("failed to map buffer memory: " + TranslateVulkanError(newBlock.buffer.GetLastErrorCode()));

			bestBlock.block = &m_blocks.emplace_back(std::move(newBlock));
			bestBlock.alignedOffset = 0;
			bestBlock.lostSpace = 0;
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

		VulkanAllocation& allocationData = allocationBlock[allocationIndex];
		allocationData.buffer = bestBlock.block->buffer;
		allocationData.mappedPtr = static_cast<UInt8*>(bestBlock.block->blockMemory.GetMappedPointer()) + bestBlock.alignedOffset;
		allocationData.offset = bestBlock.alignedOffset;
		allocationData.size = size;

		bestBlock.block->freeOffset += size;
		m_nextAllocationIndex++;

		return allocationData;
	}

	void VulkanUploadPool::Reset()
	{
		for (Block& block : m_blocks)
			block.freeOffset = 0;

		m_nextAllocationIndex = 0;
	}
}
