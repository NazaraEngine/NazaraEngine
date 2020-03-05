// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VkRenderWindow.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	inline VulkanDevice& VkRenderWindow::GetDevice()
	{
		return *m_device;
	}

	inline const VulkanDevice& VkRenderWindow::GetDevice() const
	{
		return *m_device;
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

	inline const Vk::Swapchain& VkRenderWindow::GetSwapchain() const
	{
		return m_swapchain;
	}

	inline std::shared_ptr<RenderDevice> Nz::VkRenderWindow::GetRenderDevice()
	{
		return m_device;
	}

	inline void VkRenderWindow::Present(UInt32 imageIndex, VkSemaphore waitSemaphore)
	{
		NazaraAssert(imageIndex < m_frameBuffers.size(), "Invalid image index");

		m_presentQueue.Present(m_swapchain, imageIndex, waitSemaphore);
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
