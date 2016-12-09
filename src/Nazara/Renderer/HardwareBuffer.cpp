// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/HardwareBuffer.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Renderer/Context.hpp>
#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Utility/Buffer.hpp>
#include <cstring>
#include <stdexcept>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	HardwareBuffer::HardwareBuffer(Buffer* parent, BufferType type) :
	m_buffer(0),
	m_type(type),
	m_parent(parent)
	{
	}

	HardwareBuffer::~HardwareBuffer()
	{
		if (m_buffer)
		{
			Context::EnsureContext();

			OpenGL::DeleteBuffer(m_type, m_buffer);
		}
	}

	bool HardwareBuffer::Initialize(UInt32 size, BufferUsageFlags usage)
	{
		Context::EnsureContext();

		m_buffer = 0;
		glGenBuffers(1, &m_buffer);

		OpenGL::BindBuffer(m_type, m_buffer);
		glBufferData(OpenGL::BufferTarget[m_type], size, nullptr, (usage & BufferUsage_Dynamic) ? GL_STREAM_DRAW : GL_STATIC_DRAW);

		return true;
	}

	bool HardwareBuffer::Fill(const void* data, UInt32 offset, UInt32 size)
	{
		Context::EnsureContext();

		UInt32 totalSize = m_parent->GetSize();

		bool forceDiscard = (size == totalSize);

		OpenGL::BindBuffer(m_type, m_buffer);

		// It seems glBuffer(Sub)Data performs faster than glMapBuffer under a specific range
		// http://www.stevestreeting.com/2007/03/17/glmapbuffer-vs-glbuffersubdata-the-return/
		if (size < 32*1024)
		{
			// http://www.opengl.org/wiki/Buffer_Object_Streaming
			if (forceDiscard)
				glBufferData(OpenGL::BufferTarget[m_type], totalSize, nullptr, (m_parent->GetUsage() & BufferUsage_Dynamic) ? GL_STREAM_DRAW : GL_STATIC_DRAW); // Discard

			glBufferSubData(OpenGL::BufferTarget[m_type], offset, size, data);
		}
		else
		{
			void* ptr = Map((forceDiscard) ? BufferAccess_DiscardAndWrite : BufferAccess_WriteOnly, offset, size);
			if (!ptr)
			{
				NazaraError("Failed to map buffer");
				return false;
			}

			std::memcpy(ptr, data, size);

			Unmap();
		}

		return true;
	}

	DataStorage HardwareBuffer::GetStorage() const
	{
		return DataStorage_Hardware;
	}

	void* HardwareBuffer::Map(BufferAccess access, UInt32 offset, UInt32 size)
	{
		Context::EnsureContext();

		OpenGL::BindBuffer(m_type, m_buffer);

		if (glMapBufferRange)
			return glMapBufferRange(OpenGL::BufferTarget[m_type], offset, size, OpenGL::BufferLockRange[access]);
		else
		{
			// http://www.opengl.org/wiki/Buffer_Object_Streaming
			if (access == BufferAccess_DiscardAndWrite)
				glBufferData(OpenGL::BufferTarget[m_type], m_parent->GetSize(), nullptr, (m_parent->GetUsage() & BufferUsage_Dynamic) ? GL_STREAM_DRAW : GL_STATIC_DRAW); // Discard

			UInt8* ptr = static_cast<UInt8*>(glMapBuffer(OpenGL::BufferTarget[m_type], OpenGL::BufferLock[access]));
			if (ptr)
				ptr += offset;

			return ptr;
		}
	}

	bool HardwareBuffer::Unmap()
	{
		Context::EnsureContext();

		OpenGL::BindBuffer(m_type, m_buffer);

		if (glUnmapBuffer(OpenGL::BufferTarget[m_type]) != GL_TRUE)
		{
			// An error occured, we have to reset the buffer
			glBufferData(OpenGL::BufferTarget[m_type], m_parent->GetSize(), nullptr, (m_parent->GetUsage() & BufferUsage_Dynamic) ? GL_STREAM_DRAW : GL_STATIC_DRAW);

			NazaraError("Failed to unmap buffer, reinitialising content... (OpenGL error: 0x" + String::Number(glGetError(), 16) + ')');
			return false;
		}

		return true;
	}

	void HardwareBuffer::Bind() const
	{
		OpenGL::BindBuffer(m_type, m_buffer);
	}

	GLuint HardwareBuffer::GetOpenGLID() const
	{
		return m_buffer;
	}
}
