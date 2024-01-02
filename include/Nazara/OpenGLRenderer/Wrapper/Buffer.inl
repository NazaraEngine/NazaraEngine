// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz::GL
{
	inline void* Buffer::MapRange(GLintptr offset, GLsizeiptr length, GLbitfield access)
	{
		const Context& context = EnsureDeviceContext();
		context.BindBuffer(m_target, m_objectId);
		return context.glMapBufferRange(ToOpenGL(m_target), offset, length, access);
	}

	inline void Buffer::Reset(BufferTarget target, GLsizeiptr size, const void* initialData, GLenum usage)
	{
		m_target = target;

		const Context& context = EnsureDeviceContext();

		context.BindBuffer(m_target, m_objectId);

		context.glBufferData(ToOpenGL(m_target), size, initialData, usage);
	}

	inline void Buffer::SubData(GLintptr offset, GLsizeiptr size, const void* data)
	{
		const Context& context = EnsureDeviceContext();
		context.BindBuffer(m_target, m_objectId);

		context.glBufferSubData(ToOpenGL(m_target), offset, size, data);
	}

	inline bool Buffer::Unmap()
	{
		const Context& context = EnsureDeviceContext();
		context.BindBuffer(m_target, m_objectId);
		return context.glUnmapBuffer(ToOpenGL(m_target)) == GL_TRUE;
	}

	inline GLuint Buffer::CreateHelper(OpenGLDevice& /*device*/, const Context& context)
	{
		GLuint sampler = 0;
		context.glGenBuffers(1U, &sampler);

		return sampler;
	}

	inline void Buffer::DestroyHelper(OpenGLDevice& device, const Context& context, GLuint objectId)
	{
		context.glDeleteBuffers(1U, &objectId);

		device.NotifyBufferDestruction(objectId);
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
