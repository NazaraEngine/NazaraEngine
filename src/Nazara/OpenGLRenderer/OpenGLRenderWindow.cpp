// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLRenderWindow.hpp>
#include <Nazara/OpenGLRenderer/DummySurface.hpp>
#include <Nazara/OpenGLRenderer/OpenGLCommandPool.hpp>
#include <Nazara/OpenGLRenderer/OpenGLRenderer.hpp>
#include <Nazara/Renderer/CommandPool.hpp>
#include <Nazara/Renderer/RenderWindow.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	OpenGLRenderWindow::OpenGLRenderWindow(RenderWindow& owner) :
	m_currentFrame(0),
	m_renderPass({}, {}, {}),
	m_framebuffer(*this),
	m_owner(owner)
	{
	}

	RenderFrame OpenGLRenderWindow::Acquire()
	{
		if (m_owner.IsMinimized())
			return RenderFrame();

		bool invalidateFramebuffer = false;
		Vector2ui size = m_owner.GetSize();
		if (m_size != size)
		{
			invalidateFramebuffer = true;
			m_size = size;
		}

		return RenderFrame(m_renderImage[m_currentFrame].get(), invalidateFramebuffer);
	}

	bool OpenGLRenderWindow::Create(RendererImpl* renderer, RenderSurface* surface, const RenderWindowParameters& parameters)
	{
		DummySurface* dummySurface = static_cast<DummySurface*>(surface);
		OpenGLRenderer* glRenderer = static_cast<OpenGLRenderer*>(renderer);

		OpenGLDevice& device = static_cast<OpenGLDevice&>(*m_owner.GetRenderDevice());

		GL::ContextParams contextParams;

		m_context = device.CreateContext(contextParams, dummySurface->GetWindowHandle());
		if (!m_context)
			return false;

		m_size = m_owner.GetSize();

		constexpr std::size_t RenderImageCount = 2;

		m_renderImage.reserve(RenderImageCount);
		for (std::size_t i = 0; i < RenderImageCount; ++i)
			m_renderImage.emplace_back(std::make_unique<OpenGLRenderImage>(*this));

		return true;
	}

	std::shared_ptr<CommandPool> OpenGLRenderWindow::CreateCommandPool(QueueType /*queueType*/)
	{
		return std::make_unique<OpenGLCommandPool>();
	}

	const OpenGLFramebuffer& OpenGLRenderWindow::GetFramebuffer() const
	{
		return m_framebuffer;
	}

	const OpenGLRenderPass& OpenGLRenderWindow::GetRenderPass() const
	{
		return m_renderPass;
	}

	void OpenGLRenderWindow::Present()
	{
		m_context->SwapBuffers();
		m_currentFrame = (m_currentFrame + 1) % m_renderImage.size();
	}
}

