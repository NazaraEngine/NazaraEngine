// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/VertexBuffer.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	VertexBuffer::VertexBuffer(std::shared_ptr<const VertexDeclaration> vertexDeclaration, std::shared_ptr<Buffer> buffer) :
	m_buffer(std::move(buffer)),
	m_vertexDeclaration(std::move(vertexDeclaration)),
	m_startOffset(0)
	{
		NazaraAssert(m_buffer, "invalid buffer");
		NazaraAssert(m_buffer->GetType() == BufferType::Vertex, "buffer must be an vertex buffer");

		m_endOffset = m_buffer->GetSize();
		m_vertexCount = (m_vertexDeclaration) ? m_endOffset / m_vertexDeclaration->GetStride() : 0;
	}

	VertexBuffer::VertexBuffer(std::shared_ptr<const VertexDeclaration> vertexDeclaration, std::shared_ptr<Buffer> buffer, UInt64 offset, UInt64 size) :
	m_buffer(std::move(buffer)),
	m_vertexDeclaration(std::move(vertexDeclaration)),
	m_endOffset(size),
	m_startOffset(offset)
	{
		NazaraAssert(m_buffer, "invalid buffer");
		NazaraAssert(m_buffer->GetType() == BufferType::Vertex, "buffer must be an vertex buffer");

		m_vertexCount = (m_vertexDeclaration) ? m_endOffset / m_vertexDeclaration->GetStride() : 0;
	}

	VertexBuffer::VertexBuffer(std::shared_ptr<const VertexDeclaration> vertexDeclaration, UInt64 vertexCount, BufferUsageFlags usage, const BufferFactory& bufferFactory, const void* initialData) :
	m_vertexDeclaration(std::move(vertexDeclaration)),
	m_startOffset(0),
	m_vertexCount(vertexCount)
	{
		NazaraAssert(m_vertexDeclaration, "invalid vertex declaration");
		NazaraAssert(vertexCount > 0, "invalid vertex count");

		m_endOffset = vertexCount * m_vertexDeclaration->GetStride();
		m_buffer = bufferFactory(BufferType::Vertex, m_endOffset, usage, initialData);
	}

	bool VertexBuffer::Fill(const void* data, UInt64 startVertex, UInt64 length)
	{
		UInt64 stride = m_vertexDeclaration->GetStride();
		return FillRaw(data, startVertex*stride, length*stride);
	}

	bool VertexBuffer::FillRaw(const void* data, UInt64 offset, UInt64 size)
	{
		NazaraAssert(m_buffer, "Invalid buffer");
		NazaraAssert(m_startOffset + offset + size <= m_endOffset, "Exceeding virtual buffer size");

		return m_buffer->Fill(data, m_startOffset + offset, size);
	}

	void* VertexBuffer::Map(UInt64 startVertex, UInt64 length)
	{
		UInt64 stride = m_vertexDeclaration->GetStride();

		return MapRaw(startVertex * stride, length * stride);
	}

	void* VertexBuffer::Map(UInt64 startVertex, UInt64 length) const
	{
		NazaraAssert(m_buffer, "Invalid buffer");
		NazaraAssert(m_vertexDeclaration, "Invalid vertex declaration");

		UInt64 stride = m_vertexDeclaration->GetStride();

		return MapRaw(startVertex * stride, length * stride);
	}

	void* VertexBuffer::MapRaw(UInt64 offset, UInt64 size)
	{
		NazaraAssert(m_buffer, "Invalid buffer");
		NazaraAssert(m_startOffset + offset + size <= m_endOffset, "Exceeding virtual buffer size");

		return m_buffer->Map(offset, size);
	}

	void* VertexBuffer::MapRaw(UInt64 offset, UInt64 size) const
	{
		NazaraAssert(m_buffer, "Invalid buffer");
		NazaraAssert(m_startOffset + offset + size <= m_endOffset, "Exceeding virtual buffer size");

		return m_buffer->Map(offset, size);
	}

	void VertexBuffer::SetVertexDeclaration(std::shared_ptr<const VertexDeclaration> vertexDeclaration)
	{
		NazaraAssert(vertexDeclaration, "Invalid vertex declaration");

		m_vertexCount = (m_endOffset - m_startOffset) / vertexDeclaration->GetStride();
		m_vertexDeclaration = std::move(vertexDeclaration);
	}

	void VertexBuffer::Unmap() const
	{
		m_buffer->Unmap();
	}
}
