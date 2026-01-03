// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_WRAPPER_COMMANDPOOL_HPP
#define NAZARA_VULKANRENDERER_WRAPPER_COMMANDPOOL_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/VulkanRenderer/Wrapper/DeviceObject.hpp>

namespace Nz::Vk
{
	class CommandBuffer;

	class NAZARA_VULKANRENDERER_API CommandPool : public DeviceObject<CommandPool, VkCommandPool, VkCommandPoolCreateInfo, VK_OBJECT_TYPE_COMMAND_POOL>
	{
		friend DeviceObject;

		public:
			CommandPool() = default;
			CommandPool(const CommandPool&) = delete;
			CommandPool(CommandPool&&) = default;
			~CommandPool() = default;

			CommandBuffer AllocateCommandBuffer(VkCommandBufferLevel level);
			std::vector<CommandBuffer> AllocateCommandBuffers(UInt32 commandBufferCount, VkCommandBufferLevel level);

			using DeviceObject::Create;
			inline bool Create(Device& device, UInt32 queueFamilyIndex, VkCommandPoolCreateFlags flags = 0, const VkAllocationCallbacks* allocator = nullptr);

			inline bool Reset(VkCommandPoolResetFlags flags = 0);

			CommandPool& operator=(const CommandPool&) = delete;
			CommandPool& operator=(CommandPool&&) = delete;

		private:
			static inline VkResult CreateHelper(Device& device, const VkCommandPoolCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkCommandPool* handle);
			static inline void DestroyHelper(Device& device, VkCommandPool handle, const VkAllocationCallbacks* allocator);
	};
}

#include <Nazara/VulkanRenderer/Wrapper/CommandPool.inl>

#endif // NAZARA_VULKANRENDERER_WRAPPER_COMMANDPOOL_HPP
