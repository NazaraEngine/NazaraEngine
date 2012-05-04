// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/IndexBuffer.hpp>
#include <Nazara/Core/Error.hpp>
#include <stdexcept>
#include <Nazara/Renderer/Debug.hpp>

NzIndexBuffer::NzIndexBuffer(NzBuffer* buffer, unsigned int startIndex, unsigned int indexCount) :
m_buffer(buffer),
m_ownsBuffer(false),
m_indexCount(indexCount),
m_startIndex(startIndex)
{
	if (m_buffer)
	{
		#ifdef NAZARA_DEBUG
		nzUInt8 indexSize = m_buffer->GetSize();
		if (indexSize != 1 && indexSize != 2 && indexSize != 4)
		{
			NazaraError("Invalid index size (" + NzString::Number(indexSize) + ')');
			m_buffer = nullptr;

			throw std::runtime_error("Constructor failed");
		}
		#endif

		m_buffer->AddResourceReference();
	}
}

NzIndexBuffer::NzIndexBuffer(unsigned int length, nzUInt8 indexSize, nzBufferUsage usage) :
m_ownsBuffer(true),
m_indexCount(length),
m_startIndex(0)
{
	#ifdef NAZARA_DEBUG
	if (indexSize != 1 && indexSize != 2 && indexSize != 4)
	{
		NazaraError("Invalid index size");
		m_buffer = nullptr;

		throw std::runtime_error("Constructor failed");
	}
	#endif

	m_buffer = new NzBuffer(nzBufferType_Index, length, indexSize, usage);
	m_buffer->AddResourceReference();
	m_buffer->SetPersistent(false);
}

NzIndexBuffer::NzIndexBuffer(const NzIndexBuffer& indexBuffer) :
m_buffer(indexBuffer.m_buffer),
m_indexCount(indexBuffer.m_indexCount),
m_startIndex(indexBuffer.m_startIndex)
{
	if (m_buffer)
	{
		if (m_ownsBuffer)
		{
			m_buffer = new NzBuffer(nzBufferType_Index, indexBuffer.m_buffer->GetLength(), indexBuffer.m_buffer->GetSize(), indexBuffer.m_buffer->GetUsage());
			m_buffer->AddResourceReference();
			m_buffer->SetPersistent(false);
			m_buffer->CopyContent(*indexBuffer.m_buffer);
		}
		else
		{
			m_buffer = indexBuffer.m_buffer;
			m_buffer->AddResourceReference();
		}
	}
	else
		m_buffer = nullptr;
}

NzIndexBuffer::~NzIndexBuffer()
{
	if (m_buffer)
		m_buffer->RemoveResourceReference();
}

bool NzIndexBuffer::Fill(const void* data, unsigned int offset, unsigned int length)
{
	#if NAZARA_RENDERER_SAFE
	if (!m_buffer)
	{
		NazaraError("Impossible to fill sequential buffer");
		return false;
	}

	if (offset+length > m_indexCount)
	{
		NazaraError("Exceeding virtual buffer size");
		return false;
	}
	#endif

	return m_buffer->Fill(data, m_startIndex+offset, length);
}

NzBuffer* NzIndexBuffer::GetBuffer() const
{
	return m_buffer;
}

void* NzIndexBuffer::GetBufferPtr()
{
	#if NAZARA_RENDERER_SAFE
	if (!m_buffer)
	{
		NazaraError("Sequential index buffer: Buffer has no pointer");
		return nullptr;
	}
	#endif

	return reinterpret_cast<nzUInt8*>(m_buffer->GetBufferPtr()) + m_startIndex*m_buffer->GetTypeSize();
}

const void* NzIndexBuffer::GetBufferPtr() const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_buffer)
	{
		NazaraError("Sequential index buffer: Buffer has no pointer");
		return nullptr;
	}
	#endif

	return reinterpret_cast<const nzUInt8*>(m_buffer->GetBufferPtr()) + m_startIndex*m_buffer->GetTypeSize();
}

nzUInt8 NzIndexBuffer::GetIndexSize() const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_buffer)
	{
		NazaraError("Sequential index buffer: Buffer has no index size");
		return 0;
	}
	#endif

	return m_buffer->GetTypeSize();
}

unsigned int NzIndexBuffer::GetIndexCount() const
{
	return m_indexCount;
}

unsigned int NzIndexBuffer::GetStartIndex() const
{
	return m_startIndex;
}

bool NzIndexBuffer::IsHardware() const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_buffer)
	{
		NazaraError("Sequential index buffer is neither hardware or software");
		return false;
	}
	#endif

	return m_buffer->IsHardware();
}

bool NzIndexBuffer::IsSequential() const
{
	return m_buffer == nullptr;
}

void* NzIndexBuffer::Lock(nzBufferLock lock, unsigned int offset, unsigned int length)
{
	#if NAZARA_RENDERER_SAFE
	if (!m_buffer)
	{
		NazaraError("Impossible to lock sequential index buffer");
		return nullptr;
	}

	if (offset+length > m_indexCount)
	{
		NazaraError("Exceeding virtual buffer size");
		return nullptr;
	}
	#endif

	return m_buffer->Lock(lock, m_startIndex+offset, (length) ? length : m_indexCount-offset);
}

bool NzIndexBuffer::Unlock()
{
	#if NAZARA_RENDERER_SAFE
	if (!m_buffer)
	{
		NazaraError("Impossible to unlock sequential index buffer");
		return false;
	}
	#endif

	return m_buffer->Unlock();
}
