// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VULKANRENDERIMAGE_HPP
#define NAZARA_VULKANRENDERER_VULKANRENDERIMAGE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Renderer/RenderImage.hpp>
#include <Nazara/VulkanRenderer/VulkanUploadPool.hpp>
#include <Nazara/VulkanRenderer/Wrapper/CommandBuffer.hpp>
#include <Nazara/VulkanRenderer/Wrapper/CommandPool.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Fence.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Semaphore.hpp>

namespace Nz
{
	class VulkanSwapchain;

	class NAZARA_VULKANRENDERER_API VulkanRenderImage : public RenderImage
	{
		public:
			VulkanRenderImage(VulkanSwapchain& owner);
			VulkanRenderImage(const VulkanRenderImage&) = delete;
			VulkanRenderImage(VulkanRenderImage&&) = delete;
			~VulkanRenderImage() = default;

			void Execute(const FunctionRef<void(CommandBufferBuilder& builder)>& callback, QueueTypeFlags queueTypeFlags) override;

			inline Vk::Fence& GetInFlightFence();
			inline Vk::Semaphore& GetImageAvailableSemaphore();
			UInt32 GetImageIndex() const override;
			inline Vk::Semaphore& GetRenderFinishedSemaphore();
			VulkanUploadPool& GetUploadPool() override;

			void Present() override;

			inline void Reset(UInt32 imageIndex);

			void SubmitCommandBuffer(CommandBuffer* commandBuffer, QueueTypeFlags queueTypeFlags) override;
			void SubmitCommandBuffer(VkCommandBuffer commandBuffer, QueueTypeFlags queueTypeFlags);

			VulkanRenderImage& operator=(const VulkanRenderImage&) = delete;
			VulkanRenderImage& operator=(VulkanRenderImage&&) = delete;

		private:
			std::size_t m_freeCommandBufferIndex;
			std::vector<VkCommandBuffer> m_allocatedCommandBuffers;
			std::vector<VkCommandBuffer> m_graphicalCommandBuffers;
			VulkanSwapchain& m_owner;
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
