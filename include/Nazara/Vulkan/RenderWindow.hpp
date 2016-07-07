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
#include <Nazara/Vulkan/VkFramebuffer.hpp>
#include <Nazara/Vulkan/VkSemaphore.hpp>
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

			bool Acquire(const Vk::Framebuffer** framebuffer) const override;

			bool Create(VideoMode mode, const String& title, UInt32 style = WindowStyle_Default);
			bool Create(WindowHandle handle);

			const Vk::DeviceHandle& GetDevice() const;
			UInt32 GetPresentableFamilyQueue() const;
			const Vk::Surface& GetSurface() const;
			const Vk::Swapchain& GetSwapchain() const;

			void Present() override;

			bool IsValid() const;

			void SetPhysicalDevice(VkPhysicalDevice device);

			RenderWindow& operator=(const RenderWindow&) = delete;
			RenderWindow& operator=(RenderWindow&&) = delete; ///TODO

		private:
			bool OnWindowCreated() override;
			void OnWindowDestroy() override;
			void OnWindowResized() override;

			bool SetupRenderPass(VkFormat colorFormat, VkFormat depthFormat);

			struct ImageData
			{
				Vk::CommandBuffer drawToPresentCmd;
				Vk::CommandBuffer presentToDrawCmd;
				Vk::Framebuffer frameBuffer;
			};

			Clock m_clock;
			VkPhysicalDevice m_forcedPhysicalDevice;
			std::vector<ImageData> m_images;
			Vk::CommandPool m_cmdPool;
			Vk::DeviceHandle m_device;
			Vk::Queue m_presentQueue;
			Vk::Semaphore m_imageReadySemaphore;
			Vk::Surface m_surface;
			Vk::Swapchain m_swapchain;
			mutable UInt32 m_lastImageAcquired;
			UInt32 m_presentableFamilyQueue;
	};
}

#endif // NAZARA_RENDERWINDOW_HPP
