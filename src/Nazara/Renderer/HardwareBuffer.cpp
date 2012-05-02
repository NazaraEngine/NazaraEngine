// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/HardwareBuffer.hpp>
#include <Nazara/Core/Error.hpp>
#include <cstring>
#include <stdexcept>
#include <Nazara/Renderer/Debug.hpp>

namespace
{
	GLenum bufferLock[] = {
		GL_WRITE_ONLY, // nzBufferLock_DiscardAndWrite
		GL_READ_ONLY,  // nzBufferLock_ReadOnly
		GL_READ_WRITE, // nzBufferLock_ReadWrite
		GL_WRITE_ONLY  // nzBufferLock_WriteOnly
	};

	GLenum bufferLockRange[] = {
		GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_WRITE_BIT, // nzBufferLock_DiscardAndWrite
		GL_MAP_READ_BIT,								 // nzBufferLock_ReadOnly
		GL_MAP_READ_BIT | GL_MAP_WRITE_BIT,				 // nzBufferLock_ReadWrite
		GL_MAP_WRITE_BIT								 // nzBufferLock_WriteOnly
	};

	GLenum bufferTarget[] = {
		GL_ELEMENT_ARRAY_BUFFER, // nzBufferType_Index,
		GL_ARRAY_BUFFER,		 // nzBufferType_Vertex
	};

	GLenum bufferTargetBinding[] = {
		GL_ELEMENT_ARRAY_BUFFER_BINDING, // BufferType_Index,
		GL_ARRAY_BUFFER_BINDING,		 // BufferType_Vertex
	};

	GLenum bufferUsage[] = {
		// J'ai choisi DYNAMIC_DRAW à la place de STREAM_DRAW car DYNAMIC semble plus adapté au profil "une mise à jour pour quelques rendus"
		// Ce qui est je pense le scénario qui arrivera le plus souvent (Prévoir une option pour permettre d'utiliser le STREAM_DRAW ?)
		// Source: http://www.opengl.org/discussion_boards/ubbthreads.php?ubb=showflat&Number=160839
		GL_DYNAMIC_DRAW, // nzBufferUsage_Dynamic
		GL_STATIC_DRAW  // nzBufferUsage_Static
	};

	typedef nzUInt8* (*LockRoutine)(nzBufferType type, nzBufferLock lock, unsigned int offset, unsigned int size);

	nzUInt8* LockBuffer(nzBufferType type, nzBufferLock lock, unsigned int offset, unsigned int size)
	{
		NazaraUnused(size);

		if (lock == nzBufferLock_DiscardAndWrite)
		{
			GLint size;
			glGetBufferParameteriv(bufferTargetBinding[type], GL_BUFFER_SIZE, &size);

			GLint usage;
			glGetBufferParameteriv(bufferTargetBinding[type], GL_BUFFER_USAGE, &usage);

			// On discard le buffer
			glBufferData(bufferTargetBinding[type], size, nullptr, usage);
		}

		void* ptr = glMapBuffer(bufferTarget[type], bufferLock[lock]);
		if (ptr)
			return reinterpret_cast<nzUInt8*>(ptr) + offset;
		else
			return nullptr;
	}

	nzUInt8* LockBufferRange(nzBufferType type, nzBufferLock lock, unsigned int offset, unsigned int size)
	{
		return reinterpret_cast<nzUInt8*>(glMapBufferRange(bufferTarget[type], offset, size, bufferLockRange[lock]));
	}

	nzUInt8* LockBufferFirstRun(nzBufferType type, nzBufferLock lock, unsigned int offset, unsigned int size);

	LockRoutine lockBuffer = LockBufferFirstRun;

	nzUInt8* LockBufferFirstRun(nzBufferType type, nzBufferLock lock, unsigned int offset, unsigned int size)
	{
		if (glMapBufferRange)
			lockBuffer = LockBufferRange;
		else
			lockBuffer = LockBuffer;

		return lockBuffer(type, lock, offset, size);
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
	glBindBuffer(bufferTarget[m_type], m_buffer);
}

bool NzHardwareBuffer::Create(unsigned int size, nzBufferUsage usage)
{
	m_buffer = 0;
	glGenBuffers(1, &m_buffer);

	if (!m_buffer)
	{
		NazaraError("Failed to create buffer");
		return false;
	}

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
	glDeleteBuffers(1, &m_buffer);
}

bool NzHardwareBuffer::Fill(const void* data, unsigned int offset, unsigned int size)
{
	GLuint previous;
	glGetIntegerv(bufferTargetBinding[m_type], reinterpret_cast<GLint*>(&previous));

	if (previous != m_buffer)
		glBindBuffer(bufferTarget[m_type], m_buffer);

	// Il semblerait que glBufferSubData soit plus performant que glMapBuffer(Range) en dessous d'un certain seuil
	/*if (size < 32*1024)
	{
		if (size == m_parent->GetLength())
			glBufferData(bufferTarget[m_type], m_parent->GetSize(), data, bufferUsage[m_parent->GetStorage()]);
		else
			glBufferSubData(bufferTarget[m_type], offset, size, data);
	}
	else*/
	{
		// La longueur que nous recevons est en fait la taille
		nzUInt8* ptr = lockBuffer(m_type, (size == m_parent->GetSize()) ? nzBufferLock_DiscardAndWrite : nzBufferLock_WriteOnly, offset, size);
		if (!ptr)
		{
			NazaraError("Failed to lock buffer");
			return false;
		}

		std::memcpy(ptr, data, size);

		if (glUnmapBuffer(bufferTarget[m_type]) != GL_TRUE)
		{
			// Une erreur rare est survenue, nous devons réinitialiser le buffer
			NazaraError("Failed to unlock buffer, reinitialising content... (OpenGL error : 0x" + NzString::Number(glGetError(), 16) + ')');

			glBufferData(bufferTarget[m_type], m_parent->GetSize(), nullptr, bufferUsage[m_parent->GetStorage()]);

			return false;
		}
	}

	// Inutile de rebinder s'il n'y avait aucun buffer (Optimise les opérations chaînées)
	if (previous != m_buffer && previous != 0)
		glBindBuffer(bufferTarget[m_type], previous);

	return true;
}

void* NzHardwareBuffer::GetBufferPtr()
{
	return nullptr;
}

bool NzHardwareBuffer::IsHardware() const
{
	return true;
}

void* NzHardwareBuffer::Lock(nzBufferLock lock, unsigned int offset, unsigned int length)
{
	// Pour ne pas perturber le rendu, on interfère pas avec le binding déjà présent
	GLuint previous;
	glGetIntegerv(bufferTargetBinding[m_type], reinterpret_cast<GLint*>(&previous));

	if (previous != m_buffer)
		glBindBuffer(bufferTarget[m_type], m_buffer);

	void* ptr = lockBuffer(m_type, lock, offset, length);

	// Inutile de rebinder s'il n'y avait aucun buffer (Optimise les opérrations chaînées)
	if (previous != m_buffer && previous != 0)
		glBindBuffer(bufferTarget[m_type], previous);

	return ptr;
}

bool NzHardwareBuffer::Unlock()
{
	GLuint previous;
	glGetIntegerv(bufferTargetBinding[m_type], reinterpret_cast<GLint*>(&previous));

	if (previous != m_buffer)
		glBindBuffer(bufferTarget[m_type], m_buffer);

	if (glUnmapBuffer(bufferTarget[m_type]) != GL_TRUE)
	{
		// Une erreur rare est survenue, nous devons réinitialiser le buffer
		NazaraError("Failed to unlock buffer, reinitialising content... (OpenGL error : 0x" + NzString::Number(glGetError(), 16) + ')');

		glBufferData(bufferTarget[m_type], m_parent->GetSize(), nullptr, bufferUsage[m_parent->GetStorage()]);

		// Inutile de rebinder s'il n'y avait aucun buffer (Optimise les opérrations chaînées)
		if (previous != m_buffer && previous != 0)
			glBindBuffer(bufferTarget[m_type], previous);

		return false;
	}

	// Inutile de rebinder s'il n'y avait aucun buffer (Optimise les opérrations chaînées)
	if (previous != m_buffer && previous != 0)
		glBindBuffer(bufferTarget[m_type], previous);

	return true;
}
