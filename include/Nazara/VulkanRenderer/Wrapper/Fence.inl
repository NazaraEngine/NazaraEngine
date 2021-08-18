// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/Wrapper/Fence.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	namespace Vk
	{
		inline bool Fence::Create(Device& device, VkFenceCreateFlags flags, const VkAllocationCallbacks* allocator)
		{
			VkFenceCreateInfo createInfo =
			{
				VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
				nullptr,
				flags
			};

			return Create(device, createInfo, allocator);
		}

		inline bool Fence::Reset()
		{
			m_lastErrorCode = m_device->vkResetFences(*m_device, 1U, &m_handle);
			if (m_lastErrorCode != VK_SUCCESS)
			{
				NazaraError("Failed to reset fence: " + TranslateVulkanError(m_lastErrorCode));
				return false;
			}

			return true;
		}

		inline bool Fence::Wait()
		{
			return Wait(std::numeric_limits<UInt64>::max());
		}

		inline bool Fence::Wait(UInt64 timeout, bool* didTimeout)
		{
			m_lastErrorCode = m_device->vkWaitForFences(*m_device, 1U, &m_handle, VK_TRUE, timeout);
			if (m_lastErrorCode != VK_SUCCESS && m_lastErrorCode != VK_TIMEOUT)
			{
				NazaraError("Failed to wait for fence: " + TranslateVulkanError(m_lastErrorCode));
				return false;
			}

			if (didTimeout)
				*didTimeout = (m_lastErrorCode == VK_TIMEOUT);

			return true;
		}

		inline VkResult Fence::CreateHelper(Device& device, const VkFenceCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkFence* handle)
		{
			return device.vkCreateFence(device, createInfo, allocator, handle);
		}

		inline void Fence::DestroyHelper(Device& device, VkFence handle, const VkAllocationCallbacks* allocator)
		{
			return device.vkDestroyFence(device, handle, allocator);
		}
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
