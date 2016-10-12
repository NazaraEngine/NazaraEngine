// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VkQueue.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/VulkanRenderer/Utils.hpp>
#include <Nazara/VulkanRenderer/VkDevice.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	namespace Vk
	{
		inline Queue::Queue() :
		Queue(DeviceHandle(), VK_NULL_HANDLE)
		{
		}

		inline Queue::Queue(const DeviceHandle& device, VkQueue queue) :
		m_device(device),
		m_handle(queue),
		m_lastErrorCode(VkResult::VK_SUCCESS)
		{
		}

		inline Queue::Queue(const Queue& queue) :
		m_device(queue.m_device),
		m_handle(queue.m_handle),
		m_lastErrorCode(queue.m_lastErrorCode)
		{
		}
		
		inline Queue::Queue(Queue&& queue) :
		m_device(queue.m_device),
		m_handle(queue.m_handle),
		m_lastErrorCode(queue.m_lastErrorCode)
		{
		}

		inline const DeviceHandle& Queue::GetDevice() const
		{
			return m_device;
		}

		inline VkResult Queue::GetLastErrorCode() const
		{
			return m_lastErrorCode;
		}

		inline bool Queue::Present(const VkPresentInfoKHR& presentInfo) const
		{
			m_lastErrorCode = m_device->vkQueuePresentKHR(m_handle, &presentInfo);
			if (m_lastErrorCode != VkResult::VK_SUCCESS)
			{
				NazaraError("Failed to present queue: " + TranslateVulkanError(m_lastErrorCode));
				return false;
			}

			return true;
		}

		inline bool Queue::Present(VkSwapchainKHR swapchain, UInt32 imageIndex, VkSemaphore waitSemaphore) const
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

		inline bool Queue::Submit(const VkSubmitInfo& submit, VkFence fence) const
		{
			return Submit(1, &submit, fence);
		}

		inline bool Queue::Submit(UInt32 submitCount, const VkSubmitInfo* submits, VkFence fence) const
		{
			m_lastErrorCode = m_device->vkQueueSubmit(m_handle, submitCount, submits, fence);
			if (m_lastErrorCode != VkResult::VK_SUCCESS)
			{
				NazaraError("Failed to submit queue: " + TranslateVulkanError(m_lastErrorCode));
				return false;
			}

			return true;
		}

		inline bool Queue::WaitIdle() const
		{
			m_lastErrorCode = m_device->vkQueueWaitIdle(m_handle);
			if (m_lastErrorCode != VkResult::VK_SUCCESS)
			{
				NazaraError("Failed to wait for queue: " + TranslateVulkanError(m_lastErrorCode));
				return false;
			}

			return true;
		}

		inline Queue& Queue::operator=(Queue&& queue)
		{
			m_device = std::move(queue.m_device);
			m_handle = queue.m_handle;
			m_lastErrorCode = queue.m_lastErrorCode;

			return *this;
		}

		inline Queue::operator VkQueue()
		{
			return m_handle;
		}
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
