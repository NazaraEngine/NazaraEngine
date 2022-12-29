// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VULKANRENDERWINDOW_HPP
#define NAZARA_VULKANRENDERER_VULKANRENDERWINDOW_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Renderer/RenderWindowImpl.hpp>
#include <Nazara/Renderer/RendererImpl.hpp>
#include <Nazara/VulkanRenderer/Config.hpp>
#include <Nazara/VulkanRenderer/VulkanDevice.hpp>
#include <Nazara/VulkanRenderer/VulkanRenderImage.hpp>
#include <Nazara/VulkanRenderer/VulkanRenderPass.hpp>
#include <Nazara/VulkanRenderer/VulkanWindowFramebuffer.hpp>
#include <Nazara/VulkanRenderer/Wrapper/CommandBuffer.hpp>
#include <Nazara/VulkanRenderer/Wrapper/CommandPool.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Device.hpp>
#include <Nazara/VulkanRenderer/Wrapper/DeviceMemory.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Framebuffer.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Image.hpp>
#include <Nazara/VulkanRenderer/Wrapper/QueueHandle.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Surface.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Swapchain.hpp>
#include <optional>
#include <vector>

namespace Nz
{
	class NAZARA_VULKANRENDERER_API VulkanRenderWindow : public RenderWindowImpl
	{
		public:
			VulkanRenderWindow(RenderWindow& owner);
			VulkanRenderWindow(const VulkanRenderWindow&) = delete;
			VulkanRenderWindow(VulkanRenderWindow&&) = delete; ///TODO
			~VulkanRenderWindow();

			RenderFrame Acquire() override;

			bool Create(RendererImpl* renderer, RenderSurface* surface, const RenderWindowParameters& parameters) override;

			std::shared_ptr<CommandPool> CreateCommandPool(QueueType queueType) override;

			const VulkanWindowFramebuffer& GetFramebuffer(std::size_t i) const override;
			std::size_t GetFramebufferCount() const override;
			inline VulkanDevice& GetDevice();
			inline const VulkanDevice& GetDevice() const;
			inline Vk::QueueHandle& GetGraphicsQueue();
			const VulkanRenderPass& GetRenderPass() const override;
			const Vector2ui& GetSize() const override;
			inline const Vk::Swapchain& GetSwapchain() const;

			void Present(UInt32 imageIndex, VkSemaphore waitSemaphore = VK_NULL_HANDLE);

			VulkanRenderWindow& operator=(const VulkanRenderWindow&) = delete;
			VulkanRenderWindow& operator=(VulkanRenderWindow&&) = delete; ///TODO

		private:
			bool CreateSwapchain(Vk::Surface& surface, const Vector2ui& size);
			bool SetupDepthBuffer(const Vector2ui& size);
			bool SetupFrameBuffers(const Vector2ui& size);
			bool SetupRenderPass();
			bool SetupSwapchain(const Vk::PhysicalDevice& deviceInfo, Vk::Surface& surface, const Vector2ui& size);

			std::optional<VulkanRenderPass> m_renderPass;
			std::shared_ptr<VulkanDevice> m_device;
			std::size_t m_currentFrame;
			std::vector<VulkanWindowFramebuffer> m_framebuffers;
			std::vector<Vk::Fence*> m_inflightFences;
			std::vector<std::unique_ptr<VulkanRenderImage>> m_concurrentImageData;
			Vk::DeviceMemory m_depthBufferMemory;
			Vk::Image m_depthBuffer;
			Vk::ImageView m_depthBufferView;
			Vk::QueueHandle m_graphicsQueue;
			Vk::QueueHandle m_presentQueue;
			Vk::QueueHandle m_transferQueue;
			Vk::Swapchain m_swapchain;
			RenderWindow& m_owner;
			Vector2ui m_swapchainSize;
			VkFormat m_depthStencilFormat;
			VkSurfaceFormatKHR m_surfaceFormat;
			bool m_shouldRecreateSwapchain;
	};
}

#include <Nazara/VulkanRenderer/VulkanRenderWindow.inl>

#endif // NAZARA_VULKANRENDERER_VULKANRENDERWINDOW_HPP
