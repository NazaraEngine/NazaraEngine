// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VKQUEUE_HPP
#define NAZARA_VULKANRENDERER_VKQUEUE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/VulkanRenderer/VkDevice.hpp>
#include <vulkan/vulkan.h>

namespace Nz 
{
	namespace Vk
	{
		class Queue
		{
			public:
				inline Queue();
				inline Queue(const DeviceHandle& device, VkQueue queue);
				inline Queue(const Queue& queue);
				inline Queue(Queue&& queue);
				inline ~Queue() = default;

				inline const DeviceHandle& GetDevice() const;
				inline VkResult GetLastErrorCode() const;

				inline bool Present(const VkPresentInfoKHR& presentInfo) const;
				inline bool Present(VkSwapchainKHR swapchain, UInt32 imageIndex, VkSemaphore waitSemaphore = VK_NULL_HANDLE) const;

				inline bool Submit(const VkSubmitInfo& submit, VkFence fence = VK_NULL_HANDLE) const;
				inline bool Submit(UInt32 submitCount, const VkSubmitInfo* submits, VkFence fence = VK_NULL_HANDLE) const;

				inline bool WaitIdle() const;

				Queue& operator=(const Queue& queue) = delete;
				inline Queue& operator=(Queue&&);

				inline operator VkQueue();

			protected:
				DeviceHandle m_device;
				VkQueue m_handle;
				mutable VkResult m_lastErrorCode;
		};
	}
}

#include <Nazara/VulkanRenderer/VkQueue.inl>

#endif // NAZARA_VULKANRENDERER_VKQUEUE_HPP
