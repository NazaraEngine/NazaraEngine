// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VULKANRENDERIMAGE_HPP
#define NAZARA_VULKANRENDERER_VULKANRENDERIMAGE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/RenderImage.hpp>
#include <Nazara/VulkanRenderer/VulkanUploadPool.hpp>
#include <Nazara/VulkanRenderer/Wrapper/CommandBuffer.hpp>
#include <Nazara/VulkanRenderer/Wrapper/CommandPool.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Fence.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Semaphore.hpp>
#include <vector>

namespace Nz
{
	class VkRenderWindow;

	class NAZARA_VULKANRENDERER_API VulkanRenderImage : public RenderImage
	{
		public:
			VulkanRenderImage(VkRenderWindow& owner);
			VulkanRenderImage(const VulkanRenderImage&) = delete;
			VulkanRenderImage(VulkanRenderImage&&) noexcept = default;
			~VulkanRenderImage();

			void Execute(const std::function<void(CommandBufferBuilder& builder)>& callback, QueueTypeFlags queueTypeFlags) override;

			inline Vk::Fence& GetInFlightFence();
			inline Vk::Semaphore& GetImageAvailableSemaphore();
			inline UInt32 GetImageIndex();
			inline Vk::Semaphore& GetRenderFinishedSemaphore();
			VulkanUploadPool& GetUploadPool() override;

			void SubmitCommandBuffer(CommandBuffer* commandBuffer, QueueTypeFlags queueTypeFlags) override;
			void SubmitCommandBuffer(VkCommandBuffer commandBuffer, QueueTypeFlags queueTypeFlags);

			void Present() override;

			inline void Reset(UInt32 imageIndex);

			VulkanRenderImage& operator=(const VulkanRenderImage&) = delete;
			VulkanRenderImage& operator=(VulkanRenderImage&&) = delete;

		private:
			std::size_t m_currentCommandBuffer;
			std::vector<Vk::AutoCommandBuffer> m_inFlightCommandBuffers;
			std::vector<VkCommandBuffer> m_graphicalCommandsBuffers;
			VkRenderWindow& m_owner;
			Vk::CommandPool m_commandPool;
			Vk::Fence m_inFlightFence;
			Vk::Semaphore m_imageAvailableSemaphore;
			Vk::Semaphore m_renderFinishedSemaphore;
			VulkanUploadPool m_uploadPool;
			UInt32 m_imageIndex;
	};
}

#include <Nazara/VulkanRenderer/VulkanRenderImage.inl>

#endif // NAZARA_VULKANRENDERER_VULKANRENDERIMAGE_HPP
