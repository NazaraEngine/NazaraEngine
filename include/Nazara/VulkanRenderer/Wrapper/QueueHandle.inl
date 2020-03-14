// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/Wrapper/QueueHandle.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/StackArray.hpp>
#include <Nazara/VulkanRenderer/Utils.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	namespace Vk
	{
		inline QueueHandle::QueueHandle() :
		m_handle(VK_NULL_HANDLE),
		m_lastErrorCode(VkResult::VK_SUCCESS)
		{
		}

		inline QueueHandle::QueueHandle(Device& device, VkQueue queue) :
		m_device(&device),
		m_handle(queue),
		m_lastErrorCode(VkResult::VK_SUCCESS)
		{
		}

		inline Device& QueueHandle::GetDevice() const
		{
			return *m_device;
		}

		inline VkResult QueueHandle::GetLastErrorCode() const
		{
			return m_lastErrorCode;
		}

		inline bool QueueHandle::Present(const VkPresentInfoKHR& presentInfo) const
		{
			m_lastErrorCode = m_device->vkQueuePresentKHR(m_handle, &presentInfo);
			if (m_lastErrorCode != VkResult::VK_SUCCESS)
			{
				NazaraError("Failed to present queue: " + TranslateVulkanError(m_lastErrorCode));
				return false;
			}

			return true;
		}

		inline bool QueueHandle::Present(VkSwapchainKHR swapchain, UInt32 imageIndex, VkSemaphore waitSemaphore) const
		{
			VkPresentInfoKHR presentInfo =
			{
				VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
				nullptr,
				(waitSemaphore) ? 1U : 0U,
				&waitSemaphore,
				1U,
				&swapchain,
				&imageIndex,
				nullptr
			};

			return Present(presentInfo);
		}

		inline bool QueueHandle::Submit(VkCommandBuffer commandBuffer, VkSemaphore waitSemaphore, VkPipelineStageFlags waitStage, VkSemaphore signalSemaphore, VkFence signalFence) const
		{
			return Submit(1U, &commandBuffer, waitSemaphore, waitStage, signalSemaphore, signalFence);
		}

		inline bool QueueHandle::Submit(VkCommandBuffer commandBuffer, std::initializer_list<VkSemaphore> waitSemaphores, VkPipelineStageFlags waitStage, std::initializer_list<VkSemaphore> signalSemaphores, VkFence signalFence) const
		{
			return Submit(1U, &commandBuffer, waitSemaphores, waitStage, signalSemaphores, signalFence);
		}

		inline bool QueueHandle::Submit(UInt32 commandBufferCount, const VkCommandBuffer* commandBuffers, std::initializer_list<VkSemaphore> waitSemaphores, VkPipelineStageFlags waitStage, std::initializer_list<VkSemaphore> signalSemaphores, VkFence signalFence) const
		{
			// Make continuous array of semaphores (initializer_list doesn't have that guarantee)
			StackArray<VkSemaphore> signalSemaphoresCont = NazaraStackArrayNoInit(VkSemaphore, signalSemaphores.size());
			StackArray<VkSemaphore> waitSemaphoresCont = NazaraStackArrayNoInit(VkSemaphore, waitSemaphores.size());
			std::size_t i;

			i = 0;
			for (VkSemaphore semaphore : signalSemaphores)
				signalSemaphoresCont[i++] = semaphore;

			i = 0;
			for (VkSemaphore semaphore : waitSemaphores)
				waitSemaphoresCont[i++] = semaphore;

			return Submit(commandBufferCount, commandBuffers, UInt32(waitSemaphoresCont.size()), waitSemaphoresCont.data(), waitStage, UInt32(signalSemaphoresCont.size()), signalSemaphoresCont.data(), signalFence);
		}

		inline bool QueueHandle::Submit(UInt32 commandBufferCount, const VkCommandBuffer* commandBuffers, VkSemaphore waitSemaphore, VkPipelineStageFlags waitStage, VkSemaphore signalSemaphore, VkFence signalFence) const
		{
			return Submit(commandBufferCount, commandBuffers, (waitSemaphore) ? 1U : 0U, &waitSemaphore, waitStage, (signalSemaphore) ? 1U : 0U, &signalSemaphore, signalFence);
		}

		inline bool QueueHandle::Submit(UInt32 commandBufferCount, const VkCommandBuffer* commandBuffers, UInt32 waitSemaphoreCount, const VkSemaphore* waitSemaphores, VkPipelineStageFlags waitStage, UInt32 signalSemaphoreCount, const VkSemaphore* signalSemaphores, VkFence signalFence) const
		{
			VkSubmitInfo submitInfo = {
				VK_STRUCTURE_TYPE_SUBMIT_INFO,
				nullptr,
				waitSemaphoreCount,
				waitSemaphores,
				&waitStage,
				commandBufferCount,
				commandBuffers,
				signalSemaphoreCount,
				signalSemaphores
			};

			return Submit(submitInfo, signalFence);
		}

		inline bool QueueHandle::Submit(const VkSubmitInfo& submit, VkFence signalFence) const
		{
			return Submit(1, &submit, signalFence);
		}

		inline bool QueueHandle::Submit(UInt32 submitCount, const VkSubmitInfo* submits, VkFence signalFence) const
		{
			m_lastErrorCode = m_device->vkQueueSubmit(m_handle, submitCount, submits, signalFence);
			if (m_lastErrorCode != VkResult::VK_SUCCESS)
			{
				NazaraError("Failed to submit queue: " + TranslateVulkanError(m_lastErrorCode));
				return false;
			}

			return true;
		}

		inline bool QueueHandle::WaitIdle() const
		{
			m_lastErrorCode = m_device->vkQueueWaitIdle(m_handle);
			if (m_lastErrorCode != VkResult::VK_SUCCESS)
			{
				NazaraError("Failed to wait for queue: " + TranslateVulkanError(m_lastErrorCode));
				return false;
			}

			return true;
		}

		inline QueueHandle::operator VkQueue()
		{
			return m_handle;
		}
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
