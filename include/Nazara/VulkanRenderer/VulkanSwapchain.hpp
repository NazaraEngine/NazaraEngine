// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VULKANSWAPCHAIN_HPP
#define NAZARA_VULKANRENDERER_VULKANSWAPCHAIN_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Renderer/RendererImpl.hpp>
#include <Nazara/Renderer/Swapchain.hpp>
#include <Nazara/Renderer/SwapchainParameters.hpp>
#include <Nazara/VulkanRenderer/Export.hpp>
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
	class NAZARA_VULKANRENDERER_API VulkanSwapchain : public Swapchain
	{
		public:
			VulkanSwapchain(VulkanDevice& device, WindowHandle windowHandle, const Vector2ui& windowSize, const SwapchainParameters& parameters);
			VulkanSwapchain(const VulkanSwapchain&) = delete;
			VulkanSwapchain(VulkanSwapchain&&) = delete;
			~VulkanSwapchain();

			RenderFrame AcquireFrame() override;

			std::shared_ptr<CommandPool> CreateCommandPool(QueueType queueType) override;

			const VulkanWindowFramebuffer& GetFramebuffer(std::size_t imageIndex) const override;
			std::size_t GetFramebufferCount() const override;
			inline VulkanDevice& GetDevice();
			inline const VulkanDevice& GetDevice() const;
			inline Vk::QueueHandle& GetGraphicsQueue();
			inline VkImage GetImage(std::size_t imageIndex) const;
			const VulkanRenderPass& GetRenderPass() const override;
			const Vector2ui& GetSize() const override;
			PresentMode GetPresentMode() const override;
			PresentModeFlags GetSupportedPresentModes() const override;
			inline const Vk::Swapchain& GetSwapchain() const;
			RenderResources& GetTransientResources() override;

			void NotifyResize(const Vector2ui& newSize) override;

			void Present(UInt32 imageIndex, VkSemaphore waitSemaphore = VK_NULL_HANDLE);

			void SetPresentMode(PresentMode presentMode) override;

			VulkanSwapchain& operator=(const VulkanSwapchain&) = delete;
			VulkanSwapchain& operator=(VulkanSwapchain&&) = delete;

		private:
			bool CreateSwapchain();
			bool SetupDepthBuffer();
			bool SetupFrameBuffers();
			bool SetupRenderPass();
			bool SetupSurface(WindowHandle windowHandle);
			bool SetupSwapchain(const Vk::PhysicalDevice& deviceInfo);

			std::optional<VulkanRenderPass> m_renderPass;
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
			Vk::Surface m_surface;
			Vk::Swapchain m_swapchain;
			PresentMode m_presentMode;
			PresentModeFlags m_supportedPresentModes;
			Vector2ui m_swapchainSize;
			VkFormat m_depthStencilFormat;
			VkSurfaceFormatKHR m_surfaceFormat;
			VulkanDevice& m_device;
			bool m_shouldRecreateSwapchain;
	};
}

#include <Nazara/VulkanRenderer/VulkanSwapchain.inl>

#endif // NAZARA_VULKANRENDERER_VULKANSWAPCHAIN_HPP
