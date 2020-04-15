// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLRenderPass.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	inline OpenGLRenderPass::OpenGLRenderPass(Vk::RenderPass renderPass, std::initializer_list<PixelFormat> formats) :
	m_formats(std::begin(formats), std::end(formats)),
	m_renderPass(std::move(renderPass))
	{
	}

	inline PixelFormat OpenGLRenderPass::GetAttachmentFormat(std::size_t attachmentIndex) const
	{
		return m_formats[attachmentIndex];
	}

	inline Vk::RenderPass& OpenGLRenderPass::GetRenderPass()
	{
		return m_renderPass;
	}

	inline const Vk::RenderPass& OpenGLRenderPass::GetRenderPass() const
	{
		return m_renderPass;
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
