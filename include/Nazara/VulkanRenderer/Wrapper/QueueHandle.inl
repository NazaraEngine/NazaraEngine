// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/VulkanRenderer/Utils.hpp>
#include <NazaraUtils/StackArray.hpp>

namespace Nz::Vk
{
	inline QueueHandle::QueueHandle() :
	m_handle(VK_NULL_HANDLE),
	m_lastErrorCode(VkResult::VK_SUCCESS)
	{
	}

	inline QueueHandle::QueueHandle(Device& device, VkQueue queue, UInt32 queueFamilyIndex) :
	m_device(&device),
	m_handle(queue),
	m_lastErrorCode(VkResult::VK_SUCCESS),
	m_queueFamilyIndex(queueFamilyIndex)
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

	inline UInt32 QueueHandle::GetQueueFamilyIndex() const
	{
		return m_queueFamilyIndex;
	}

	inline bool QueueHandle::Present(const VkPresentInfoKHR& presentInfo) const
	{
		m_lastErrorCode = m_device->vkQueuePresentKHR(m_handle, &presentInfo);
		return (m_lastErrorCode != VkResult::VK_SUCCESS);
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

	inline bool QueueHandle::Submit(VkCommandBuffer commandBuffer, VkFence signalFence) const
	{
		return Submit(1U, &commandBuffer, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, signalFence);
	}

	inline bool QueueHandle::Submit(VkCommandBuffer commandBuffer, VkSemaphore waitSemaphore, VkPipelineStageFlags waitStage, VkSemaphore signalSemaphore, VkFence signalFence) const
	{
		return Submit(1U, &commandBuffer, waitSemaphore, waitStage, signalSemaphore, signalFence);
	}

	inline bool QueueHandle::Submit(VkCommandBuffer commandBuffer, std::initializer_list<VkSemaphore> waitSemaphores, std::initializer_list<VkPipelineStageFlags> waitStage, std::initializer_list<VkSemaphore> signalSemaphores, VkFence signalFence) const
	{
		return Submit(1U, &commandBuffer, waitSemaphores, waitStage, signalSemaphores, signalFence);
	}

	inline bool QueueHandle::Submit(UInt32 commandBufferCount, const VkCommandBuffer* commandBuffers, std::initializer_list<VkSemaphore> waitSemaphores, std::initializer_list<VkPipelineStageFlags> waitStages, std::initializer_list<VkSemaphore> signalSemaphores, VkFence signalFence) const
	{
		NazaraAssert(waitSemaphores.size() == waitStages.size(), "Wait stage count must match wait semaphores count");

		// Make continuous array of semaphores (initializer_list doesn't have that guarantee)
		StackArray<VkSemaphore> signalSemaphoresCont = NazaraStackArrayNoInit(VkSemaphore, signalSemaphores.size());
		StackArray<VkSemaphore> waitSemaphoresCont = NazaraStackArrayNoInit(VkSemaphore, waitSemaphores.size());
		StackArray<VkPipelineStageFlags> waitStageCont = NazaraStackArrayNoInit(VkPipelineStageFlags, waitStages.size());
		std::size_t i;

		i = 0;
		for (VkSemaphore semaphore : signalSemaphores)
			signalSemaphoresCont[i++] = semaphore;

		i = 0;
		for (VkSemaphore semaphore : waitSemaphores)
			waitSemaphoresCont[i++] = semaphore;

		i = 0;
		for (VkPipelineStageFlags flags : waitStages)
			waitStageCont[i++] = flags;

		return Submit(commandBufferCount, commandBuffers, UInt32(waitSemaphoresCont.size()), waitSemaphoresCont.data(), waitStageCont.data(), UInt32(signalSemaphoresCont.size()), signalSemaphoresCont.data(), signalFence);
	}

	inline bool QueueHandle::Submit(UInt32 commandBufferCount, const VkCommandBuffer* commandBuffers, VkSemaphore waitSemaphore, VkPipelineStageFlags waitStage, VkSemaphore signalSemaphore, VkFence signalFence) const
	{
		return Submit(commandBufferCount, commandBuffers, (waitSemaphore) ? 1U : 0U, (waitSemaphore) ? &waitSemaphore : nullptr, &waitStage, (signalSemaphore) ? 1U : 0U, (signalSemaphore) ? &signalSemaphore : nullptr, signalFence);
	}

	inline bool QueueHandle::Submit(UInt32 commandBufferCount, const VkCommandBuffer* commandBuffers, UInt32 waitSemaphoreCount, const VkSemaphore* waitSemaphores, const VkPipelineStageFlags* waitStages, UInt32 signalSemaphoreCount, const VkSemaphore* signalSemaphores, VkFence signalFence) const
	{
		VkSubmitInfo submitInfo = {
			VK_STRUCTURE_TYPE_SUBMIT_INFO,
			nullptr,
			waitSemaphoreCount,
			waitSemaphores,
			waitStages,
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
			NazaraError("failed to submit queue: {0}", TranslateVulkanError(m_lastErrorCode));
			return false;
		}

		return true;
	}

	inline bool QueueHandle::WaitIdle() const
	{
		m_lastErrorCode = m_device->vkQueueWaitIdle(m_handle);
		if (m_lastErrorCode != VkResult::VK_SUCCESS)
		{
			NazaraError("failed to wait for queue: {0}", TranslateVulkanError(m_lastErrorCode));
			return false;
		}

		return true;
	}

	inline QueueHandle::operator VkQueue()
	{
		return m_handle;
	}
}

