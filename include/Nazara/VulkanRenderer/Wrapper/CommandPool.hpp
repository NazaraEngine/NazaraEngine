// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VKCOMMANDPOOL_HPP
#define NAZARA_VULKANRENDERER_VKCOMMANDPOOL_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/HandledObject.hpp>
#include <Nazara/VulkanRenderer/Wrapper/DeviceObject.hpp>

namespace Nz 
{
	namespace Vk
	{
		class CommandBuffer;
		class CommandPool;

		using CommandPoolHandle = ObjectHandle<CommandPool>;

		class NAZARA_VULKANRENDERER_API CommandPool : public DeviceObject<CommandPool, VkCommandPool, VkCommandPoolCreateInfo>, public HandledObject<CommandPool>
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
				inline bool Create(const DeviceHandle& device, UInt32 queueFamilyIndex, VkCommandPoolCreateFlags flags = 0, const VkAllocationCallbacks* allocator = nullptr);

				inline bool Reset(VkCommandPoolResetFlags flags);

				CommandPool& operator=(const CommandPool&) = delete;
				CommandPool& operator=(CommandPool&&) = delete;

			private:
				static inline VkResult CreateHelper(const DeviceHandle& device, const VkCommandPoolCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkCommandPool* handle);
				static inline void DestroyHelper(const DeviceHandle& device, VkCommandPool handle, const VkAllocationCallbacks* allocator);
		};
	}
}

#include <Nazara/VulkanRenderer/Wrapper/CommandPool.inl>

#endif // NAZARA_VULKANRENDERER_VKCOMMANDPOOL_HPP
