// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLSwapchain.hpp>
#include <Nazara/OpenGLRenderer/OpenGLCommandPool.hpp>
#include <Nazara/OpenGLRenderer/OpenGLRenderer.hpp>
#include <Nazara/Renderer/CommandPool.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	OpenGLSwapchain::OpenGLSwapchain(OpenGLDevice& device, WindowHandle windowHandle, const Vector2ui& windowSize, const SwapchainParameters& parameters) :
	m_device(device),
	m_framebuffer(*this),
	m_currentImageIndex(0),
	m_size(windowSize),
	m_sizeInvalidated(false)
	{
		GL::ContextParams contextParams;
#ifdef NAZARA_OPENGLRENDERER_DEBUG
		contextParams.wrapErrorHandling = true;
#endif
		//TODO: Pass swapchain parameters to context

		m_context = m_device.CreateContext(contextParams, windowHandle);
		if (!m_context)
			throw std::runtime_error("failed to create swapchain context");

		// TODO: extract the exact window pixel format
		PixelFormat colorFormat;
		switch (contextParams.bitsPerPixel)
		{
			case 8:  colorFormat = PixelFormat::R8; break;
			case 16: colorFormat = PixelFormat::RG8; break;
			case 24: colorFormat = PixelFormat::RGB8; break;

			case 32:
			default:
				colorFormat = PixelFormat::RGBA8;
				break;
		}

		// TODO: extract the exact depth-stencil format
		PixelFormat depthFormat;
		if (contextParams.stencilBits > 0)
			depthFormat = PixelFormat::Depth24Stencil8;
		else if (contextParams.depthBits > 24)
			depthFormat = PixelFormat::Depth32F;
		else if (contextParams.depthBits > 16)
			depthFormat = PixelFormat::Depth24;
		else if (contextParams.depthBits > 0)
			depthFormat = PixelFormat::Depth16;
		else
			depthFormat = PixelFormat::Undefined;

		m_supportedPresentModes = m_context->GetSupportedPresentModes();

#ifdef NAZARA_PLATFORM_WEB
		m_presentMode = PresentMode::Immediate; //< default present mode
#else
		m_presentMode = PresentMode::VerticalSync; //< default present mode
#endif

		for (PresentMode presentMode : parameters.presentMode)
		{
			if (m_supportedPresentModes & presentMode)
			{
				SetPresentMode(presentMode);
				break;
			}
		}

		std::vector<RenderPass::Attachment> attachments;
		std::vector<RenderPass::SubpassDescription> subpassDescriptions;
		std::vector<RenderPass::SubpassDependency> subpassDependencies;

		BuildRenderPass(colorFormat, depthFormat, attachments, subpassDescriptions, subpassDependencies);
		m_renderPass.emplace(std::move(attachments), std::move(subpassDescriptions), std::move(subpassDependencies));

		constexpr std::size_t RenderImageCount = 2;

		m_renderImage.reserve(RenderImageCount);
		for (std::size_t i = 0; i < RenderImageCount; ++i)
			m_renderImage.emplace_back(std::make_unique<OpenGLRenderImage>(*this));
	}

	RenderFrame OpenGLSwapchain::AcquireFrame()
	{
		bool sizeInvalidated = m_sizeInvalidated;
		m_sizeInvalidated = false;

		OpenGLRenderImage& renderImage = *m_renderImage[m_currentImageIndex];
		renderImage.Reset(m_currentImageIndex);

		return RenderFrame(&renderImage, sizeInvalidated, m_size);
	}

	std::shared_ptr<CommandPool> OpenGLSwapchain::CreateCommandPool(QueueType /*queueType*/)
	{
		return std::make_shared<OpenGLCommandPool>();
	}

	const OpenGLFramebuffer& OpenGLSwapchain::GetFramebuffer(std::size_t /*imageIndex*/) const
	{
		return m_framebuffer;
	}

	std::size_t OpenGLSwapchain::GetFramebufferCount() const
	{
		return 1;
	}

	const OpenGLRenderPass& OpenGLSwapchain::GetRenderPass() const
	{
		return *m_renderPass;
	}

	PresentMode OpenGLSwapchain::GetPresentMode() const
	{
		return m_presentMode;
	}

	const Vector2ui& OpenGLSwapchain::GetSize() const
	{
		return m_size;
	}

	PresentModeFlags OpenGLSwapchain::GetSupportedPresentModes() const
	{
		return m_supportedPresentModes;
	}

	void OpenGLSwapchain::NotifyResize(const Vector2ui& newSize)
	{
		OnSwapchainResize(this, newSize);

		m_size = newSize;
		m_sizeInvalidated = true;
	}

	void OpenGLSwapchain::Present()
	{
		m_context->SwapBuffers();
		m_currentImageIndex = (m_currentImageIndex + 1) % m_renderImage.size();
	}

	void OpenGLSwapchain::SetPresentMode(PresentMode presentMode)
	{
		NazaraAssert(m_supportedPresentModes & presentMode, "unsupported present mode");

		if (m_presentMode != presentMode)
		{
			m_context->SetPresentMode(presentMode);
			m_presentMode = presentMode;
		}
	}

	RenderResources& OpenGLSwapchain::GetTransientResources()
	{
		return *m_renderImage[m_currentImageIndex];
	}
}
