// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/Buffer.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Renderer/HardwareBuffer.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/SoftwareBuffer.hpp>
#include <stdexcept>
#include <Nazara/Renderer/Debug.hpp>

namespace
{
	nzRendererCap storageToCapability[] = {
		nzRendererCap_HardwareBuffer, // nzBufferStorage_Hardware
		nzRendererCap_SoftwareBuffer, // nzBufferStorage_Software
	};
}

NzBuffer::NzBuffer(nzBufferType type) :
m_type(type),
m_typeSize(0),
m_impl(nullptr),
m_length(0)
{
}

NzBuffer::NzBuffer(nzBufferType type, unsigned int length, nzUInt8 typeSize, nzBufferUsage usage) :
m_type(type),
m_impl(nullptr)
{
	Create(length, typeSize, usage);

	#ifdef NAZARA_DEBUG
	if (!m_impl)
	{
		NazaraError("Failed to create buffer");
		throw std::runtime_error("Constructor failed");
	}
	#endif
}

NzBuffer::~NzBuffer()
{
	Destroy();
}

bool NzBuffer::CopyContent(NzBuffer& buffer)
{
	void* ptr = buffer.Lock(nzBufferLock_ReadOnly);
	if (!ptr)
	{
		NazaraError("Unable to lock source buffer");
		return false;
	}

	bool r = Fill(ptr, 0, m_length);

	if (!buffer.Unlock())
		NazaraWarning("Unable to unlock source buffer");

	return r;
}

bool NzBuffer::Create(unsigned int length, nzUInt8 typeSize, nzBufferUsage usage)
{
	Destroy();

	// On tente d'abord de faire un buffer hardware, si supporté
	if (NazaraRenderer->HasCapability(nzRendererCap_HardwareBuffer))
	{
		m_impl = new NzHardwareBuffer(this, m_type);
		if (!m_impl->Create(length*typeSize, usage))
		{
			NazaraWarning("Failed to create hardware buffer, trying to create software buffer...");

			delete m_impl;
			m_impl = nullptr;
		}
	}

	if (!m_impl)
	{
		if (!NazaraRenderer->HasCapability(nzRendererCap_SoftwareBuffer))
		{
			// Ne devrait jamais arriver
			NazaraError("Software buffer not supported");
			return false;
		}

		m_impl = new NzSoftwareBuffer(this, m_type);
		if (!m_impl->Create(length*typeSize, usage))
		{
			NazaraError("Failed to create software buffer");
			delete m_impl;
			m_impl = nullptr;

			return false;
		}
	}

	m_length = length;
	m_typeSize = typeSize;
	m_usage = usage;

	// Si on arrive ici c'est que tout s'est bien passé.
	return true;
}

void NzBuffer::Destroy()
{
	if (m_impl)
	{
		m_impl->Destroy();
		delete m_impl;
		m_impl = nullptr;
	}
}

bool NzBuffer::Fill(const void* data, unsigned int offset, unsigned int length)
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Buffer not created");
		return false;
	}

	if (offset+length > m_length)
	{
		NazaraError("Exceeding buffer size");
		return false;
	}
	#endif

	return m_impl->Fill(data, offset*m_typeSize, length*m_typeSize);
}

void* NzBuffer::GetBufferPtr()
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Buffer not created");
		return false;
	}
	#endif

	return m_impl->GetBufferPtr();
}

const void* NzBuffer::GetBufferPtr() const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Buffer not created");
		return false;
	}
	#endif

	return m_impl->GetBufferPtr();
}

NzBufferImpl* NzBuffer::GetImpl() const
{
	return m_impl;
}

unsigned int NzBuffer::GetLength() const
{
	return m_length;
}

unsigned int NzBuffer::GetSize() const
{
	return m_length*m_typeSize;
}

nzBufferStorage NzBuffer::GetStorage() const
{
	return m_storage;
}

nzBufferType NzBuffer::GetType() const
{
	return m_type;
}

nzUInt8 NzBuffer::GetTypeSize() const
{
	return m_typeSize;
}

nzBufferUsage NzBuffer::GetUsage() const
{
	return m_usage;
}

bool NzBuffer::IsHardware() const
{
	return m_storage == nzBufferStorage_Hardware;
}

void* NzBuffer::Lock(nzBufferLock lock, unsigned int offset, unsigned int length)
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Buffer not created");
		return nullptr;
	}

	if (offset+length > m_length)
	{
		NazaraError("Exceeding buffer size");
		return nullptr;
	}
	#endif

	return m_impl->Lock(lock, offset*m_typeSize, length*m_typeSize);
}

bool NzBuffer::Unlock()
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Buffer not created");
		return false;
	}
	#endif

	return m_impl->Unlock();
}

bool NzBuffer::IsSupported(nzBufferStorage storage)
{
	return NazaraRenderer->HasCapability(storageToCapability[storage]);
}
