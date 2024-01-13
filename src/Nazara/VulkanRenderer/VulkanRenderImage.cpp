// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanRenderImage.hpp>
#include <Nazara/VulkanRenderer/VulkanCommandBuffer.hpp>
#include <Nazara/VulkanRenderer/VulkanCommandBufferBuilder.hpp>
#include <Nazara/VulkanRenderer/VulkanSwapchain.hpp>
#include <cassert>
#include <stdexcept>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	VulkanRenderImage::VulkanRenderImage(VulkanSwapchain& owner) :
	RenderImage(owner.GetDevice()),
	m_freeCommandBufferIndex(0),
	m_owner(owner),
	m_uploadPool(m_owner.GetDevice(), 2 * 1024 * 1024)
	{
		Vk::QueueHandle& graphicsQueue = m_owner.GetGraphicsQueue();
		if (!m_commandPool.Create(m_owner.GetDevice(), graphicsQueue.GetQueueFamilyIndex(), VK_COMMAND_POOL_CREATE_TRANSIENT_BIT))
			throw std::runtime_error("failed to create command pool: " + TranslateVulkanError(m_commandPool.GetLastErrorCode()));

		if (!m_imageAvailableSemaphore.Create(m_owner.GetDevice()))
			throw std::runtime_error("failed to create image available semaphore: " + TranslateVulkanError(m_imageAvailableSemaphore.GetLastErrorCode()));

		if (!m_renderFinishedSemaphore.Create(m_owner.GetDevice()))
			throw std::runtime_error("failed to create image finished semaphore: " + TranslateVulkanError(m_renderFinishedSemaphore.GetLastErrorCode()));

		if (!m_inFlightFence.Create(m_owner.GetDevice(), VK_FENCE_CREATE_SIGNALED_BIT))
			throw std::runtime_error("failed to create in-flight fence: " + TranslateVulkanError(m_inFlightFence.GetLastErrorCode()));
	}

	void VulkanRenderImage::Execute(const FunctionRef<void(CommandBufferBuilder& builder)>& callback, QueueTypeFlags queueTypeFlags)
	{
		if (m_freeCommandBufferIndex >= m_allocatedCommandBuffers.size())
		{
			assert(m_freeCommandBufferIndex == m_allocatedCommandBuffers.size());
			m_allocatedCommandBuffers.push_back(m_commandPool.AllocateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY));
		}

		Vk::CommandBuffer commandBuffer(m_commandPool, m_allocatedCommandBuffers[m_freeCommandBufferIndex++]);
		if (!commandBuffer.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT))
			throw std::runtime_error("failed to begin command buffer: " + TranslateVulkanError(commandBuffer.GetLastErrorCode()));

		VulkanCommandBufferBuilder builder(commandBuffer);
		callback(builder);

		if (!commandBuffer.End())
			throw std::runtime_error("failed to build command buffer: " + TranslateVulkanError(commandBuffer.GetLastErrorCode()));

		SubmitCommandBuffer(commandBuffer, queueTypeFlags);
	}

	UInt32 VulkanRenderImage::GetImageIndex() const
	{
		return m_imageIndex;
	}

	VulkanUploadPool& VulkanRenderImage::GetUploadPool()
	{
		return m_uploadPool;
	}

	void VulkanRenderImage::Present()
	{
		Vk::QueueHandle& graphicsQueue = m_owner.GetGraphicsQueue();
		if (!graphicsQueue.Submit(UInt32(m_graphicalCommandBuffers.size()), m_graphicalCommandBuffers.data(), m_imageAvailableSemaphore, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, m_renderFinishedSemaphore, m_inFlightFence))
			throw std::runtime_error("Failed to submit command buffers: " + TranslateVulkanError(graphicsQueue.GetLastErrorCode()));

		m_owner.Present(m_imageIndex, m_renderFinishedSemaphore);
	}

	void VulkanRenderImage::SubmitCommandBuffer(CommandBuffer* commandBuffer, QueueTypeFlags queueTypeFlags)
	{
		VulkanCommandBuffer& vkCommandBuffer = *SafeCast<VulkanCommandBuffer*>(commandBuffer);

		return SubmitCommandBuffer(vkCommandBuffer.GetCommandBuffer(), queueTypeFlags);
	}

	void VulkanRenderImage::SubmitCommandBuffer(VkCommandBuffer commandBuffer, QueueTypeFlags queueTypeFlags)
	{
		if (queueTypeFlags & QueueType::Graphics)
			m_graphicalCommandBuffers.push_back(commandBuffer);
		else
		{
			Vk::QueueHandle& graphicsQueue = m_owner.GetGraphicsQueue();
			if (!graphicsQueue.Submit(commandBuffer))
				throw std::runtime_error("Failed to submit command buffer: " + TranslateVulkanError(graphicsQueue.GetLastErrorCode()));
		}
	}
}
