// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLRenderWindow.hpp>
#include <Nazara/OpenGLRenderer/DummySurface.hpp>
#include <Nazara/OpenGLRenderer/OpenGLRenderer.hpp>
#include <Nazara/Renderer/CommandPool.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	OpenGLRenderWindow::OpenGLRenderWindow() :
	m_currentFrame(0)
	{
	}

	OpenGLRenderImage& OpenGLRenderWindow::Acquire()
	{
		return m_renderImage[m_currentFrame];
	}

	bool OpenGLRenderWindow::Create(RendererImpl* renderer, RenderSurface* surface, const Vector2ui& size, const RenderWindowParameters& parameters)
	{
		DummySurface* dummySurface = static_cast<DummySurface*>(surface);
		OpenGLRenderer* glRenderer = static_cast<OpenGLRenderer*>(renderer);

		m_device = std::static_pointer_cast<OpenGLDevice>(glRenderer->InstanciateRenderDevice(0));

		GL::ContextParams contextParams;

		m_context = m_device->CreateContext(contextParams, dummySurface->GetWindowHandle());
		if (!m_context)
			return false;

		return true;
	}

	std::unique_ptr<CommandPool> OpenGLRenderWindow::CreateCommandPool(QueueType queueType)
	{
		return {};
	}

	const OpenGLFramebuffer& OpenGLRenderWindow::GetFramebuffer() const
	{
		return m_framebuffer;
	}

	const OpenGLRenderPass& OpenGLRenderWindow::GetRenderPass() const
	{
		return m_renderPass;
	}

	std::shared_ptr<RenderDevice> OpenGLRenderWindow::GetRenderDevice()
	{
		return m_device;
	}

	void OpenGLRenderWindow::Present()
	{
		m_context->SwapBuffers();
		m_currentFrame = (m_currentFrame + 1) % m_renderImage.size();
	}
}

