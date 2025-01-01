// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/IndexBuffer.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/Export.hpp>
#include <Nazara/Core/IndexIterator.hpp>
#include <Nazara/Core/IndexMapper.hpp>

namespace Nz
{
	IndexBuffer::IndexBuffer(IndexType indexType, std::shared_ptr<Buffer> buffer) :
	m_buffer(std::move(buffer)),
	m_indexType(indexType),
	m_endOffset(m_buffer->GetSize()),
	m_startOffset(0)
	{
		NazaraAssertMsg(m_buffer, "invalid buffer");
		NazaraAssertMsg(m_buffer->GetType() == BufferType::Index, "buffer must be an index buffer");

		m_endOffset = m_buffer->GetSize();
		m_indexCount = SafeCast<UInt32>(m_endOffset / GetStride());
	}

	IndexBuffer::IndexBuffer(IndexType indexType, std::shared_ptr<Buffer> buffer, UInt64 offset, UInt64 size) :
	m_buffer(std::move(buffer)),
	m_indexType(indexType),
	m_endOffset(offset + size),
	m_startOffset(offset)
	{
		NazaraAssertMsg(m_buffer, "invalid buffer");
		NazaraAssertMsg(m_buffer->GetType() == BufferType::Index, "buffer must be an index buffer");
		NazaraAssertMsg(size > 0, "invalid buffer size");

		m_indexCount = SafeCast<UInt32>(size / GetStride());
	}

	IndexBuffer::IndexBuffer(IndexType indexType, UInt32 indexCount, BufferUsageFlags usage, const BufferFactory& bufferFactory, const void* initialData) :
	m_indexType(indexType),
	m_indexCount(indexCount),
	m_startOffset(0)
	{
		NazaraAssertMsg(indexCount > 0, "invalid index count");

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
		NazaraAssertMsg(m_buffer, "Invalid buffer");
		NazaraAssertMsg(m_startOffset + offset + size <= m_endOffset, "Exceeding virtual buffer size");

		return m_buffer->Fill(data, m_startOffset+offset, size);
	}

	void* IndexBuffer::MapRaw(UInt64 offset, UInt64 size)
	{
		NazaraAssertMsg(m_buffer, "Invalid buffer");
		NazaraAssertMsg(m_startOffset + offset + size <= m_endOffset, "Exceeding virtual buffer size");

		return m_buffer->Map(offset, size);
	}

	void* IndexBuffer::MapRaw(UInt64 offset, UInt64 size) const
	{
		NazaraAssertMsg(m_buffer, "Invalid buffer");
		NazaraAssertMsg(m_startOffset + offset + size <= m_endOffset, "Exceeding virtual buffer size");

		return m_buffer->Map(offset, size);
	}

	void IndexBuffer::Optimize()
	{
		IndexMapper mapper(*this);

		OptimizeIndices(mapper.begin(), m_indexCount);
	}

	void IndexBuffer::ReverseWinding()
	{
		IndexMapper mapper(*this);
		for (UInt32 i = 0; i < mapper.GetIndexCount(); ++i)
		{
			UInt32 b = mapper.Get(i * 3 + 1);
			UInt32 c = mapper.Get(i * 3 + 2);
			mapper.Set(i * 3 + 1, c);
			mapper.Set(i * 3 + 2, b);
		}
	}

	void IndexBuffer::Unmap() const
	{
		m_buffer->Unmap();
	}
}
