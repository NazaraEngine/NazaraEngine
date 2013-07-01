// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/IndexBuffer.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Utility/Algorithm.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/IndexIterator.hpp>
#include <Nazara/Utility/IndexMapper.hpp>
#include <stdexcept>
#include <Nazara/Utility/Debug.hpp>

NzIndexBuffer::NzIndexBuffer(bool largeIndices, NzBuffer* buffer, unsigned int startOffset, unsigned int endOffset) :
m_buffer(buffer),
m_largeIndices(largeIndices),
m_endOffset(endOffset),
m_startOffset(startOffset)
{
	#ifdef NAZARA_DEBUG
	if (!m_buffer || !m_buffer->IsValid())
	{
		NazaraError("Buffer is invalid");
		throw std::invalid_argument("Buffer must be valid");
	}

	if (endOffset > startOffset)
	{
		NazaraError("End offset cannot be over start offset");
		throw std::invalid_argument("End offset cannot be over start offset");
	}

	unsigned int bufferSize = m_buffer->GetSize();
	if (startOffset >= bufferSize)
	{
		NazaraError("Start offset is over buffer size");
		throw std::invalid_argument("Start offset is over buffer size");
	}

	if (endOffset >= bufferSize)
	{
		NazaraError("End offset is over buffer size");
		throw std::invalid_argument("End offset is over buffer size");
	}
	#endif

	unsigned int stride = (largeIndices) ? sizeof(nzUInt32) : sizeof(nzUInt16);

	m_indexCount = (endOffset - startOffset) / stride;
}

NzIndexBuffer::NzIndexBuffer(bool largeIndices, unsigned int length, nzBufferStorage storage, nzBufferUsage usage) :
m_largeIndices(largeIndices),
m_indexCount(length),
m_startOffset(0)
{
	m_endOffset = length * ((largeIndices) ? sizeof(nzUInt32) : sizeof(nzUInt16));

	m_buffer = new NzBuffer(nzBufferType_Index, m_endOffset, storage, usage);
	m_buffer->SetPersistent(false);
}

NzIndexBuffer::NzIndexBuffer(const NzIndexBuffer& indexBuffer) :
NzResource(),
m_buffer(indexBuffer.m_buffer),
m_largeIndices(indexBuffer.m_largeIndices),
m_endOffset(indexBuffer.m_endOffset),
m_indexCount(indexBuffer.m_indexCount),
m_startOffset(indexBuffer.m_startOffset)
{
}

unsigned int NzIndexBuffer::ComputeCacheMissCount() const
{
	NzIndexMapper mapper(this);

	return NzComputeCacheMissCount(mapper.begin(), m_indexCount);
}

bool NzIndexBuffer::Fill(const void* data, unsigned int offset, unsigned int size, bool forceDiscard)
{
	#if NAZARA_UTILITY_SAFE
	if (m_startOffset + offset + size > m_endOffset)
	{
		NazaraError("Exceeding virtual buffer size");
		return false;
	}
	#endif

	return m_buffer->Fill(data, m_startOffset+offset, size, forceDiscard);
}

bool NzIndexBuffer::FillIndices(const void* data, unsigned int startIndex, unsigned int length, bool forceDiscard)
{
	unsigned int stride = GetStride();
	return Fill(data, startIndex*stride, length*stride, forceDiscard);
}

NzBuffer* NzIndexBuffer::GetBuffer() const
{
	return m_buffer;
}

unsigned int NzIndexBuffer::GetEndOffset() const
{
	return m_endOffset;
}

unsigned int NzIndexBuffer::GetIndexCount() const
{
	return m_indexCount;
}

unsigned int NzIndexBuffer::GetStride() const
{
	return (m_largeIndices) ? sizeof(nzUInt32) : sizeof(nzUInt16);
}

unsigned int NzIndexBuffer::GetStartOffset() const
{
	return m_startOffset;
}

bool NzIndexBuffer::HasLargeIndices() const
{
	return m_largeIndices;
}

bool NzIndexBuffer::IsHardware() const
{
	return m_buffer->IsHardware();
}

void* NzIndexBuffer::Map(nzBufferAccess access, unsigned int offset, unsigned int size)
{
	#if NAZARA_UTILITY_SAFE
	if (m_startOffset + offset + size > m_endOffset)
	{
		NazaraError("Exceeding virtual buffer size");
		return false;
	}
	#endif

	return m_buffer->Map(access, offset, size);
}

void* NzIndexBuffer::Map(nzBufferAccess access, unsigned int offset, unsigned int size) const
{
	#if NAZARA_UTILITY_SAFE
	if (m_startOffset + offset + size > m_endOffset)
	{
		NazaraError("Exceeding virtual buffer size");
		return nullptr;
	}
	#endif

	return m_buffer->Map(access, offset, size);
}

void* NzIndexBuffer::MapIndices(nzBufferAccess access, unsigned int startIndex, unsigned int length)
{
	unsigned int stride = GetStride();

	return Map(access, startIndex*stride, length*stride);
}

void* NzIndexBuffer::MapIndices(nzBufferAccess access, unsigned int startIndex, unsigned int length) const
{
	unsigned int stride = GetStride();

	return Map(access, startIndex*stride, length*stride);
}

void NzIndexBuffer::Optimize()
{
	NzIndexMapper mapper(this);

	NzOptimizeIndices(mapper.begin(), m_indexCount);
}

bool NzIndexBuffer::SetStorage(nzBufferStorage storage)
{
	return m_buffer->SetStorage(storage);
}

void NzIndexBuffer::Unmap() const
{
	m_buffer->Unmap();
}
