// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanRenderWindow.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	inline VulkanDevice& VulkanRenderWindow::GetDevice()
	{
		return *m_device;
	}

	inline const VulkanDevice& VulkanRenderWindow::GetDevice() const
	{
		return *m_device;
	}

	inline Vk::QueueHandle& VulkanRenderWindow::GetGraphicsQueue()
	{
		return m_graphicsQueue;
	}

	inline const Vk::Swapchain& VulkanRenderWindow::GetSwapchain() const
	{
		return m_swapchain;
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
