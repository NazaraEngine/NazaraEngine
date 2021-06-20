// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VkRenderWindow.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	inline const VulkanMultipleFramebuffer& VkRenderWindow::GetFramebuffer() const
	{
		return *m_framebuffer;
	}

	inline VulkanDevice& VkRenderWindow::GetDevice()
	{
		return *m_device;
	}

	inline const VulkanDevice& VkRenderWindow::GetDevice() const
	{
		return *m_device;
	}

	inline Vk::QueueHandle& VkRenderWindow::GetGraphicsQueue()
	{
		return m_graphicsQueue;
	}

	inline const VulkanRenderPass& VkRenderWindow::GetRenderPass() const
	{
		return *m_renderPass;
	}

	inline const Vk::Swapchain& VkRenderWindow::GetSwapchain() const
	{
		return m_swapchain;
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
