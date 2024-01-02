// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/IndexBuffer.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Utility/Algorithm.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/IndexIterator.hpp>
#include <Nazara/Utility/IndexMapper.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	IndexBuffer::IndexBuffer(IndexType indexType, std::shared_ptr<Buffer> buffer) :
	m_buffer(std::move(buffer)),
	m_indexType(indexType),
	m_endOffset(m_buffer->GetSize()),
	m_startOffset(0)
	{
		NazaraAssert(m_buffer, "invalid buffer");
		NazaraAssert(m_buffer->GetType() == BufferType::Index, "buffer must be an index buffer");

		m_endOffset = m_buffer->GetSize();
		m_indexCount = SafeCast<UInt32>(m_endOffset / GetStride());
	}

	IndexBuffer::IndexBuffer(IndexType indexType, std::shared_ptr<Buffer> buffer, UInt64 offset, UInt64 size) :
	m_buffer(std::move(buffer)),
	m_indexType(indexType),
	m_endOffset(offset + size),
	m_startOffset(offset)
	{
		NazaraAssert(m_buffer, "invalid buffer");
		NazaraAssert(m_buffer->GetType() == BufferType::Index, "buffer must be an index buffer");
		NazaraAssert(size > 0, "invalid buffer size");

		m_indexCount = SafeCast<UInt32>(size / GetStride());
	}

	IndexBuffer::IndexBuffer(IndexType indexType, UInt32 indexCount, BufferUsageFlags usage, const BufferFactory& bufferFactory, const void* initialData) :
	m_indexType(indexType),
	m_indexCount(indexCount),
	m_startOffset(0)
	{
		NazaraAssert(indexCount > 0, "invalid index count");

		m_endOffset = indexCount * GetStride();
		m_buffer = bufferFactory(BufferType::Index, m_endOffset, usage, initialData);
	}

	UInt64 IndexBuffer::ComputeCacheMissCount()
	{
		IndexMapper mapper(*this);

		return Nz::ComputeCacheMissCount(mapper.begin(), m_indexCount);
	}

	bool IndexBuffer::Fill(const void* data, UInt64 startIndex, UInt64 length)
	{
		UInt64 stride = GetStride();

		return FillRaw(data, startIndex*stride, length*stride);
	}

	bool IndexBuffer::FillRaw(const void* data, UInt64 offset, UInt64 size)
	{
		NazaraAssert(m_buffer, "Invalid buffer");
		NazaraAssert(m_startOffset + offset + size <= m_endOffset, "Exceeding virtual buffer size");

		return m_buffer->Fill(data, m_startOffset+offset, size);
	}

	void* IndexBuffer::MapRaw(UInt64 offset, UInt64 size)
	{
		NazaraAssert(m_buffer, "Invalid buffer");
		NazaraAssert(m_startOffset + offset + size <= m_endOffset, "Exceeding virtual buffer size");

		return m_buffer->Map(offset, size);
	}

	void* IndexBuffer::MapRaw(UInt64 offset, UInt64 size) const
	{
		NazaraAssert(m_buffer, "Invalid buffer");
		NazaraAssert(m_startOffset + offset + size <= m_endOffset, "Exceeding virtual buffer size");

		return m_buffer->Map(offset, size);
	}

	void IndexBuffer::Optimize()
	{
		IndexMapper mapper(*this);

		OptimizeIndices(mapper.begin(), m_indexCount);
	}

	void IndexBuffer::Unmap() const
	{
		m_buffer->Unmap();
	}
}
