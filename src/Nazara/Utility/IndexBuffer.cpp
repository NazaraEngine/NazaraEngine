// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/IndexBuffer.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Utility/Algorithm.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/IndexIterator.hpp>
#include <Nazara/Utility/IndexMapper.hpp>
#include <stdexcept>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	IndexBuffer::IndexBuffer(bool largeIndices, BufferRef buffer)
	{
		ErrorFlags(ErrorFlag_ThrowException, true);
		Reset(largeIndices, std::move(buffer));
	}

	IndexBuffer::IndexBuffer(bool largeIndices, BufferRef buffer, UInt32 offset, UInt32 size)
	{
		ErrorFlags(ErrorFlag_ThrowException, true);
		Reset(largeIndices, std::move(buffer), offset, size);
	}

	IndexBuffer::IndexBuffer(bool largeIndices, UInt32 length, DataStorage storage, BufferUsageFlags usage)
	{
		ErrorFlags(ErrorFlag_ThrowException, true);
		Reset(largeIndices, length, storage, usage);
	}

	IndexBuffer::IndexBuffer(const IndexBuffer& indexBuffer) :
	RefCounted(),
	m_buffer(indexBuffer.m_buffer),
	m_endOffset(indexBuffer.m_endOffset),
	m_indexCount(indexBuffer.m_indexCount),
	m_startOffset(indexBuffer.m_startOffset),
	m_largeIndices(indexBuffer.m_largeIndices)
	{
	}

	IndexBuffer::~IndexBuffer()
	{
		OnIndexBufferRelease(this);
	}

	unsigned int IndexBuffer::ComputeCacheMissCount() const
	{
		IndexMapper mapper(this);

		return Nz::ComputeCacheMissCount(mapper.begin(), m_indexCount);
	}

	bool IndexBuffer::Fill(const void* data, UInt32 startIndex, UInt32 length)
	{
		UInt32 stride = GetStride();

		return FillRaw(data, startIndex*stride, length*stride);
	}

	bool IndexBuffer::FillRaw(const void* data, UInt32 offset, UInt32 size)
	{
		NazaraAssert(m_buffer && m_buffer->IsValid(), "Invalid buffer");
		NazaraAssert(m_startOffset + offset + size <= m_endOffset, "Exceeding virtual buffer size");

		return m_buffer->Fill(data, m_startOffset+offset, size);
	}

	void* IndexBuffer::MapRaw(BufferAccess access, UInt32 offset, UInt32 size)
	{
		NazaraAssert(m_buffer && m_buffer->IsValid(), "Invalid buffer");
		NazaraAssert(m_startOffset + offset + size <= m_endOffset, "Exceeding virtual buffer size");

		return m_buffer->Map(access, offset, size);
	}

	void* IndexBuffer::MapRaw(BufferAccess access, UInt32 offset, UInt32 size) const
	{
		NazaraAssert(m_buffer && m_buffer->IsValid(), "Invalid buffer");
		NazaraAssert(m_startOffset + offset + size <= m_endOffset, "Exceeding virtual buffer size");

		return m_buffer->Map(access, offset, size);
	}

	void IndexBuffer::Optimize()
	{
		IndexMapper mapper(this);

		OptimizeIndices(mapper.begin(), m_indexCount);
	}

	void IndexBuffer::Reset()
	{
		m_buffer.Reset();
	}

	void IndexBuffer::Reset(bool largeIndices, BufferRef buffer)
	{
		NazaraAssert(buffer && buffer->IsValid(), "Invalid buffer");

		Reset(largeIndices, buffer, 0, buffer->GetSize());
	}

	void IndexBuffer::Reset(bool largeIndices, BufferRef buffer, UInt32 offset, UInt32 size)
	{
		NazaraAssert(buffer && buffer->IsValid(), "Invalid buffer");
		NazaraAssert(size > 0, "Invalid size");
		NazaraAssert(offset + size > buffer->GetSize(), "Virtual buffer exceed buffer bounds");

		UInt32 stride = static_cast<UInt32>((largeIndices) ? sizeof(UInt32) : sizeof(UInt16));

		m_buffer = buffer;
		m_endOffset = offset + size;
		m_indexCount = size / stride;
		m_largeIndices = largeIndices;
		m_startOffset = offset;
	}

	void IndexBuffer::Reset(bool largeIndices, UInt32 length, DataStorage storage, BufferUsageFlags usage)
	{
		UInt32 stride = static_cast<UInt32>((largeIndices) ? sizeof(UInt32) : sizeof(UInt16));

		m_endOffset = length * stride;
		m_indexCount = length;
		m_largeIndices = largeIndices;
		m_startOffset = 0;

		m_buffer = Buffer::New(BufferType_Index, m_endOffset, storage, usage);
	}

	void IndexBuffer::Reset(const IndexBuffer& indexBuffer)
	{
		m_buffer = indexBuffer.m_buffer;
		m_endOffset = indexBuffer.m_endOffset;
		m_indexCount = indexBuffer.m_indexCount;
		m_largeIndices = indexBuffer.m_largeIndices;
		m_startOffset = indexBuffer.m_startOffset;
	}

	void IndexBuffer::Unmap() const
	{
		m_buffer->Unmap();
	}

	IndexBuffer& IndexBuffer::operator=(const IndexBuffer& indexBuffer)
	{
		Reset(indexBuffer);

		return *this;
	}
}
