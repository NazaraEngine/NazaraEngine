// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Vulkan/VkCommandPool.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Vulkan/VkDevice.hpp>
#include <Nazara/Vulkan/Debug.hpp>

namespace Nz
{
	namespace Vk
	{
		inline Queue::Queue(Device& device, VkQueue queue) :
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

		inline Device& Queue::GetDevice()
		{
			return m_device;
		}

		inline VkResult Queue::GetLastErrorCode() const
		{
			return m_lastErrorCode;
		}

		inline bool Queue::Present(const VkPresentInfoKHR& presentInfo)
		{
			m_lastErrorCode = m_device.vkQueuePresentKHR(m_handle, &presentInfo);
			if (m_lastErrorCode != VkResult::VK_SUCCESS)
				return false;

			return true;
		}

		inline bool Queue::Present(VkSwapchainKHR swapchain, UInt32 imageIndex, VkSemaphore waitSemaphore)
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

		inline bool Queue::Submit(const VkSubmitInfo& submit, VkFence fence)
		{
			return Submit(1, &submit, fence);
		}

		inline bool Queue::Submit(UInt32 submitCount, const VkSubmitInfo* submits, VkFence fence)
		{
			m_lastErrorCode = m_device.vkQueueSubmit(m_handle, submitCount, submits, fence);
			if (m_lastErrorCode != VkResult::VK_SUCCESS)
				return false;

			return true;
		}

		inline bool Queue::WaitIdle()
		{
			m_lastErrorCode = m_device.vkQueueWaitIdle(m_handle);
			if (m_lastErrorCode != VkResult::VK_SUCCESS)
				return false;

			return true;
		}

		inline Queue::operator VkQueue()
		{
			return m_handle;
		}

	}
}

#include <Nazara/Vulkan/DebugOff.hpp>
