// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VULKANCOMMANDPOOL_HPP
#define NAZARA_VULKANRENDERER_VULKANCOMMANDPOOL_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Renderer/CommandPool.hpp>
#include <Nazara/VulkanRenderer/Export.hpp>
#include <Nazara/VulkanRenderer/VulkanCommandBuffer.hpp>
#include <Nazara/VulkanRenderer/Wrapper/CommandPool.hpp>
#include <NazaraUtils/Bitset.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_VULKANRENDERER_API VulkanCommandPool final : public CommandPool
	{
		friend VulkanCommandBuffer;

		public:
			inline VulkanCommandPool(Vk::Device& device, QueueType queueType);
			inline VulkanCommandPool(Vk::Device& device, UInt32 queueFamilyIndex);
			VulkanCommandPool(const VulkanCommandPool&) = delete;
			VulkanCommandPool(VulkanCommandPool&&) noexcept = default;
			~VulkanCommandPool() = default;

			CommandBufferPtr BuildCommandBuffer(const FunctionRef<void(CommandBufferBuilder& builder)>& callback) override;

			void UpdateDebugName(std::string_view name) override;

			VulkanCommandPool& operator=(const VulkanCommandPool&) = delete;
			VulkanCommandPool& operator=(VulkanCommandPool&&) = delete;

		private:
			struct CommandPool;

			CommandPool& AllocatePool();
			template<typename... Args> CommandBufferPtr AllocateFromPool(std::size_t poolIndex, Args&&... args);
			void Release(CommandBuffer& commandBuffer);
			inline void TryToShrink();

			struct CommandPool
			{
				using BindingStorage = std::aligned_storage_t<sizeof(VulkanCommandBuffer), alignof(VulkanCommandBuffer)>;

				Bitset<UInt64> freeCommands;
				std::unique_ptr<BindingStorage[]> storage;
			};

			MovablePtr<Vk::Device> m_device;
			std::vector<CommandPool> m_commandPools;
			Vk::CommandPool m_commandPool;
	};
}

#include <Nazara/VulkanRenderer/VulkanCommandPool.inl>

#endif // NAZARA_VULKANRENDERER_VULKANCOMMANDPOOL_HPP
