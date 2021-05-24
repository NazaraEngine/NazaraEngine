// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/VertexBuffer.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	VertexBuffer::VertexBuffer(std::shared_ptr<const VertexDeclaration> vertexDeclaration, std::shared_ptr<Buffer> buffer)
	{
		ErrorFlags(ErrorFlag_ThrowException, true);
		Reset(std::move(vertexDeclaration), std::move(buffer));
	}

	VertexBuffer::VertexBuffer(std::shared_ptr<const VertexDeclaration> vertexDeclaration, std::shared_ptr<Buffer> buffer, std::size_t offset, std::size_t size)
	{
		ErrorFlags(ErrorFlag_ThrowException, true);
		Reset(std::move(vertexDeclaration), std::move(buffer), offset, size);
	}

	VertexBuffer::VertexBuffer(std::shared_ptr<const VertexDeclaration> vertexDeclaration, std::size_t length, DataStorage storage, BufferUsageFlags usage)
	{
		ErrorFlags(ErrorFlag_ThrowException, true);
		Reset(std::move(vertexDeclaration), length, storage, usage);
	}

	bool VertexBuffer::Fill(const void* data, std::size_t startVertex, std::size_t length)
	{
		std::size_t stride = static_cast<std::size_t>(m_vertexDeclaration->GetStride());
		return FillRaw(data, startVertex*stride, length*stride);
	}

	bool VertexBuffer::FillRaw(const void* data, std::size_t offset, std::size_t size)
	{
		NazaraAssert(m_buffer && m_buffer->IsValid(), "Invalid buffer");
		NazaraAssert(m_startOffset + offset + size <= m_endOffset, "Exceeding virtual buffer size");

		return m_buffer->Fill(data, m_startOffset + offset, size);
	}

	void* VertexBuffer::Map(BufferAccess access, std::size_t startVertex, std::size_t length)
	{
		std::size_t stride = static_cast<std::size_t>(m_vertexDeclaration->GetStride());

		return MapRaw(access, startVertex*stride, length*stride);
	}

	void* VertexBuffer::Map(BufferAccess access, std::size_t startVertex, std::size_t length) const
	{
		NazaraAssert(m_buffer && m_buffer->IsValid(), "Invalid buffer");
		NazaraAssert(m_vertexDeclaration, "Invalid vertex declaration");

		std::size_t stride = static_cast<std::size_t>(m_vertexDeclaration->GetStride());

		return MapRaw(access, startVertex*stride, length*stride);
	}

	void* VertexBuffer::MapRaw(BufferAccess access, std::size_t offset, std::size_t size)
	{
		NazaraAssert(m_buffer && m_buffer->IsValid(), "Invalid buffer");
		NazaraAssert(m_startOffset + offset + size <= m_endOffset, "Exceeding virtual buffer size");

		return m_buffer->Map(access, offset, size);
	}

	void* VertexBuffer::MapRaw(BufferAccess access, std::size_t offset, std::size_t size) const
	{
		NazaraAssert(m_buffer && m_buffer->IsValid(), "Invalid buffer");
		NazaraAssert(m_startOffset + offset + size <= m_endOffset, "Exceeding virtual buffer size");

		return m_buffer->Map(access, offset, size);
	}

	void VertexBuffer::Reset()
	{
		m_buffer.reset();
		m_vertexDeclaration.reset();
	}

	void VertexBuffer::Reset(std::shared_ptr<const VertexDeclaration> vertexDeclaration, std::shared_ptr<Buffer> buffer)
	{
		NazaraAssert(buffer && buffer->IsValid(), "Invalid buffer");
		NazaraAssert(buffer->GetType() == BufferType::Vertex, "Buffer must be a vertex buffer");

		std::size_t size = buffer->GetSize();
		Reset(std::move(vertexDeclaration), std::move(buffer), 0, size);
	}

	void VertexBuffer::Reset(std::shared_ptr<const VertexDeclaration> vertexDeclaration, std::shared_ptr<Buffer> buffer, std::size_t offset, std::size_t size)
	{
		NazaraAssert(buffer && buffer->IsValid(), "Invalid buffer");
		NazaraAssert(size > 0, "Invalid size");
		NazaraAssert(offset + size <= buffer->GetSize(), "Virtual buffer exceed buffer bounds");

		m_buffer = buffer;
		m_endOffset = offset + size;
		m_startOffset = offset;
		m_vertexCount = (vertexDeclaration) ? (size / static_cast<std::size_t>(vertexDeclaration->GetStride())) : 0;
		m_vertexDeclaration = vertexDeclaration;
	}

	void VertexBuffer::Reset(std::shared_ptr<const VertexDeclaration> vertexDeclaration, std::size_t length, DataStorage storage, BufferUsageFlags usage)
	{
		m_endOffset = length * ((vertexDeclaration) ? static_cast<std::size_t>(vertexDeclaration->GetStride()) : 1);
		m_startOffset = 0;
		m_vertexCount = length;
		m_vertexDeclaration = std::move(vertexDeclaration);

		m_buffer = std::make_shared<Buffer>(BufferType::Vertex, m_endOffset, storage, usage);
	}

	void VertexBuffer::Reset(const VertexBuffer& vertexBuffer)
	{
		m_buffer = vertexBuffer.m_buffer;
		m_endOffset = vertexBuffer.m_endOffset;
		m_startOffset = vertexBuffer.m_startOffset;
		m_vertexCount = vertexBuffer.m_vertexCount;
		m_vertexDeclaration = vertexBuffer.m_vertexDeclaration;
	}

	void VertexBuffer::SetVertexDeclaration(std::shared_ptr<const VertexDeclaration> vertexDeclaration)
	{
		NazaraAssert(vertexDeclaration, "Invalid vertex declaration");

		m_vertexCount = (m_endOffset - m_startOffset) / static_cast<std::size_t>(vertexDeclaration->GetStride());
		m_vertexDeclaration = std::move(vertexDeclaration);
	}

	void VertexBuffer::Unmap() const
	{
		m_buffer->Unmap();
	}
}
