// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/VertexBuffer.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <stdexcept>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	VertexBuffer::VertexBuffer(VertexDeclarationConstRef vertexDeclaration, BufferRef buffer)
	{
		ErrorFlags(ErrorFlag_ThrowException, true);
		Reset(std::move(vertexDeclaration), std::move(buffer));
	}

	VertexBuffer::VertexBuffer(VertexDeclarationConstRef vertexDeclaration, BufferRef buffer, UInt32 offset, UInt32 size)
	{
		ErrorFlags(ErrorFlag_ThrowException, true);
		Reset(std::move(vertexDeclaration), std::move(buffer), offset, size);
	}

	VertexBuffer::VertexBuffer(VertexDeclarationConstRef vertexDeclaration, UInt32 length, DataStorage storage, BufferUsageFlags usage)
	{
		ErrorFlags(ErrorFlag_ThrowException, true);
		Reset(std::move(vertexDeclaration), length, storage, usage);
	}

	VertexBuffer::VertexBuffer(const VertexBuffer& vertexBuffer) :
	RefCounted(),
	m_buffer(vertexBuffer.m_buffer),
	m_vertexDeclaration(vertexBuffer.m_vertexDeclaration),
	m_endOffset(vertexBuffer.m_endOffset),
	m_startOffset(vertexBuffer.m_startOffset),
	m_vertexCount(vertexBuffer.m_vertexCount)
	{
	}

	VertexBuffer::~VertexBuffer()
	{
		OnVertexBufferRelease(this);
	}

	bool VertexBuffer::Fill(const void* data, UInt32 startVertex, UInt32 length)
	{
		UInt32 stride = static_cast<UInt32>(m_vertexDeclaration->GetStride());
		return FillRaw(data, startVertex*stride, length*stride);
	}

	bool VertexBuffer::FillRaw(const void* data, UInt32 offset, UInt32 size)
	{
		NazaraAssert(m_buffer && m_buffer->IsValid(), "Invalid buffer");
		NazaraAssert(m_startOffset + offset + size <= m_endOffset, "Exceeding virtual buffer size");

		return m_buffer->Fill(data, m_startOffset + offset, size);
	}

	void* VertexBuffer::Map(BufferAccess access, UInt32 startVertex, UInt32 length)
	{
		UInt32 stride = static_cast<UInt32>(m_vertexDeclaration->GetStride());

		return MapRaw(access, startVertex*stride, length*stride);
	}

	void* VertexBuffer::Map(BufferAccess access, UInt32 startVertex, UInt32 length) const
	{
		NazaraAssert(m_buffer && m_buffer->IsValid(), "Invalid buffer");
		NazaraAssert(m_vertexDeclaration, "Invalid vertex declaration");

		UInt32 stride = static_cast<UInt32>(m_vertexDeclaration->GetStride());

		return MapRaw(access, startVertex*stride, length*stride);
	}

	void* VertexBuffer::MapRaw(BufferAccess access, UInt32 offset, UInt32 size)
	{
		NazaraAssert(m_buffer && m_buffer->IsValid(), "Invalid buffer");
		NazaraAssert(m_startOffset + offset + size <= m_endOffset, "Exceeding virtual buffer size");

		return m_buffer->Map(access, offset, size);
	}

	void* VertexBuffer::MapRaw(BufferAccess access, UInt32 offset, UInt32 size) const
	{
		NazaraAssert(m_buffer && m_buffer->IsValid(), "Invalid buffer");
		NazaraAssert(m_startOffset + offset + size <= m_endOffset, "Exceeding virtual buffer size");

		return m_buffer->Map(access, offset, size);
	}

	void VertexBuffer::Reset()
	{
		m_buffer.Reset();
		m_vertexDeclaration.Reset();
	}

	void VertexBuffer::Reset(VertexDeclarationConstRef vertexDeclaration, BufferRef buffer)
	{
		NazaraAssert(buffer && buffer->IsValid(), "Invalid buffer");

		UInt32 size = buffer->GetSize();
		Reset(std::move(vertexDeclaration), std::move(buffer), 0, size);
	}

	void VertexBuffer::Reset(VertexDeclarationConstRef vertexDeclaration, BufferRef buffer, UInt32 offset, UInt32 size)
	{
		NazaraAssert(buffer && buffer->IsValid(), "Invalid buffer");
		NazaraAssert(size > 0, "Invalid size");
		NazaraAssert(offset + size <= buffer->GetSize(), "Virtual buffer exceed buffer bounds");

		m_buffer = buffer;
		m_endOffset = offset + size;
		m_startOffset = offset;
		m_vertexCount = (vertexDeclaration) ? (size / static_cast<UInt32>(vertexDeclaration->GetStride())) : 0;
		m_vertexDeclaration = vertexDeclaration;
	}

	void VertexBuffer::Reset(VertexDeclarationConstRef vertexDeclaration, UInt32 length, DataStorage storage, BufferUsageFlags usage)
	{
		m_endOffset = length * ((vertexDeclaration) ? static_cast<UInt32>(vertexDeclaration->GetStride()) : 1);
		m_startOffset = 0;
		m_vertexCount = length;
		m_vertexDeclaration = std::move(vertexDeclaration);

		m_buffer = Buffer::New(BufferType_Vertex, m_endOffset, storage, usage);
	}

	void VertexBuffer::Reset(const VertexBuffer& vertexBuffer)
	{
		m_buffer = vertexBuffer.m_buffer;
		m_endOffset = vertexBuffer.m_endOffset;
		m_startOffset = vertexBuffer.m_startOffset;
		m_vertexCount = vertexBuffer.m_vertexCount;
		m_vertexDeclaration = vertexBuffer.m_vertexDeclaration;
	}

	void VertexBuffer::SetVertexDeclaration(VertexDeclarationConstRef vertexDeclaration)
	{
		NazaraAssert(vertexDeclaration, "Invalid vertex declaration");

		m_vertexCount = (m_endOffset - m_startOffset) / static_cast<UInt32>(vertexDeclaration->GetStride());
		m_vertexDeclaration = std::move(vertexDeclaration);
	}

	void VertexBuffer::Unmap() const
	{
		m_buffer->Unmap();
	}

	VertexBuffer& VertexBuffer::operator=(const VertexBuffer& vertexBuffer)
	{
		Reset(vertexBuffer);

		return *this;
	}
}
