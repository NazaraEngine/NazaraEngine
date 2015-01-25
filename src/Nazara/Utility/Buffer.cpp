// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Buffer.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
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
	NzAbstractBuffer* SoftwareBufferFactory(NzBuffer* parent, nzBufferType type)
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

NzBuffer::NzBuffer(nzBufferType type, unsigned int size, nzUInt32 storage, nzBufferUsage usage) :
m_type(type),
m_impl(nullptr)
{
	NzErrorFlags flags(nzErrorFlag_ThrowException, true);
	Create(size, storage, usage);
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

bool NzBuffer::Create(unsigned int size, nzUInt32 storage, nzBufferUsage usage)
{
	Destroy();

	// Notre buffer est-il supporté ?
	if (!IsStorageSupported(storage))
	{
		NazaraError("Buffer storage not supported");
		return false;
	}

	std::unique_ptr<NzAbstractBuffer> impl(s_bufferFactories[storage](this, m_type));
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

nzUInt32 NzBuffer::GetStorage() const
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
	return m_storage & nzDataStorage_Hardware;
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

bool NzBuffer::SetStorage(nzUInt32 storage)
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

	if (!IsStorageSupported(storage))
	{
		NazaraError("Storage not supported");
		return false;
	}

	void* ptr = m_impl->Map(nzBufferAccess_ReadOnly, 0, m_size);
	if (!ptr)
	{
		NazaraError("Failed to map buffer");
		return false;
	}

	NzCallOnExit unmapMyImpl([this]()
	{
		m_impl->Unmap();
	});

	std::unique_ptr<NzAbstractBuffer> impl(s_bufferFactories[storage](this, m_type));
	if (!impl->Create(m_size, m_usage))
	{
		NazaraError("Failed to create buffer");
		return false;
	}

	NzCallOnExit destroyImpl([&impl]()
	{
		impl->Destroy();
	});

	if (!impl->Fill(ptr, 0, m_size))
	{
		NazaraError("Failed to fill buffer");
		return false;
	}

	destroyImpl.Reset();

	unmapMyImpl.CallAndReset();
	m_impl->Destroy();
	delete m_impl;

	m_impl = impl.release();
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
		NazaraWarning("Failed to unmap buffer (it's content may be undefined)"); ///TODO: Unexpected ?
}

bool NzBuffer::IsStorageSupported(nzUInt32 storage)
{
	return s_bufferFactories[storage] != nullptr;
}

void NzBuffer::SetBufferFactory(nzUInt32 storage, BufferFactory func)
{
	s_bufferFactories[storage] = func;
}

bool NzBuffer::Initialize()
{
	s_bufferFactories[nzDataStorage_Software] = SoftwareBufferFactory;

	return true;
}

void NzBuffer::Uninitialize()
{
	std::memset(s_bufferFactories, 0, (nzDataStorage_Max+1)*sizeof(NzBuffer::BufferFactory));
}

NzBuffer::BufferFactory NzBuffer::s_bufferFactories[nzDataStorage_Max+1] = {nullptr};
