// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLFramebuffer.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	inline OpenGLFramebuffer::OpenGLFramebuffer(Type type) :
	m_type(type)
	{
	}

	inline auto OpenGLFramebuffer::GetType() const -> Type
	{
		return m_type;
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
