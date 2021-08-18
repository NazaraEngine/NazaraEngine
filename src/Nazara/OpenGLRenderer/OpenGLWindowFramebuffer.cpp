// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLWindowFramebuffer.hpp>
#include <Nazara/OpenGLRenderer/OpenGLRenderWindow.hpp>
#include <stdexcept>
#include <Nazara/OpenGLRenderer/Debug.hpp>

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
}
