// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Buffer.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Utility/AbstractBuffer.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/SoftwareBuffer.hpp>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <Nazara/Utility/Debug.hpp>

namespace
{
	NzAbstractBuffer* SoftwareBufferFunction(NzBuffer* parent, nzBufferType type)
	{
		return new NzSoftwareBuffer(parent, type);
	}
}

NzBuffer::NzBuffer(nzBufferType type) :
m_type(type),
m_impl(nullptr),
m_size(0)
{
}

NzBuffer::NzBuffer(nzBufferType type, unsigned int size, nzBufferStorage storage, nzBufferUsage usage) :
m_type(type),
m_impl(nullptr)
{
	Create(size, storage, usage);

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

bool NzBuffer::CopyContent(const NzBuffer& buffer)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Buffer must be valid");
		return false;
	}

	if (!buffer.IsValid())
	{
		NazaraError("Source buffer must be valid");
		return false;
	}
	#endif

	NzBufferMapper<NzBuffer> mapper(buffer, nzBufferAccess_ReadOnly);

	return Fill(mapper.GetPointer(), 0, buffer.GetSize());
}

bool NzBuffer::Create(unsigned int size, nzBufferStorage storage, nzBufferUsage usage)
{
	Destroy();

	// Notre buffer est-il supporté ?
	if (!s_bufferFunctions[storage])
	{
		NazaraError("Buffer storage not supported");
		return false;
	}

	std::unique_ptr<NzAbstractBuffer> impl(s_bufferFunctions[storage](this, m_type));
	if (!impl->Create(size, usage))
	{
		NazaraError("Failed to create buffer");
		return false;
	}

	m_impl = impl.release();
	m_size = size;
	m_storage = storage;
	m_usage = usage;

	NotifyCreated();
	return true; // Si on arrive ici c'est que tout s'est bien passé.
}

void NzBuffer::Destroy()
{
	if (m_impl)
	{
		NotifyDestroy();

		m_impl->Destroy();
		delete m_impl;
		m_impl = nullptr;
	}
}

bool NzBuffer::Fill(const void* data, unsigned int offset, unsigned int size, bool forceDiscard)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Buffer not valid");
		return false;
	}

	if (offset+size > m_size)
	{
		NazaraError("Exceeding buffer size (" + NzString::Number(offset+size) + " > " + NzString::Number(m_size) + ')');
		return false;
	}
	#endif

	return m_impl->Fill(data, offset, (size == 0) ? m_size-offset : size, forceDiscard);
}

NzAbstractBuffer* NzBuffer::GetImpl() const
{
	return m_impl;
}

unsigned int NzBuffer::GetSize() const
{
	return m_size;
}

nzBufferStorage NzBuffer::GetStorage() const
{
	return m_storage;
}

nzBufferType NzBuffer::GetType() const
{
	return m_type;
}

nzBufferUsage NzBuffer::GetUsage() const
{
	return m_usage;
}

bool NzBuffer::IsHardware() const
{
	return m_storage == nzBufferStorage_Hardware;
}

bool NzBuffer::IsValid() const
{
	return m_impl != nullptr;
}

void* NzBuffer::Map(nzBufferAccess access, unsigned int offset, unsigned int size)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Buffer not valid");
		return nullptr;
	}

	if (offset+size > m_size)
	{
		NazaraError("Exceeding buffer size");
		return nullptr;
	}
	#endif

	return m_impl->Map(access, offset, (size == 0) ? m_size-offset : size);
}

void* NzBuffer::Map(nzBufferAccess access, unsigned int offset, unsigned int size) const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Buffer not valid");
		return nullptr;
	}

	if (access != nzBufferAccess_ReadOnly)
	{
		NazaraError("Buffer access must be read-only when used const");
		return nullptr;
	}

	if (offset+size > m_size)
	{
		NazaraError("Exceeding buffer size");
		return nullptr;
	}
	#endif

	return m_impl->Map(access, offset, (size == 0) ? m_size-offset : size);
}

bool NzBuffer::SetStorage(nzBufferStorage storage)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Buffer not valid");
		return false;
	}
	#endif

	if (m_storage == storage)
		return true;

	#if NAZARA_UTILITY_SAFE
	if (!IsSupported(storage))
	{
		NazaraError("Storage not supported");
		return false;
	}
	#endif

	void* ptr = m_impl->Map(nzBufferAccess_ReadOnly, 0, m_size);
	if (!ptr)
	{
		NazaraError("Failed to map buffer");
		return false;
	}

	NzAbstractBuffer* impl = s_bufferFunctions[storage](this, m_type);
	if (!impl->Create(m_size, m_usage))
	{
		NazaraError("Failed to create buffer");
		delete impl;
		m_impl->Unmap();

		return false;
	}

	if (!impl->Fill(ptr, 0, m_size))
	{
		NazaraError("Failed to fill buffer");
		impl->Destroy();
		delete impl;
		m_impl->Unmap();

		return false;
	}

	m_impl->Unmap();
	m_impl->Destroy();
	delete m_impl;

	m_impl = impl;
	m_storage = storage;

	return true;
}

void NzBuffer::Unmap() const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Buffer not valid");
		return;
	}
	#endif

	if (!m_impl->Unmap())
		NazaraWarning("Failed to unmap buffer (it's content is undefined)"); ///TODO: Unexpected ?
}

bool NzBuffer::IsSupported(nzBufferStorage storage)
{
	return s_bufferFunctions[storage] != nullptr;
}

void NzBuffer::SetBufferFunction(nzBufferStorage storage, BufferFunction func)
{
	s_bufferFunctions[storage] = func;
}

bool NzBuffer::Initialize()
{
	s_bufferFunctions[nzBufferStorage_Software] = SoftwareBufferFunction;

	return true;
}

void NzBuffer::Uninitialize()
{
	std::memset(s_bufferFunctions, 0, (nzBufferStorage_Max+1)*sizeof(NzBuffer::BufferFunction));
}

NzBuffer::BufferFunction NzBuffer::s_bufferFunctions[nzBufferStorage_Max+1] = {0};
