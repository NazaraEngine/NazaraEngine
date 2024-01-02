// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanCommandPool.hpp>
#include <Nazara/VulkanRenderer/VulkanCommandBuffer.hpp>
#include <Nazara/VulkanRenderer/VulkanCommandBufferBuilder.hpp>
#include <Nazara/VulkanRenderer/Wrapper/CommandBuffer.hpp>
#include <NazaraUtils/StackVector.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	CommandBufferPtr VulkanCommandPool::BuildCommandBuffer(const std::function<void(CommandBufferBuilder& builder)>& callback)
	{
		Vk::AutoCommandBuffer commandBuffer = m_commandPool.AllocateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY);

		if (!commandBuffer->Begin(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT))
			throw std::runtime_error("failed to begin command buffer: " + TranslateVulkanError(commandBuffer->GetLastErrorCode()));

		VulkanCommandBufferBuilder builder(commandBuffer.Get());
		callback(builder);

		if (!commandBuffer->End())
			throw std::runtime_error("failed to build command buffer: " + TranslateVulkanError(commandBuffer->GetLastErrorCode()));

		for (std::size_t i = 0; i < m_commandPools.size(); ++i)
		{
			if (m_commandPools[i].freeCommands.TestNone())
				continue;

			return AllocateFromPool(i, std::move(commandBuffer));
		}

		// No allocation could be made, time to allocate a new pool
		std::size_t newPoolIndex = m_commandPools.size();
		AllocatePool();

		return AllocateFromPool(newPoolIndex, std::move(commandBuffer));
	}

	void VulkanCommandPool::UpdateDebugName(std::string_view name)
	{
		m_commandPool.SetDebugName(name);
	}

	auto VulkanCommandPool::AllocatePool() -> CommandPool&
	{
		constexpr UInt32 MaxSet = 128;

		CommandPool pool;
		pool.freeCommands.Resize(MaxSet, true);
		pool.storage = std::make_unique<CommandPool::BindingStorage[]>(MaxSet);

		return m_commandPools.emplace_back(std::move(pool));
	}

	void VulkanCommandPool::Release(CommandBuffer& binding)
	{
		VulkanCommandBuffer& vulkanBinding = static_cast<VulkanCommandBuffer&>(binding);

		std::size_t poolIndex = vulkanBinding.GetPoolIndex();
		std::size_t bindingIndex = vulkanBinding.GetBindingIndex();

		assert(poolIndex < m_commandPools.size());
		auto& pool = m_commandPools[poolIndex];
		assert(!pool.freeCommands.Test(bindingIndex));

		VulkanCommandBuffer* bindingMemory = reinterpret_cast<VulkanCommandBuffer*>(&pool.storage[bindingIndex]);
		PlacementDestroy(bindingMemory);

		pool.freeCommands.Set(bindingIndex);

		// Try to free pool if it's one of the last one
		if (poolIndex >= m_commandPools.size() - 1 && poolIndex <= m_commandPools.size())
			TryToShrink();
	}
}
