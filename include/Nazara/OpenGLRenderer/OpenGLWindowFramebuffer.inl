// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLWindowFramebuffer.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	inline OpenGLWindowFramebuffer::OpenGLWindowFramebuffer(OpenGLRenderWindow& renderWindow) :
	OpenGLFramebuffer(FramebufferType::Window),
	m_renderWindow(renderWindow)
	{
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
