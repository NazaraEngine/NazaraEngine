// Copyright (C) 2014 Jérôme Leclercq
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

NzIndexBuffer::NzIndexBuffer(bool largeIndices, NzBuffer* buffer, unsigned int startOffset, unsigned int endOffset)
{
	Reset(largeIndices, buffer, startOffset, endOffset);
}

NzIndexBuffer::NzIndexBuffer(bool largeIndices, unsigned int length, nzBufferStorage storage, nzBufferUsage usage)
{
	Reset(largeIndices, length, storage, usage);
}

NzIndexBuffer::NzIndexBuffer(const NzIndexBuffer& indexBuffer) :
NzRefCounted(),
m_buffer(indexBuffer.m_buffer),
m_largeIndices(indexBuffer.m_largeIndices),
m_endOffset(indexBuffer.m_endOffset),
m_indexCount(indexBuffer.m_indexCount),
m_startOffset(indexBuffer.m_startOffset)
{
}

NzIndexBuffer::NzIndexBuffer(NzIndexBuffer&& indexBuffer) noexcept :
NzRefCounted(),
m_buffer(std::move(indexBuffer.m_buffer)),
m_largeIndices(indexBuffer.m_largeIndices),
m_endOffset(indexBuffer.m_endOffset),
m_indexCount(indexBuffer.m_indexCount),
m_startOffset(indexBuffer.m_startOffset)
{
}

NzIndexBuffer::~NzIndexBuffer()
{
	NotifyDestroy();
}

unsigned int NzIndexBuffer::ComputeCacheMissCount() const
{
	NzIndexMapper mapper(this);

	return NzComputeCacheMissCount(mapper.begin(), m_indexCount);
}

bool NzIndexBuffer::Fill(const void* data, unsigned int startIndex, unsigned int length, bool forceDiscard)
{
	unsigned int stride = GetStride();
	return FillRaw(data, startIndex*stride, length*stride, forceDiscard);
}

bool NzIndexBuffer::FillRaw(const void* data, unsigned int offset, unsigned int size, bool forceDiscard)
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

bool NzIndexBuffer::IsValid() const
{
	return m_buffer;
}

void* NzIndexBuffer::Map(nzBufferAccess access, unsigned int startIndex, unsigned int length)
{
	unsigned int stride = GetStride();
	return MapRaw(access, startIndex*stride, length*stride);
}

void* NzIndexBuffer::Map(nzBufferAccess access, unsigned int startIndex, unsigned int length) const
{
	unsigned int stride = GetStride();
	return MapRaw(access, startIndex*stride, length*stride);
}

void* NzIndexBuffer::MapRaw(nzBufferAccess access, unsigned int offset, unsigned int size)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_buffer)
	{
		NazaraError("No buffer");
		return nullptr;
	}

	if (m_startOffset + offset + size > m_endOffset)
	{
		NazaraError("Exceeding virtual buffer size");
		return nullptr;
	}
	#endif

	return m_buffer->Map(access, offset, size);
}

void* NzIndexBuffer::MapRaw(nzBufferAccess access, unsigned int offset, unsigned int size) const
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

void NzIndexBuffer::Optimize()
{
	NzIndexMapper mapper(this);

	NzOptimizeIndices(mapper.begin(), m_indexCount);
}

void NzIndexBuffer::Reset()
{
	m_buffer.Reset();
}

void NzIndexBuffer::Reset(bool largeIndices, NzBuffer* buffer, unsigned int startOffset, unsigned int endOffset)
{
	#if NAZARA_UTILITY_SAFE
	if (!buffer || !buffer->IsValid())
	{
		NazaraError("Buffer is invalid");
		return;
	}

	if (endOffset > startOffset)
	{
		NazaraError("End offset cannot be over start offset");
		return;
	}

	unsigned int bufferSize = buffer->GetSize();
	if (startOffset >= bufferSize)
	{
		NazaraError("Start offset is over buffer size");
		return;
	}

	if (endOffset >= bufferSize)
	{
		NazaraError("End offset is over buffer size");
		return;
	}
	#endif

	unsigned int stride = (largeIndices) ? sizeof(nzUInt32) : sizeof(nzUInt16);

	m_buffer = buffer;
	m_endOffset = endOffset;
	m_indexCount = (endOffset - startOffset) / stride;
	m_largeIndices = largeIndices;
	m_startOffset = startOffset;
}

void NzIndexBuffer::Reset(bool largeIndices, unsigned int length, nzBufferStorage storage, nzBufferUsage usage)
{
	unsigned int stride = (largeIndices) ? sizeof(nzUInt32) : sizeof(nzUInt16);

	m_endOffset = length * stride;
	m_indexCount = length;
	m_largeIndices = largeIndices;
	m_startOffset = 0;

	m_buffer = new NzBuffer(nzBufferType_Index, m_endOffset, storage, usage);
	m_buffer->SetPersistent(false);
}

void NzIndexBuffer::Reset(const NzIndexBuffer& indexBuffer)
{
	m_buffer = indexBuffer.m_buffer;
	m_endOffset = indexBuffer.m_endOffset;
	m_indexCount = indexBuffer.m_indexCount;
	m_largeIndices = indexBuffer.m_largeIndices;
	m_startOffset = indexBuffer.m_startOffset;
}

void NzIndexBuffer::Reset(NzIndexBuffer&& indexBuffer) noexcept
{
	m_buffer = std::move(indexBuffer.m_buffer);
	m_endOffset = indexBuffer.m_endOffset;
	m_indexCount = indexBuffer.m_indexCount;
	m_largeIndices = indexBuffer.m_largeIndices;
	m_startOffset = indexBuffer.m_startOffset;
}

bool NzIndexBuffer::SetStorage(nzBufferStorage storage)
{
	return m_buffer->SetStorage(storage);
}

void NzIndexBuffer::Unmap() const
{
	m_buffer->Unmap();
}

NzIndexBuffer& NzIndexBuffer::operator=(const NzIndexBuffer& indexBuffer)
{
	Reset(indexBuffer);

	return *this;
}

NzIndexBuffer& NzIndexBuffer::operator=(NzIndexBuffer&& indexBuffer) noexcept
{
	Reset(indexBuffer);

	return *this;
}
