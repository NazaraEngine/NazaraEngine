// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLBuffer.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	inline const GL::Buffer& OpenGLBuffer::GetBuffer() const
	{
		return m_buffer;
	}

	inline BufferType OpenGLBuffer::GetType() const
	{
		return m_type;
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
