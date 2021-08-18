// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/Wrapper/CommandPool.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Device.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	namespace Vk
	{
		inline bool CommandPool::Create(Device& device, UInt32 queueFamilyIndex, VkCommandPoolCreateFlags flags, const VkAllocationCallbacks* allocator)
		{
			VkCommandPoolCreateInfo createInfo =
			{
				VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
				nullptr,
				flags,
				queueFamilyIndex
			};

			return Create(device, createInfo, allocator);
		}

		inline bool CommandPool::Reset(VkCommandPoolResetFlags flags)
		{
			m_lastErrorCode = m_device->vkResetCommandPool(*m_device, m_handle, flags);
			if (m_lastErrorCode != VkResult::VK_SUCCESS)
				return false;

			return true;
		}

		inline VkResult CommandPool::CreateHelper(Device& device, const VkCommandPoolCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkCommandPool* handle)
		{
			return device.vkCreateCommandPool(device, createInfo, allocator, handle);
		}

		inline void CommandPool::DestroyHelper(Device& device, VkCommandPool handle, const VkAllocationCallbacks* allocator)
		{
			return device.vkDestroyCommandPool(device, handle, allocator);
		}
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
