// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLBuffer.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	inline OpenGLBuffer::OpenGLBuffer(Vk::Device& device, BufferType type) :
	m_device(device),
	m_type(type)
	{
	}

	inline VkBuffer OpenGLBuffer::GetBuffer()
	{
		return m_buffer;
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
