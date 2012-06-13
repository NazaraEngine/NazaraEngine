// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/SoftwareBuffer.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Context.hpp>
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
	#ifdef NAZARA_DEBUG
	if (NzContext::GetCurrent() == nullptr)
	{
		NazaraError("No active context");
		return;
	}
	#endif

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

	m_mapped = false;

	return true;
}

void NzSoftwareBuffer::Destroy()
{
	delete[] m_buffer;
}

bool NzSoftwareBuffer::Fill(const void* data, unsigned int offset, unsigned int size)
{
	#if NAZARA_RENDERER_SAFE
	if (m_mapped)
	{
		NazaraError("Buffer already mapped");
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

void* NzSoftwareBuffer::Map(nzBufferAccess access, unsigned int offset, unsigned int size)
{
	NazaraUnused(access);
	NazaraUnused(size);

	#if NAZARA_RENDERER_SAFE
	if (m_mapped)
	{
		NazaraError("Buffer already mapped");
		return nullptr;
	}
	#endif

	m_mapped = true;

	return &m_buffer[offset];
}

bool NzSoftwareBuffer::Unmap()
{
	#if NAZARA_RENDERER_SAFE
	if (!m_mapped)
	{
		NazaraError("Buffer not mapped");
		return true;
	}
	#endif

	m_mapped = false;

	return true;
}
