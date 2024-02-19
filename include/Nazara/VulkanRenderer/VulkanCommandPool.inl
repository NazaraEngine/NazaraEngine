// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <stdexcept>

namespace Nz
{
	inline VulkanCommandPool::VulkanCommandPool(Vk::Device& device, QueueType queueType)
	{
		UInt32 queueFamilyIndex = device.GetDefaultFamilyIndex(queueType);
		if (queueFamilyIndex == Vk::Device::InvalidQueue)
			throw std::runtime_error("QueueType " + std::to_string(UnderlyingCast(queueType)) + " is not supported");

		if (!m_commandPool.Create(device, queueFamilyIndex))
			throw std::runtime_error("Failed to create command pool: " + TranslateVulkanError(m_commandPool.GetLastErrorCode()));
	}

	inline VulkanCommandPool::VulkanCommandPool(Vk::Device& device, UInt32 queueFamilyIndex)
	{
		if (!m_commandPool.Create(device, queueFamilyIndex))
			throw std::runtime_error("Failed to create command pool: " + TranslateVulkanError(m_commandPool.GetLastErrorCode()));
	}

	template<typename... Args>
	CommandBufferPtr VulkanCommandPool::AllocateFromPool(std::size_t poolIndex, Args&&... args)
	{
		auto& pool = m_commandPools[poolIndex];

		std::size_t freeBindingId = pool.freeCommands.FindFirst();
		if (freeBindingId == pool.freeCommands.npos)
			return {}; //< No free binding in this pool

		pool.freeCommands.Reset(freeBindingId);

		VulkanCommandBuffer* freeBindingMemory = reinterpret_cast<VulkanCommandBuffer*>(&pool.storage[freeBindingId]);
		return CommandBufferPtr(PlacementNew(freeBindingMemory, *this, poolIndex, freeBindingId, std::forward<Args>(args)...));
	}

	inline void VulkanCommandPool::TryToShrink()
	{
		std::size_t poolCount = m_commandPools.size();
		if (poolCount >= 2 && m_commandPools.back().freeCommands.TestAll())
		{
			for (std::size_t i = poolCount - 1; i > 0; --i)
			{
				if (!m_commandPools[i].freeCommands.TestAll())
					break;

				poolCount--;
			}

			m_commandPools.resize(poolCount);
		}
	}
}

