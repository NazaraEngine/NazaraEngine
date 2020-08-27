// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_RENDERWINDOW_HPP
#define NAZARA_VULKANRENDERER_RENDERWINDOW_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Renderer/RendererImpl.hpp>
#include <Nazara/Renderer/RenderWindowImpl.hpp>
#include <Nazara/VulkanRenderer/Config.hpp>
#include <Nazara/VulkanRenderer/VulkanDevice.hpp>
#include <Nazara/VulkanRenderer/VulkanMultipleFramebuffer.hpp>
#include <Nazara/VulkanRenderer/VulkanRenderImage.hpp>
#include <Nazara/VulkanRenderer/VulkanRenderPass.hpp>
#include <Nazara/VulkanRenderer/VkRenderTarget.hpp>
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
	class NAZARA_VULKANRENDERER_API VkRenderWindow : public VkRenderTarget, public RenderWindowImpl
	{
		public:
			VkRenderWindow(RenderWindow& owner);
			VkRenderWindow(const VkRenderWindow&) = delete;
			VkRenderWindow(VkRenderWindow&&) = delete; ///TODO
			~VkRenderWindow();

			RenderFrame Acquire() override;

			bool Create(RendererImpl* renderer, RenderSurface* surface, const RenderWindowParameters& parameters) override;

			std::unique_ptr<CommandPool> CreateCommandPool(QueueType queueType) override;

			inline const VulkanMultipleFramebuffer& GetFramebuffer() const override;
			inline VulkanDevice& GetDevice();
			inline const VulkanDevice& GetDevice() const;
			inline Vk::QueueHandle& GetGraphicsQueue();
			inline const VulkanRenderPass& GetRenderPass() const override;
			inline const Vk::Swapchain& GetSwapchain() const;

			inline std::shared_ptr<RenderDevice> GetRenderDevice() override;

			void Present(UInt32 imageIndex, VkSemaphore waitSemaphore = VK_NULL_HANDLE);

			VkRenderWindow& operator=(const VkRenderWindow&) = delete;
			VkRenderWindow& operator=(VkRenderWindow&&) = delete; ///TODO

		private:
			bool CreateSwapchain(Vk::Surface& surface, const Vector2ui& size);
			bool SetupDepthBuffer(const Vector2ui& size);
			bool SetupFrameBuffers(const Vector2ui& size);
			bool SetupRenderPass();
			bool SetupSwapchain(const Vk::PhysicalDevice& deviceInfo, Vk::Surface& surface, const Vector2ui& size);

			std::optional<VulkanMultipleFramebuffer> m_framebuffer;
			std::optional<VulkanRenderPass> m_renderPass;
			std::shared_ptr<VulkanDevice> m_device;
			std::size_t m_currentFrame;
			std::vector<Vk::Fence*> m_inflightFences;
			std::vector<VulkanRenderImage> m_concurrentImageData;
			Vk::DeviceMemory m_depthBufferMemory;
			Vk::Image m_depthBuffer;
			Vk::ImageView m_depthBufferView;
			Vk::QueueHandle m_graphicsQueue;
			Vk::QueueHandle m_presentQueue;
			Vk::QueueHandle m_transferQueue;
			Vk::Swapchain m_swapchain;
			Clock m_clock;
			RenderWindow& m_owner;
			Vector2ui m_swapchainSize;
			VkFormat m_depthStencilFormat;
			VkSurfaceFormatKHR m_surfaceFormat;
			bool m_shouldRecreateSwapchain;
	};
}

#include <Nazara/VulkanRenderer/VkRenderWindow.inl>

#endif // NAZARA_VULKANRENDERER_RENDERWINDOW_HPP
