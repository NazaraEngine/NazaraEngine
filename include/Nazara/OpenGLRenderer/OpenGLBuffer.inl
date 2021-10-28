// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
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
