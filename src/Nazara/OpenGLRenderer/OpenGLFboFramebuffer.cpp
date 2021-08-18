// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLFboFramebuffer.hpp>
#include <Nazara/OpenGLRenderer/OpenGLRenderPass.hpp>
#include <Nazara/OpenGLRenderer/OpenGLTexture.hpp>
#include <stdexcept>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	OpenGLFboFramebuffer::OpenGLFboFramebuffer(OpenGLDevice& device, const std::vector<std::shared_ptr<Texture>>& attachments) :
	OpenGLFramebuffer(FramebufferType::Texture)
	{
		if (!m_framebuffer.Create(device))
			throw std::runtime_error("failed to create framebuffer object");

		std::size_t colorAttachmentCount = 0;
		bool hasDepth = false; 
		bool hasStencil = false;

		for (std::size_t i = 0; i < attachments.size(); ++i)
		{
			assert(attachments[i]);
			const OpenGLTexture& glTexture = static_cast<const OpenGLTexture&>(*attachments[i]);

			PixelFormat textureFormat = glTexture.GetFormat();

			GLenum attachment;
			switch (PixelFormatInfo::GetContent(textureFormat))
			{
				case PixelFormatContent::ColorRGBA:
					attachment = static_cast<GLenum>(GL_COLOR_ATTACHMENT0 + colorAttachmentCount);
					colorAttachmentCount++;
					break;

				case PixelFormatContent::Depth:
					if (hasDepth)
						throw std::runtime_error("a framebuffer can only have one depth attachment");

					attachment = GL_DEPTH_ATTACHMENT;
					hasDepth = true;
					break;

				case PixelFormatContent::DepthStencil:
					if (hasDepth)
						throw std::runtime_error("a framebuffer can only have one depth attachment");

					if (hasStencil)
						throw std::runtime_error("a framebuffer can only have one stencil attachment");

					attachment = GL_DEPTH_STENCIL_ATTACHMENT;
					hasDepth = true;
					hasStencil = true;
					break;

				case PixelFormatContent::Stencil:
					if (hasStencil)
						throw std::runtime_error("a framebuffer can only have one stencil attachment");

					attachment = GL_STENCIL_ATTACHMENT;
					hasStencil = true;
					break;

				case PixelFormatContent::Undefined:
				default:
					throw std::runtime_error("unhandled pixel format " + PixelFormatInfo::GetName(textureFormat));
			}

			m_framebuffer.Texture2D(attachment, ToOpenGL(OpenGLTexture::ToTextureTarget(glTexture.GetType())), glTexture.GetTexture().GetObjectId());
		}

		GLenum status = m_framebuffer.Check();
		if (status != GL_FRAMEBUFFER_COMPLETE)
			throw std::runtime_error("invalid framebuffer: 0x" + NumberToString(status, 16));

		m_colorAttachmentCount = colorAttachmentCount;
	}

	void OpenGLFboFramebuffer::Activate() const
	{
		const GL::Context& context = m_framebuffer.EnsureDeviceContext();

		context.BindFramebuffer(GL::FramebufferTarget::Draw, m_framebuffer.GetObjectId());
	}

	std::size_t OpenGLFboFramebuffer::GetColorBufferCount() const
	{
		return m_colorAttachmentCount;
	}
}
