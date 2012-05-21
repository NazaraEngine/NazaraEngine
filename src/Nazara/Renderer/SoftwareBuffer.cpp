// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/SoftwareBuffer.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <cstring>
#include <stdexcept>
#include <Nazara/Renderer/Debug.hpp>

namespace
{
	GLenum bufferTarget[] = {
		GL_ELEMENT_ARRAY_BUFFER, // nzBufferType_Index,
		GL_ARRAY_BUFFER,		 // nzBufferType_Vertex
	};
}

NzSoftwareBuffer::NzSoftwareBuffer(NzBuffer* parent, nzBufferType type) :
m_type(type)
{
	NazaraUnused(parent);
}

NzSoftwareBuffer::~NzSoftwareBuffer()
{
}

void NzSoftwareBuffer::Bind()
{
	glBindBuffer(bufferTarget[m_type], 0);
}

bool NzSoftwareBuffer::Create(unsigned int size, nzBufferUsage usage)
{
	NazaraUnused(usage);

	// Cette allocation est protégée car sa taille dépend directement de paramètres utilisateurs
	try
	{
		m_buffer = new nzUInt8[size];
	}
	catch (const std::exception& e)
	{
		NazaraError("Failed to allocate software buffer (" + NzString(e.what()) + ')');
		return false;
	}

	m_locked = false;

	return true;
}

void NzSoftwareBuffer::Destroy()
{
	delete[] m_buffer;
}

bool NzSoftwareBuffer::Fill(const void* data, unsigned int offset, unsigned int size)
{
	#if NAZARA_RENDERER_SAFE
	if (m_locked)
	{
		NazaraError("Buffer already locked");
		return false;
	}
	#endif

	std::memcpy(&m_buffer[offset], data, size);

	return true;
}

void* NzSoftwareBuffer::GetBufferPtr()
{
	return m_buffer;
}

bool NzSoftwareBuffer::IsHardware() const
{
	return false;
}

void* NzSoftwareBuffer::Lock(nzBufferLock lock, unsigned int offset, unsigned int size)
{
	NazaraUnused(lock);
	NazaraUnused(size);

	#if NAZARA_RENDERER_SAFE
	if (m_locked)
	{
		NazaraError("Buffer already locked");
		return nullptr;
	}
	#endif

	m_locked = true;

	return &m_buffer[offset];
}

bool NzSoftwareBuffer::Unlock()
{
	#if NAZARA_RENDERER_SAFE
	if (!m_locked)
	{
		NazaraError("Buffer not locked");
		return true;
	}
	#endif

	m_locked = false;

	return true;
}
