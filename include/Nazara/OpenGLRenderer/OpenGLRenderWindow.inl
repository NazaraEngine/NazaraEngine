// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/VkRenderWindow.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	inline const OpenGLMultipleFramebuffer& VkRenderWindow::GetFramebuffer() const
	{
		return *m_framebuffer;
	}

	inline OpenGLDevice& VkRenderWindow::GetDevice()
	{
		return *m_device;
	}

	inline const OpenGLDevice& VkRenderWindow::GetDevice() const
	{
		return *m_device;
	}

	inline Vk::QueueHandle& VkRenderWindow::GetGraphicsQueue()
	{
		return m_graphicsQueue;
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
		NazaraAssert(imageIndex < m_inflightFences.size(), "Invalid image index");

		m_presentQueue.Present(m_swapchain, imageIndex, waitSemaphore);

		m_currentFrame = (m_currentFrame + 1) % m_inflightFences.size();
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
