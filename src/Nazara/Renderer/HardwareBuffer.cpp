// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/HardwareBuffer.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Renderer/Context.hpp>
#include <Nazara/Renderer/OpenGL.hpp>
#include <cstring>
#include <stdexcept>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	HardwareBuffer::HardwareBuffer(Buffer* parent, BufferType type) :
	m_type(type),
	m_parent(parent)
	{
	}

	HardwareBuffer::~HardwareBuffer() = default;

	bool HardwareBuffer::Create(unsigned int size, BufferUsage usage)
	{
		Context::EnsureContext();

		m_buffer = 0;
		glGenBuffers(1, &m_buffer);

		OpenGL::BindBuffer(m_type, m_buffer);

		glBufferData(OpenGL::BufferTarget[m_type], size, nullptr, OpenGL::BufferUsage[usage]);

		return true;
	}

	void HardwareBuffer::Destroy()
	{
		Context::EnsureContext();

		OpenGL::DeleteBuffer(m_type, m_buffer);
	}

	bool HardwareBuffer::Fill(const void* data, unsigned int offset, unsigned int size, bool forceDiscard)
	{
		Context::EnsureContext();

		unsigned int totalSize = m_parent->GetSize();

		if (!forceDiscard)
			forceDiscard = (size == totalSize);

		OpenGL::BindBuffer(m_type, m_buffer);

		// Il semblerait que glBuffer(Sub)Data soit plus performant que glMapBuffer(Range) en dessous d'un certain seuil
		// http://www.stevestreeting.com/2007/03/17/glmapbuffer-vs-glbuffersubdata-the-return/
		if (size < 32*1024)
		{
			// http://www.opengl.org/wiki/Buffer_Object_Streaming
			if (forceDiscard)
				glBufferData(OpenGL::BufferTarget[m_type], totalSize, nullptr, OpenGL::BufferUsage[m_parent->GetUsage()]); // Discard

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

	bool HardwareBuffer::IsHardware() const
	{
		return true;
	}

	void* HardwareBuffer::Map(BufferAccess access, unsigned int offset, unsigned int size)
	{
		Context::EnsureContext();

		OpenGL::BindBuffer(m_type, m_buffer);

		if (glMapBufferRange)
			return glMapBufferRange(OpenGL::BufferTarget[m_type], offset, size, OpenGL::BufferLockRange[access]);
		else
		{
			// http://www.opengl.org/wiki/Buffer_Object_Streaming
			if (access == BufferAccess_DiscardAndWrite)
				glBufferData(OpenGL::BufferTarget[m_type], m_parent->GetSize(), nullptr, OpenGL::BufferUsage[m_parent->GetUsage()]); // Discard

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
			glBufferData(OpenGL::BufferTarget[m_type], m_parent->GetSize(), nullptr, OpenGL::BufferUsage[m_parent->GetUsage()]);

			// Une erreur rare est survenue, nous devons réinitialiser le buffer
			NazaraError("Failed to unmap buffer, reinitialising content... (OpenGL error : 0x" + String::Number(glGetError(), 16) + ')');
			return false;
		}

		return true;
	}

	void HardwareBuffer::Bind() const
	{
		OpenGL::BindBuffer(m_type, m_buffer);
	}

	unsigned int HardwareBuffer::GetOpenGLID() const
	{
		return m_buffer;
}
}
