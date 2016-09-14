// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VkRenderWindow.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	inline const Vk::DeviceHandle& VkRenderWindow::GetDevice() const
	{
		return m_device;
	}

	inline const Vk::Framebuffer& VkRenderWindow::GetFrameBuffer(UInt32 imageIndex) const
	{
		return m_frameBuffers[imageIndex];
	}

	inline UInt32 VkRenderWindow::GetFramebufferCount() const
	{
		return static_cast<UInt32>(m_frameBuffers.size());
	}

	inline UInt32 VkRenderWindow::GetPresentableFamilyQueue() const
	{
		return m_presentableFamilyQueue;
	}

	inline const Vk::Surface& VkRenderWindow::GetSurface() const
	{
		return m_surface;
	}

	inline const Vk::Swapchain& VkRenderWindow::GetSwapchain() const
	{
		return m_swapchain;
	}

	inline void VkRenderWindow::Present(UInt32 imageIndex)
	{
		NazaraAssert(imageIndex < m_frameBuffers.size(), "Invalid image index");

		m_presentQueue.Present(m_swapchain, imageIndex);
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
