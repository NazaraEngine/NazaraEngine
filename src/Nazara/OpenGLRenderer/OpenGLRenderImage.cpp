// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#if 0

#include <Nazara/OpenGLRenderer/OpenGLRenderImage.hpp>
#include <Nazara/OpenGLRenderer/VkRenderWindow.hpp>
#include <Nazara/OpenGLRenderer/OpenGLCommandBuffer.hpp>
#include <Nazara/OpenGLRenderer/OpenGLCommandBufferBuilder.hpp>
#include <stdexcept>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	OpenGLRenderImage::OpenGLRenderImage(VkRenderWindow& owner) :
	m_owner(owner),
	m_uploadPool(m_owner.GetDevice(), 2 * 1024 * 1024)
	{
		Vk::QueueHandle& graphicsQueue = m_owner.GetGraphicsQueue();
		if (!m_commandPool.Create(m_owner.GetDevice(), graphicsQueue.GetQueueFamilyIndex(), VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT))
			throw std::runtime_error("failed to create command pool: " + TranslateOpenGLError(m_commandPool.GetLastErrorCode()));

		if (!m_imageAvailableSemaphore.Create(m_owner.GetDevice()))
			throw std::runtime_error("failed to create image available semaphore: " + TranslateOpenGLError(m_imageAvailableSemaphore.GetLastErrorCode()));

		if (!m_renderFinishedSemaphore.Create(m_owner.GetDevice()))
			throw std::runtime_error("failed to create image finished semaphore: " + TranslateOpenGLError(m_imageAvailableSemaphore.GetLastErrorCode()));

		if (!m_inFlightFence.Create(m_owner.GetDevice(), VK_FENCE_CREATE_SIGNALED_BIT))
			throw std::runtime_error("failed to create in-flight fence: " + TranslateOpenGLError(m_inFlightFence.GetLastErrorCode()));
	}

	OpenGLRenderImage::~OpenGLRenderImage()
	{
		m_inFlightCommandBuffers.clear();
	}

	void OpenGLRenderImage::Execute(const std::function<void(CommandBufferBuilder& builder)>& callback, QueueTypeFlags queueTypeFlags)
	{
		Vk::CommandBuffer* commandBuffer;
		if (m_currentCommandBuffer >= m_inFlightCommandBuffers.size())
		{
			Vk::AutoCommandBuffer& newlyAllocatedBuffer = m_inFlightCommandBuffers.emplace_back(m_commandPool.AllocateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY));
			commandBuffer = &newlyAllocatedBuffer.Get();
			m_currentCommandBuffer++;
		}
		else
			commandBuffer = &m_inFlightCommandBuffers[m_currentCommandBuffer++].Get();

		if (!commandBuffer->Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT))
			throw std::runtime_error("failed to begin command buffer: " + TranslateOpenGLError(commandBuffer->GetLastErrorCode()));

		OpenGLCommandBufferBuilder builder(*commandBuffer, m_imageIndex);
		callback(builder);

		if (!commandBuffer->End())
			throw std::runtime_error("failed to build command buffer: " + TranslateOpenGLError(commandBuffer->GetLastErrorCode()));

		SubmitCommandBuffer(*commandBuffer, queueTypeFlags);
	}

	OpenGLUploadPool& OpenGLRenderImage::GetUploadPool()
	{
		return m_uploadPool;
	}

	void OpenGLRenderImage::SubmitCommandBuffer(CommandBuffer* commandBuffer, QueueTypeFlags queueTypeFlags)
	{
		OpenGLCommandBuffer& vkCommandBuffer = *static_cast<OpenGLCommandBuffer*>(commandBuffer);

		return SubmitCommandBuffer(vkCommandBuffer.GetCommandBuffer(m_imageIndex), queueTypeFlags);
	}

	void OpenGLRenderImage::SubmitCommandBuffer(VkCommandBuffer commandBuffer, QueueTypeFlags queueTypeFlags)
	{
		if (queueTypeFlags & QueueType::Graphics)
			m_graphicalCommandsBuffers.push_back(commandBuffer);
		else
		{
			Vk::QueueHandle& graphicsQueue = m_owner.GetGraphicsQueue();
			if (!graphicsQueue.Submit(commandBuffer))
				throw std::runtime_error("Failed to submit command buffer: " + TranslateOpenGLError(graphicsQueue.GetLastErrorCode()));
		}
	}

	void OpenGLRenderImage::Present()
	{
		Vk::QueueHandle& graphicsQueue = m_owner.GetGraphicsQueue();
		if (!graphicsQueue.Submit(UInt32(m_graphicalCommandsBuffers.size()), m_graphicalCommandsBuffers.data(), m_imageAvailableSemaphore, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, m_renderFinishedSemaphore, m_inFlightFence))
			throw std::runtime_error("Failed to submit command buffers: " + TranslateOpenGLError(graphicsQueue.GetLastErrorCode()));

		m_owner.Present(m_imageIndex, m_renderFinishedSemaphore);
	}
}

#endif
