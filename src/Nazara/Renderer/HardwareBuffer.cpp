// Copyright (C) 2014 Jérôme Leclercq
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

namespace
{
	using LockRoutine = nzUInt8* (*)(nzBufferType type, nzBufferAccess access, unsigned int offset, unsigned int size);

	nzUInt8* LockBuffer(nzBufferType type, nzBufferAccess access, unsigned int offset, unsigned int size)
	{
		NazaraUnused(size);

		if (access == nzBufferAccess_DiscardAndWrite)
		{
			GLint bufSize;
			glGetBufferParameteriv(NzOpenGL::BufferTargetBinding[type], GL_BUFFER_SIZE, &bufSize);

			GLint bufUsage;
			glGetBufferParameteriv(NzOpenGL::BufferTargetBinding[type], GL_BUFFER_USAGE, &bufUsage);

			// On discard le buffer
			glBufferData(NzOpenGL::BufferTargetBinding[type], bufSize, nullptr, bufUsage);
		}

		void* ptr = glMapBuffer(NzOpenGL::BufferTarget[type], NzOpenGL::BufferLock[access]);
		if (ptr)
			return reinterpret_cast<nzUInt8*>(ptr) + offset;
		else
			return nullptr;
	}

	nzUInt8* LockBufferRange(nzBufferType type, nzBufferAccess access, unsigned int offset, unsigned int size)
	{
		return reinterpret_cast<nzUInt8*>(glMapBufferRange(NzOpenGL::BufferTarget[type], offset, size, NzOpenGL::BufferLockRange[access]));
	}

	nzUInt8* LockBufferFirstRun(nzBufferType type, nzBufferAccess access, unsigned int offset, unsigned int size);

	LockRoutine mapBuffer = LockBufferFirstRun;

	nzUInt8* LockBufferFirstRun(nzBufferType type, nzBufferAccess access, unsigned int offset, unsigned int size)
	{
		if (glMapBufferRange)
			mapBuffer = LockBufferRange;
		else
			mapBuffer = LockBuffer;

		return mapBuffer(type, access, offset, size);
	}
}

NzHardwareBuffer::NzHardwareBuffer(NzBuffer* parent, nzBufferType type) :
m_type(type),
m_parent(parent)
{
}

NzHardwareBuffer::~NzHardwareBuffer() = default;

bool NzHardwareBuffer::Create(unsigned int size, nzBufferUsage usage)
{
	NzContext::EnsureContext();

	m_buffer = 0;
	glGenBuffers(1, &m_buffer);

	NzOpenGL::BindBuffer(m_type, m_buffer);

	glBufferData(NzOpenGL::BufferTarget[m_type], size, nullptr, NzOpenGL::BufferUsage[usage]);

	return true;
}

void NzHardwareBuffer::Destroy()
{
	NzContext::EnsureContext();

	NzOpenGL::DeleteBuffer(m_type, m_buffer);
}

bool NzHardwareBuffer::Fill(const void* data, unsigned int offset, unsigned int size, bool forceDiscard)
{
	NzContext::EnsureContext();

	unsigned int totalSize = m_parent->GetSize();

	if (!forceDiscard)
		forceDiscard = (size == totalSize);

	NzOpenGL::BindBuffer(m_type, m_buffer);

	// http://www.opengl.org/wiki/Vertex_Specification_Best_Practices
	if (forceDiscard)
		glBufferData(NzOpenGL::BufferTarget[m_type], totalSize, nullptr, NzOpenGL::BufferUsage[m_parent->GetUsage()]); // Discard

	// Il semblerait que glBuffer(Sub)Data soit plus performant que glMapBuffer(Range) en dessous d'un certain seuil
	// http://www.stevestreeting.com/2007/03/17/glmapbuffer-vs-glbuffersubdata-the-return/
	if (size < 32*1024)
		glBufferSubData(NzOpenGL::BufferTarget[m_type], offset, size, data);
	else
	{
		nzUInt8* ptr = mapBuffer(m_type, (forceDiscard) ? nzBufferAccess_DiscardAndWrite : nzBufferAccess_WriteOnly, offset, size);
		if (!ptr)
		{
			NazaraError("Failed to map buffer");
			return false;
		}

		std::memcpy(ptr, data, size);

		if (glUnmapBuffer(NzOpenGL::BufferTarget[m_type]) != GL_TRUE)
		{
			// Une erreur rare est survenue, nous devons réinitialiser le buffer
			NazaraError("Failed to unmap buffer, reinitialising content... (OpenGL error : 0x" + NzString::Number(glGetError(), 16) + ')');

			glBufferData(NzOpenGL::BufferTarget[m_type], totalSize, nullptr, NzOpenGL::BufferUsage[m_parent->GetUsage()]);

			return false;
		}
	}

	return true;
}

bool NzHardwareBuffer::IsHardware() const
{
	return true;
}

void* NzHardwareBuffer::Map(nzBufferAccess access, unsigned int offset, unsigned int size)
{
	NzContext::EnsureContext();

	NzOpenGL::BindBuffer(m_type, m_buffer);

	if (access == nzBufferAccess_DiscardAndWrite)
		glBufferData(NzOpenGL::BufferTarget[m_type], m_parent->GetSize(), nullptr, NzOpenGL::BufferUsage[m_parent->GetUsage()]); // Discard

	return mapBuffer(m_type, access, offset, size);
}

bool NzHardwareBuffer::Unmap()
{
	NzContext::EnsureContext();

	NzOpenGL::BindBuffer(m_type, m_buffer);

	if (glUnmapBuffer(NzOpenGL::BufferTarget[m_type]) != GL_TRUE)
	{
		glBufferData(NzOpenGL::BufferTarget[m_type], m_parent->GetSize(), nullptr, NzOpenGL::BufferUsage[m_parent->GetUsage()]);

		// Une erreur rare est survenue, nous devons réinitialiser le buffer
		NazaraError("Failed to unmap buffer, reinitialising content... (OpenGL error : 0x" + NzString::Number(glGetError(), 16) + ')');
		return false;
	}

	return true;
}

void NzHardwareBuffer::Bind() const
{
	NzOpenGL::BindBuffer(m_type, m_buffer);
}

unsigned int NzHardwareBuffer::GetOpenGLID() const
{
	return m_buffer;
}
