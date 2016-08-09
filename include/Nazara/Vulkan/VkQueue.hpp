// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKAN_VKQUEUE_HPP
#define NAZARA_VULKAN_VKQUEUE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Vulkan/VkDevice.hpp>
#include <vulkan/vulkan.h>

namespace Nz 
{
	namespace Vk
	{
		class Queue
		{
			public:
				inline Queue(Device& device, VkQueue queue);
				inline Queue(const Queue& queue);
				inline Queue(Queue&& queue);
				inline ~Queue() = default;

				inline Device& GetDevice();
				inline VkResult GetLastErrorCode() const;

				inline bool Present(const VkPresentInfoKHR& presentInfo);
				inline bool Present(VkSwapchainKHR swapchain, UInt32 imageIndex, VkSemaphore waitSemaphore = VK_NULL_HANDLE);

				inline bool Submit(const VkSubmitInfo& submit, VkFence fence = VK_NULL_HANDLE);
				inline bool Submit(UInt32 submitCount, const VkSubmitInfo* submits, VkFence fence = VK_NULL_HANDLE);

				inline bool WaitIdle();

				Queue& operator=(const Queue& queue) = delete;
				Queue& operator=(Queue&&) = delete;

				inline operator VkQueue();

			protected:
				Device& m_device;
				VkQueue m_handle;
				VkResult m_lastErrorCode;
		};
	}
}

#include <Nazara/Vulkan/VkQueue.inl>

#endif // NAZARA_VULKAN_VKQUEUE_HPP
