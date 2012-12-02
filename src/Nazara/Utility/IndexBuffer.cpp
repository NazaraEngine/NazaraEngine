// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/IndexBuffer.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Utility/Config.hpp>
#include <stdexcept>
#include <Nazara/Utility/Debug.hpp>

///FIXME: Gérer efficacement les erreurs de création du buffer

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
		if (indexSize != 2 && indexSize != 4)
		{
			NazaraError("Invalid index size (" + NzString::Number(indexSize) + ')');
			m_buffer = nullptr;

			throw std::runtime_error("Constructor failed");
		}
		#endif

		m_buffer->AddResourceReference();
	}
}

NzIndexBuffer::NzIndexBuffer(unsigned int length, bool largeIndices, nzBufferStorage storage, nzBufferUsage usage) :
m_ownsBuffer(true),
m_indexCount(length),
m_startIndex(0)
{
	m_buffer = new NzBuffer(nzBufferType_Index, length, (largeIndices) ? 4 : 2, storage, usage);
	m_buffer->AddResourceReference();
	m_buffer->SetPersistent(false);
}

NzIndexBuffer::NzIndexBuffer(const NzIndexBuffer& indexBuffer) :
NzResource(true),
m_buffer(indexBuffer.m_buffer),
m_ownsBuffer(indexBuffer.m_ownsBuffer),
m_indexCount(indexBuffer.m_indexCount),
m_startIndex(indexBuffer.m_startIndex)
{
	if (m_buffer)
	{
		if (m_ownsBuffer)
		{
			NzBuffer* buffer = indexBuffer.m_buffer;

			m_buffer = new NzBuffer(nzBufferType_Index, buffer->GetLength(), buffer->GetSize(), buffer->GetStorage(), buffer->GetUsage());
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
}

NzIndexBuffer::~NzIndexBuffer()
{
	if (m_buffer)
		m_buffer->RemoveResourceReference();
}

bool NzIndexBuffer::Fill(const void* data, unsigned int offset, unsigned int length)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_buffer)
	{
		NazaraError("Impossible to fill sequential buffers");
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

void* NzIndexBuffer::GetPointer()
{
	#if NAZARA_UTILITY_SAFE
	if (!m_buffer)
	{
		NazaraError("Sequential buffers have no pointer");
		return nullptr;
	}
	#endif

	return reinterpret_cast<nzUInt8*>(m_buffer->GetPointer()) + m_startIndex*m_buffer->GetTypeSize();
}

const void* NzIndexBuffer::GetPointer() const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_buffer)
	{
		NazaraError("Sequential buffers have no pointer");
		return nullptr;
	}
	#endif

	return reinterpret_cast<const nzUInt8*>(m_buffer->GetPointer()) + m_startIndex*m_buffer->GetTypeSize();
}

unsigned int NzIndexBuffer::GetIndexCount() const
{
	return m_indexCount;
}

unsigned int NzIndexBuffer::GetStartIndex() const
{
	return m_startIndex;
}

bool NzIndexBuffer::HasLargeIndices() const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_buffer)
	{
		NazaraError("Sequential buffers have no index size");
		return 0;
	}
	#endif

	return (m_buffer->GetTypeSize() == 4);
}

bool NzIndexBuffer::IsHardware() const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_buffer)
	{
		NazaraWarning("Sequential index buffers are neither hardware or software");
		return false;
	}
	#endif

	return m_buffer->IsHardware();
}

bool NzIndexBuffer::IsSequential() const
{
	return m_buffer == nullptr;
}

void* NzIndexBuffer::Map(nzBufferAccess access, unsigned int offset, unsigned int length)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_buffer)
	{
		NazaraError("Impossible to map sequential buffers");
		return nullptr;
	}

	if (offset+length > m_indexCount)
	{
		NazaraError("Exceeding virtual buffer size");
		return nullptr;
	}
	#endif

	return m_buffer->Map(access, m_startIndex+offset, (length) ? length : m_indexCount-offset);
}

bool NzIndexBuffer::SetStorage(nzBufferStorage storage)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_buffer)
	{
		NazaraWarning("Sequential buffers have no storage");
		return true;
	}
	#endif

	return m_buffer->SetStorage(storage);
}

bool NzIndexBuffer::Unmap()
{
	#if NAZARA_UTILITY_SAFE
	if (!m_buffer)
	{
		NazaraError("Impossible to unlock sequential buffers");
		return false;
	}
	#endif

	return m_buffer->Unmap();
}
