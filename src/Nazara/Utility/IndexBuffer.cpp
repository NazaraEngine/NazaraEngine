// Copyright (C) 2020 Jérôme Leclercq
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
	IndexBuffer::IndexBuffer(bool largeIndices, std::shared_ptr<Buffer> buffer)
	{
		ErrorFlags(ErrorMode::ThrowException, true);
		Reset(largeIndices, std::move(buffer));
	}

	IndexBuffer::IndexBuffer(bool largeIndices, std::shared_ptr<Buffer> buffer, std::size_t offset, std::size_t size)
	{
		ErrorFlags(ErrorMode::ThrowException, true);
		Reset(largeIndices, std::move(buffer), offset, size);
	}

	IndexBuffer::IndexBuffer(bool largeIndices, std::size_t length, DataStorage storage, BufferUsageFlags usage)
	{
		ErrorFlags(ErrorMode::ThrowException, true);
		Reset(largeIndices, length, storage, usage);
	}

	unsigned int IndexBuffer::ComputeCacheMissCount() const
	{
		IndexMapper mapper(*this);

		return Nz::ComputeCacheMissCount(mapper.begin(), m_indexCount);
	}

	bool IndexBuffer::Fill(const void* data, std::size_t startIndex, std::size_t length)
	{
		std::size_t stride = GetStride();

		return FillRaw(data, startIndex*stride, length*stride);
	}

	bool IndexBuffer::FillRaw(const void* data, std::size_t offset, std::size_t size)
	{
		NazaraAssert(m_buffer && m_buffer->IsValid(), "Invalid buffer");
		NazaraAssert(m_startOffset + offset + size <= m_endOffset, "Exceeding virtual buffer size");

		return m_buffer->Fill(data, m_startOffset+offset, size);
	}

	void* IndexBuffer::MapRaw(BufferAccess access, std::size_t offset, std::size_t size)
	{
		NazaraAssert(m_buffer && m_buffer->IsValid(), "Invalid buffer");
		NazaraAssert(m_startOffset + offset + size <= m_endOffset, "Exceeding virtual buffer size");

		return m_buffer->Map(access, offset, size);
	}

	void* IndexBuffer::MapRaw(BufferAccess access, std::size_t offset, std::size_t size) const
	{
		NazaraAssert(m_buffer && m_buffer->IsValid(), "Invalid buffer");
		NazaraAssert(m_startOffset + offset + size <= m_endOffset, "Exceeding virtual buffer size");

		return m_buffer->Map(access, offset, size);
	}

	void IndexBuffer::Optimize()
	{
		IndexMapper mapper(*this);

		OptimizeIndices(mapper.begin(), m_indexCount);
	}

	void IndexBuffer::Reset()
	{
		m_buffer.reset();
	}

	void IndexBuffer::Reset(bool largeIndices, std::shared_ptr<Buffer> buffer)
	{
		NazaraAssert(buffer && buffer->IsValid(), "Invalid buffer");

		Reset(largeIndices, buffer, 0, buffer->GetSize());
	}

	void IndexBuffer::Reset(bool largeIndices, std::shared_ptr<Buffer> buffer, std::size_t offset, std::size_t size)
	{
		NazaraAssert(buffer && buffer->IsValid(), "Invalid buffer");
		NazaraAssert(buffer->GetType() == BufferType::Index, "Buffer must be an index buffer");
		NazaraAssert(size > 0, "Invalid size");
		NazaraAssert(offset + size > buffer->GetSize(), "Virtual buffer exceed buffer bounds");

		std::size_t stride = static_cast<std::size_t>((largeIndices) ? sizeof(UInt32) : sizeof(UInt16));

		m_buffer = buffer;
		m_endOffset = offset + size;
		m_indexCount = size / stride;
		m_largeIndices = largeIndices;
		m_startOffset = offset;
	}

	void IndexBuffer::Reset(bool largeIndices, std::size_t length, DataStorage storage, BufferUsageFlags usage)
	{
		std::size_t stride = static_cast<std::size_t>((largeIndices) ? sizeof(UInt32) : sizeof(UInt16));

		m_endOffset = length * stride;
		m_indexCount = length;
		m_largeIndices = largeIndices;
		m_startOffset = 0;

		m_buffer = std::make_shared<Buffer>(BufferType::Index, m_endOffset, storage, usage);
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
}
