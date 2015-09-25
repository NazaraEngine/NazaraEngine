// Copyright (C) 2015 Jérôme Leclercq
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
	IndexBuffer::IndexBuffer(bool largeIndices, Buffer* buffer)
	{
		ErrorFlags(ErrorFlag_ThrowException, true);
		Reset(largeIndices, buffer);
	}

	IndexBuffer::IndexBuffer(bool largeIndices, Buffer* buffer, unsigned int startOffset, unsigned int endOffset)
	{
		ErrorFlags(ErrorFlag_ThrowException, true);
		Reset(largeIndices, buffer, startOffset, endOffset);
	}

	IndexBuffer::IndexBuffer(bool largeIndices, unsigned int length, UInt32 storage, BufferUsage usage)
	{
		ErrorFlags(ErrorFlag_ThrowException, true);
		Reset(largeIndices, length, storage, usage);
	}

	IndexBuffer::IndexBuffer(const IndexBuffer& indexBuffer) :
	RefCounted(),
	m_buffer(indexBuffer.m_buffer),
	m_largeIndices(indexBuffer.m_largeIndices),
	m_endOffset(indexBuffer.m_endOffset),
	m_indexCount(indexBuffer.m_indexCount),
	m_startOffset(indexBuffer.m_startOffset)
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

	bool IndexBuffer::Fill(const void* data, unsigned int startIndex, unsigned int length, bool forceDiscard)
	{
		unsigned int stride = GetStride();
		return FillRaw(data, startIndex*stride, length*stride, forceDiscard);
	}

	bool IndexBuffer::FillRaw(const void* data, unsigned int offset, unsigned int size, bool forceDiscard)
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

	Buffer* IndexBuffer::GetBuffer() const
	{
		return m_buffer;
	}

	unsigned int IndexBuffer::GetEndOffset() const
	{
		return m_endOffset;
	}

	unsigned int IndexBuffer::GetIndexCount() const
	{
		return m_indexCount;
	}

	unsigned int IndexBuffer::GetStride() const
	{
		return (m_largeIndices) ? sizeof(UInt32) : sizeof(UInt16);
	}

	unsigned int IndexBuffer::GetStartOffset() const
	{
		return m_startOffset;
	}

	bool IndexBuffer::HasLargeIndices() const
	{
		return m_largeIndices;
	}

	bool IndexBuffer::IsHardware() const
	{
		return m_buffer->IsHardware();
	}

	bool IndexBuffer::IsValid() const
	{
		return m_buffer;
	}

	void* IndexBuffer::Map(BufferAccess access, unsigned int startIndex, unsigned int length)
	{
		unsigned int stride = GetStride();
		return MapRaw(access, startIndex*stride, length*stride);
	}

	void* IndexBuffer::Map(BufferAccess access, unsigned int startIndex, unsigned int length) const
	{
		unsigned int stride = GetStride();
		return MapRaw(access, startIndex*stride, length*stride);
	}

	void* IndexBuffer::MapRaw(BufferAccess access, unsigned int offset, unsigned int size)
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

	void* IndexBuffer::MapRaw(BufferAccess access, unsigned int offset, unsigned int size) const
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

	void IndexBuffer::Optimize()
	{
		IndexMapper mapper(this);

		OptimizeIndices(mapper.begin(), m_indexCount);
	}

	void IndexBuffer::Reset()
	{
		m_buffer.Reset();
	}

	void IndexBuffer::Reset(bool largeIndices, Buffer* buffer)
	{
		Reset(largeIndices, buffer, 0, buffer->GetSize()-1);
	}

	void IndexBuffer::Reset(bool largeIndices, Buffer* buffer, unsigned int startOffset, unsigned int endOffset)
	{
		#if NAZARA_UTILITY_SAFE
		if (!buffer || !buffer->IsValid())
		{
			NazaraError("Buffer is invalid");
			return;
		}

		if (startOffset > endOffset)
		{
			NazaraError("Start offset cannot be over end offset");
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

		unsigned int stride = (largeIndices) ? sizeof(UInt32) : sizeof(UInt16);

		m_buffer = buffer;
		m_endOffset = endOffset;
		m_indexCount = (endOffset - startOffset) / stride;
		m_largeIndices = largeIndices;
		m_startOffset = startOffset;
	}

	void IndexBuffer::Reset(bool largeIndices, unsigned int length, UInt32 storage, BufferUsage usage)
	{
		unsigned int stride = (largeIndices) ? sizeof(UInt32) : sizeof(UInt16);

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

	bool IndexBuffer::SetStorage(UInt32 storage)
	{
		return m_buffer->SetStorage(storage);
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
