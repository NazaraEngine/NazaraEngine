// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/OpenGLRenderer/OpenGLWindowFramebuffer.hpp>
#include <Nazara/OpenGLRenderer/OpenGLSwapchain.hpp>
#include <stdexcept>

namespace Nz
{
	void OpenGLWindowFramebuffer::Activate() const
	{
		GL::Context& context = m_renderWindow.GetContext();
		if (!GL::Context::SetCurrentContext(&context))
			throw std::runtime_error("failed to bind window context");

		context.BindFramebuffer(GL::FramebufferTarget::Draw, 0);
	}

	std::size_t OpenGLWindowFramebuffer::GetColorBufferCount() const
	{
		return 1;
	}

	const Vector2ui& OpenGLWindowFramebuffer::GetSize() const
	{
		return m_renderWindow.GetSize();
	}

	void OpenGLWindowFramebuffer::UpdateDebugName(std::string_view /*name*/)
	{
		// No OpenGL object to name
	}
}
