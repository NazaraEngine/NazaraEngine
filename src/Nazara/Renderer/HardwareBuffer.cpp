// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
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
	GLenum bufferLock[] = {
		GL_WRITE_ONLY, // nzBufferAccess_DiscardAndWrite
		GL_READ_ONLY,  // nzBufferAccess_ReadOnly
		GL_READ_WRITE, // nzBufferAccess_ReadWrite
		GL_WRITE_ONLY  // nzBufferAccess_WriteOnly
	};

	GLenum bufferLockRange[] = {
		GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_WRITE_BIT, // nzBufferAccess_DiscardAndWrite
		GL_MAP_READ_BIT,                                 // nzBufferAccess_ReadOnly
		GL_MAP_READ_BIT | GL_MAP_WRITE_BIT,              // nzBufferAccess_ReadWrite
		GL_MAP_WRITE_BIT                                 // nzBufferAccess_WriteOnly
	};

	GLenum bufferTarget[] = {
		GL_ELEMENT_ARRAY_BUFFER, // nzBufferType_Index,
		GL_ARRAY_BUFFER,		 // nzBufferType_Vertex
	};

	GLenum bufferTargetBinding[] = {
		GL_ELEMENT_ARRAY_BUFFER_BINDING, // nzBufferType_Index,
		GL_ARRAY_BUFFER_BINDING,		 // nzBufferType_Vertex
	};

	GLenum bufferUsage[] = {
		// J'ai choisi DYNAMIC à la place de STREAM car DYNAMIC semble plus adapté au profil "une mise à jour pour quelques rendus"
		// Ce qui est je pense le scénario qui arrivera le plus souvent (Prévoir une option pour permettre d'utiliser le STREAM_DRAW ?)
		// Source: http://www.opengl.org/discussion_boards/ubbthreads.php?ubb=showflat&Number=160839
		GL_DYNAMIC_DRAW, // nzBufferUsage_Dynamic
		GL_STATIC_DRAW  // nzBufferUsage_Static
	};

	typedef nzUInt8* (*LockRoutine)(nzBufferType type, nzBufferAccess access, unsigned int offset, unsigned int size);

	nzUInt8* LockBuffer(nzBufferType type, nzBufferAccess access, unsigned int offset, unsigned int size)
	{
		NazaraUnused(size);

		if (access == nzBufferAccess_DiscardAndWrite)
		{
			GLint bufSize;
			glGetBufferParameteriv(bufferTargetBinding[type], GL_BUFFER_SIZE, &bufSize);

			GLint bufUsage;
			glGetBufferParameteriv(bufferTargetBinding[type], GL_BUFFER_USAGE, &bufUsage);

			// On discard le buffer
			glBufferData(bufferTargetBinding[type], bufSize, nullptr, bufUsage);
		}

		void* ptr = glMapBuffer(bufferTarget[type], bufferLock[access]);
		if (ptr)
			return reinterpret_cast<nzUInt8*>(ptr) + offset;
		else
			return nullptr;
	}

	nzUInt8* LockBufferRange(nzBufferType type, nzBufferAccess access, unsigned int offset, unsigned int size)
	{
		return reinterpret_cast<nzUInt8*>(glMapBufferRange(bufferTarget[type], offset, size, bufferLockRange[access]));
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

	glBindBuffer(bufferTarget[m_type], m_buffer);
}

bool NzHardwareBuffer::Create(unsigned int size, nzBufferUsage usage)
{
	NzContext::EnsureContext();

	m_buffer = 0;
	glGenBuffers(1, &m_buffer);

	GLint previous;
	glGetIntegerv(bufferTargetBinding[m_type], &previous);

	glBindBuffer(bufferTarget[m_type], m_buffer);
	glBufferData(bufferTarget[m_type], size, nullptr, bufferUsage[usage]);

	// Pour ne pas perturber le rendu, on interfère pas avec le binding déjà présent
	if (previous != 0)
		glBindBuffer(bufferTarget[m_type], previous);

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
	glGetIntegerv(bufferTargetBinding[m_type], reinterpret_cast<GLint*>(&previous));

	if (previous != m_buffer)
		glBindBuffer(bufferTarget[m_type], m_buffer);

	// Il semblerait que glBuffer(Sub)Data soit plus performant que glMapBuffer(Range) en dessous d'un certain seuil
	// http://www.stevestreeting.com/2007/03/17/glmapbuffer-vs-glbuffersubdata-the-return/
	if (size < 32*1024)
	{
		// http://www.opengl.org/wiki/Vertex_Specification_Best_Practices
		if (size == m_parent->GetSize())
			glBufferData(bufferTarget[m_type], m_parent->GetSize(), nullptr, bufferUsage[m_parent->GetUsage()]); // Discard

		glBufferSubData(bufferTarget[m_type], offset, size, data);
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

		if (glUnmapBuffer(bufferTarget[m_type]) != GL_TRUE)
		{
			// Une erreur rare est survenue, nous devons réinitialiser le buffer
			NazaraError("Failed to unmap buffer, reinitialising content... (OpenGL error : 0x" + NzString::Number(glGetError(), 16) + ')');

			glBufferData(bufferTarget[m_type], m_parent->GetSize(), nullptr, bufferUsage[m_parent->GetStorage()]);

			return false;
		}
	}

	// Inutile de rebinder s'il n'y avait aucun buffer (Optimise les opérations chaînées)
	if (previous != m_buffer && previous != 0)
		glBindBuffer(bufferTarget[m_type], previous);

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

	// Pour ne pas perturber le rendu, on interfère pas avec le binding déjà présent
	GLuint previous;
	glGetIntegerv(bufferTargetBinding[m_type], reinterpret_cast<GLint*>(&previous));

	if (previous != m_buffer)
		glBindBuffer(bufferTarget[m_type], m_buffer);

	void* ptr = mapBuffer(m_type, access, offset, size);

	// Inutile de rebinder s'il n'y avait aucun buffer (Optimise les opérrations chaînées)
	if (previous != m_buffer && previous != 0)
		glBindBuffer(bufferTarget[m_type], previous);

	return ptr;
}

bool NzHardwareBuffer::Unmap()
{
	NzContext::EnsureContext();

	GLuint previous;
	glGetIntegerv(bufferTargetBinding[m_type], reinterpret_cast<GLint*>(&previous));

	if (previous != m_buffer)
		glBindBuffer(bufferTarget[m_type], m_buffer);

	if (glUnmapBuffer(bufferTarget[m_type]) != GL_TRUE)
	{
		// Une erreur rare est survenue, nous devons réinitialiser le buffer
		NazaraError("Failed to unmap buffer, reinitialising content... (OpenGL error : 0x" + NzString::Number(glGetError(), 16) + ')');

		glBufferData(bufferTarget[m_type], m_parent->GetSize(), nullptr, bufferUsage[m_parent->GetStorage()]);

		// Inutile de rebinder s'il n'y avait aucun buffer (Optimise les opérations chaînées)
		if (previous != m_buffer && previous != 0)
			glBindBuffer(bufferTarget[m_type], previous);

		return false;
	}

	// Inutile de rebinder s'il n'y avait aucun buffer (Optimise les opérations chaînées)
	if (previous != m_buffer && previous != 0)
		glBindBuffer(bufferTarget[m_type], previous);

	return true;
}
