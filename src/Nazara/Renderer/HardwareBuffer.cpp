// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/HardwareBuffer.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Renderer/Context.hpp>
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

NzHardwareBuffer::~NzHardwareBuffer()
{
}

void NzHardwareBuffer::Bind()
{
	#ifdef NAZARA_DEBUG
	if (NzContext::GetCurrent() == nullptr)
	{
		NazaraError("No active context");
		return;
	}
	#endif

	glBindBuffer(NzOpenGL::BufferTarget[m_type], m_buffer);
}

bool NzHardwareBuffer::Create(unsigned int size, nzBufferUsage usage)
{
	NzContext::EnsureContext();

	m_buffer = 0;
	glGenBuffers(1, &m_buffer);

	GLint previous;
	glGetIntegerv(NzOpenGL::BufferTargetBinding[m_type], &previous);

	glBindBuffer(NzOpenGL::BufferTarget[m_type], m_buffer);
	glBufferData(NzOpenGL::BufferTarget[m_type], size, nullptr, NzOpenGL::BufferUsage[usage]);

	// Pour ne pas perturber le rendu, on n'interfère pas avec le binding déjà présent
	if (previous != 0)
		glBindBuffer(NzOpenGL::BufferTarget[m_type], previous);

	return true;
}

void NzHardwareBuffer::Destroy()
{
	NzContext::EnsureContext();

	glDeleteBuffers(1, &m_buffer);
}

bool NzHardwareBuffer::Fill(const void* data, unsigned int offset, unsigned int size)
{
	NzContext::EnsureContext();

	GLuint previous;
	glGetIntegerv(NzOpenGL::BufferTargetBinding[m_type], reinterpret_cast<GLint*>(&previous));

	if (previous != m_buffer)
		glBindBuffer(NzOpenGL::BufferTarget[m_type], m_buffer);

	// Il semblerait que glBuffer(Sub)Data soit plus performant que glMapBuffer(Range) en dessous d'un certain seuil
	// http://www.stevestreeting.com/2007/03/17/glmapbuffer-vs-glbuffersubdata-the-return/
	if (size < 32*1024)
	{
		// http://www.opengl.org/wiki/Vertex_Specification_Best_Practices
		if (size == m_parent->GetSize())
			glBufferData(NzOpenGL::BufferTarget[m_type], m_parent->GetSize(), nullptr, NzOpenGL::BufferUsage[m_parent->GetUsage()]); // Discard

		glBufferSubData(NzOpenGL::BufferTarget[m_type], offset, size, data);
	}
	else
	{
		nzUInt8* ptr = mapBuffer(m_type, (size == m_parent->GetSize()) ? nzBufferAccess_DiscardAndWrite : nzBufferAccess_WriteOnly, offset, size);
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

			glBufferData(NzOpenGL::BufferTarget[m_type], m_parent->GetSize(), nullptr, NzOpenGL::BufferUsage[m_parent->GetStorage()]);

			return false;
		}
	}

	// Inutile de rebinder s'il n'y avait aucun buffer (Optimise les opérations chaînées)
	if (previous != m_buffer && previous != 0)
		glBindBuffer(NzOpenGL::BufferTarget[m_type], previous);

	return true;
}

void* NzHardwareBuffer::GetPointer()
{
	return nullptr;
}

bool NzHardwareBuffer::IsHardware() const
{
	return true;
}

void* NzHardwareBuffer::Map(nzBufferAccess access, unsigned int offset, unsigned int size)
{
	NzContext::EnsureContext();

	// Pour ne pas perturber le rendu, on n'interfère pas avec le binding déjà présent
	GLuint previous;
	glGetIntegerv(NzOpenGL::BufferTargetBinding[m_type], reinterpret_cast<GLint*>(&previous));

	if (previous != m_buffer)
		glBindBuffer(NzOpenGL::BufferTarget[m_type], m_buffer);

	void* ptr = mapBuffer(m_type, access, offset, size);

	// Inutile de rebinder s'il n'y avait aucun buffer (Optimise les opérrations chaînées)
	if (previous != m_buffer && previous != 0)
		glBindBuffer(NzOpenGL::BufferTarget[m_type], previous);

	return ptr;
}

bool NzHardwareBuffer::Unmap()
{
	NzContext::EnsureContext();

	GLuint previous;
	glGetIntegerv(NzOpenGL::BufferTargetBinding[m_type], reinterpret_cast<GLint*>(&previous));

	if (previous != m_buffer)
		glBindBuffer(NzOpenGL::BufferTarget[m_type], m_buffer);

	if (glUnmapBuffer(NzOpenGL::BufferTarget[m_type]) != GL_TRUE)
	{
		// Une erreur rare est survenue, nous devons réinitialiser le buffer
		NazaraError("Failed to unmap buffer, reinitialising content... (OpenGL error : 0x" + NzString::Number(glGetError(), 16) + ')');

		glBufferData(NzOpenGL::BufferTarget[m_type], m_parent->GetSize(), nullptr, NzOpenGL::BufferUsage[m_parent->GetStorage()]);

		// Inutile de rebinder s'il n'y avait aucun buffer (Optimise les opérations chaînées)
		if (previous != m_buffer && previous != 0)
			glBindBuffer(NzOpenGL::BufferTarget[m_type], previous);

		return false;
	}

	// Inutile de rebinder s'il n'y avait aucun buffer (Optimise les opérations chaînées)
	if (previous != m_buffer && previous != 0)
		glBindBuffer(NzOpenGL::BufferTarget[m_type], previous);

	return true;
}
