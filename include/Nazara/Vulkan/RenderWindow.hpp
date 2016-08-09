// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERWINDOW_HPP
#define NAZARA_RENDERWINDOW_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Vulkan/Config.hpp>
#include <Nazara/Vulkan/RenderTarget.hpp>
#include <Nazara/Vulkan/VkCommandBuffer.hpp>
#include <Nazara/Vulkan/VkCommandPool.hpp>
#include <Nazara/Vulkan/VkDevice.hpp>
#include <Nazara/Vulkan/VkDeviceMemory.hpp>
#include <Nazara/Vulkan/VkFramebuffer.hpp>
#include <Nazara/Vulkan/VkImage.hpp>
#include <Nazara/Vulkan/VkSurface.hpp>
#include <Nazara/Vulkan/VkSwapchain.hpp>
#include <Nazara/Utility/Window.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_VULKAN_API RenderWindow : public RenderTarget, public Window
	{
		public:
			RenderWindow();
			RenderWindow(VideoMode mode, const String& title, UInt32 style = WindowStyle_Default);
			RenderWindow(WindowHandle handle);
			RenderWindow(const RenderWindow&) = delete;
			RenderWindow(RenderWindow&&) = delete; ///TODO
			virtual ~RenderWindow();

			bool Acquire(UInt32* index) const override;

			void BuildPreRenderCommands(UInt32 imageIndex, Vk::CommandBuffer& commandBuffer) override;
			void BuildPostRenderCommands(UInt32 imageIndex, Vk::CommandBuffer& commandBuffer) override;

			bool Create(VideoMode mode, const String& title, UInt32 style = WindowStyle_Default);
			bool Create(WindowHandle handle);

			const Vk::Framebuffer& GetFrameBuffer(UInt32 imageIndex) const override;
			UInt32 GetFramebufferCount() const;
			const Vk::DeviceHandle& GetDevice() const;
			UInt32 GetPresentableFamilyQueue() const;
			const Vk::Surface& GetSurface() const;
			const Vk::Swapchain& GetSwapchain() const;

			void Present(UInt32 imageIndex) override;

			bool IsValid() const;

			void SetDepthStencilFormats(std::vector<PixelFormatType> pixelFormat);
			void SetPhysicalDevice(VkPhysicalDevice device);

			RenderWindow& operator=(const RenderWindow&) = delete;
			RenderWindow& operator=(RenderWindow&&) = delete; ///TODO

		private:
			bool OnWindowCreated() override;
			void OnWindowDestroy() override;
			void OnWindowResized() override;

			bool SetupDepthBuffer();
			bool SetupRenderPass();
			bool SetupSwapchain();

			Clock m_clock;
			VkColorSpaceKHR m_colorSpace;
			VkFormat m_colorFormat;
			VkFormat m_depthStencilFormat;
			VkPhysicalDevice m_forcedPhysicalDevice;
			std::vector<PixelFormatType> m_wantedDepthStencilFormats;
			std::vector<Vk::Framebuffer> m_frameBuffers;
			Vk::DeviceHandle m_device;
			Vk::DeviceMemory m_depthBufferMemory;
			Vk::Image m_depthBuffer;
			Vk::ImageView m_depthBufferView;
			Vk::Queue m_presentQueue;
			Vk::Surface m_surface;
			Vk::Swapchain m_swapchain;
			UInt32 m_presentableFamilyQueue;
	};
}

#endif // NAZARA_RENDERWINDOW_HPP
