// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_RENDERWINDOW_HPP
#define NAZARA_OPENGLRENDERER_RENDERWINDOW_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Renderer/RendererImpl.hpp>
#include <Nazara/Renderer/RenderWindowImpl.hpp>
#include <Nazara/OpenGLRenderer/Config.hpp>
#include <Nazara/OpenGLRenderer/OpenGLDevice.hpp>
#include <Nazara/OpenGLRenderer/OpenGLMultipleFramebuffer.hpp>
#include <Nazara/OpenGLRenderer/OpenGLRenderImage.hpp>
#include <Nazara/OpenGLRenderer/OpenGLRenderPass.hpp>
#include <Nazara/OpenGLRenderer/VkRenderTarget.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/CommandBuffer.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/CommandPool.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Device.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/DeviceMemory.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Framebuffer.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Image.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/QueueHandle.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Surface.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Swapchain.hpp>
#include <optional>
#include <vector>

namespace Nz
{
	class NAZARA_OPENGLRENDERER_API OpenGLRenderWindow : public RenderWindowImpl
	{
		public:
			VkRenderWindow();
			VkRenderWindow(const VkRenderWindow&) = delete;
			VkRenderWindow(VkRenderWindow&&) = delete; ///TODO
			virtual ~VkRenderWindow();

			OpenGLRenderImage& Acquire() override;

			bool Create(RendererImpl* renderer, RenderSurface* surface, const Vector2ui& size, const RenderWindowParameters& parameters) override;
			std::unique_ptr<CommandPool> CreateCommandPool(QueueType queueType) override;

			inline const OpenGLFramebuffer& GetFramebuffer() const override;
			inline OpenGLDevice& GetDevice();
			inline const OpenGLDevice& GetDevice() const;
			inline Vk::QueueHandle& GetGraphicsQueue();
			const OpenGLRenderPass& GetRenderPass() const override;
			inline const Vk::Swapchain& GetSwapchain() const;

			std::shared_ptr<RenderDevice> GetRenderDevice() override;

			void Present(UInt32 imageIndex, VkSemaphore waitSemaphore = VK_NULL_HANDLE);

			VkRenderWindow& operator=(const VkRenderWindow&) = delete;
			VkRenderWindow& operator=(VkRenderWindow&&) = delete; ///TODO

		private:
			bool SetupDepthBuffer(const Vector2ui& size);
			bool SetupRenderPass();
			bool SetupSwapchain(const Vk::PhysicalDevice& deviceInfo, Vk::Surface& surface, const Vector2ui& size);

			std::size_t m_currentFrame;
			Clock m_clock;
			VkFormat m_depthStencilFormat;
			VkSurfaceFormatKHR m_surfaceFormat;
			std::optional<OpenGLMultipleFramebuffer> m_framebuffer;
			std::optional<OpenGLRenderPass> m_renderPass;
			std::shared_ptr<OpenGLDevice> m_device;
			std::vector<Vk::Fence*> m_inflightFences;
			std::vector<OpenGLRenderImage> m_concurrentImageData;
			Vk::DeviceMemory m_depthBufferMemory;
			Vk::Image m_depthBuffer;
			Vk::ImageView m_depthBufferView;
			Vk::QueueHandle m_graphicsQueue;
			Vk::QueueHandle m_presentQueue;
			Vk::QueueHandle m_transferQueue;
			Vk::Swapchain m_swapchain;
	};
}

#include <Nazara/OpenGLRenderer/VkRenderWindow.inl>

#endif // NAZARA_OPENGLRENDERER_RENDERWINDOW_HPP
