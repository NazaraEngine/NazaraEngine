// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_WRAPPER_QUEUEHANDLE_HPP
#define NAZARA_VULKANRENDERER_WRAPPER_QUEUEHANDLE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Device.hpp>
#include <NazaraUtils/MovablePtr.hpp>
#include <vulkan/vulkan_core.h>

namespace Nz
{
	namespace Vk
	{
		class QueueHandle
		{
			public:
				inline QueueHandle();
				inline QueueHandle(Device& device, VkQueue queue, UInt32 queueFamilyIndex);
				QueueHandle(const QueueHandle& queue) = delete;
				QueueHandle(QueueHandle&& queue) noexcept = default;
				~QueueHandle() = default;

				inline Device& GetDevice() const;
				inline VkResult GetLastErrorCode() const;
				inline UInt32 GetQueueFamilyIndex() const;

				inline bool Present(const VkPresentInfoKHR& presentInfo) const;
				inline bool Present(VkSwapchainKHR swapchain, UInt32 imageIndex, VkSemaphore waitSemaphore = VK_NULL_HANDLE) const;

				inline bool Submit(VkCommandBuffer commandBuffer, VkFence signalFence = VK_NULL_HANDLE) const;
				inline bool Submit(VkCommandBuffer commandBuffer, VkSemaphore waitSemaphore, VkPipelineStageFlags waitStage, VkSemaphore signalSemaphore, VkFence signalFence = VK_NULL_HANDLE) const;
				inline bool Submit(VkCommandBuffer commandBuffer, std::initializer_list<VkSemaphore> waitSemaphores, std::initializer_list<VkPipelineStageFlags> waitStages, std::initializer_list<VkSemaphore> signalSemaphores, VkFence signalFence = VK_NULL_HANDLE) const;
				inline bool Submit(UInt32 commandBufferCount, const VkCommandBuffer* commandBuffers, std::initializer_list<VkSemaphore> waitSemaphores, std::initializer_list<VkPipelineStageFlags> waitStages, std::initializer_list<VkSemaphore> signalSemaphores, VkFence signalFence = VK_NULL_HANDLE) const;
				inline bool Submit(UInt32 commandBufferCount, const VkCommandBuffer* commandBuffers, VkSemaphore waitSemaphore, VkPipelineStageFlags waitStage, VkSemaphore signalSemaphore, VkFence signalFence = VK_NULL_HANDLE) const;
				inline bool Submit(UInt32 commandBufferCount, const VkCommandBuffer* commandBuffers, UInt32 waitSemaphoreCount, const VkSemaphore* waitSemaphores, const VkPipelineStageFlags* waitStage, UInt32 signalSemaphoreCount, const VkSemaphore* signalSemaphores, VkFence signalFence = VK_NULL_HANDLE) const;
				inline bool Submit(const VkSubmitInfo& submit, VkFence signalFence = VK_NULL_HANDLE) const;
				inline bool Submit(UInt32 submitCount, const VkSubmitInfo* submits, VkFence signalFence = VK_NULL_HANDLE) const;

				inline bool WaitIdle() const;

				QueueHandle& operator=(const QueueHandle& queue) = delete;
				QueueHandle& operator=(QueueHandle&&) noexcept = default;

				inline operator VkQueue();

			protected:
				MovablePtr<Device> m_device;
				VkQueue m_handle;
				mutable VkResult m_lastErrorCode;
				UInt32 m_queueFamilyIndex;
		};
	}
}

#include <Nazara/VulkanRenderer/Wrapper/QueueHandle.inl>

#endif // NAZARA_VULKANRENDERER_WRAPPER_QUEUEHANDLE_HPP
