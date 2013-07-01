// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/VertexBuffer.hpp>
#include <Nazara/Core/Error.hpp>
#include <stdexcept>
#include <Nazara/Utility/Debug.hpp>

NzVertexBuffer::NzVertexBuffer(const NzVertexDeclaration* vertexDeclaration, NzBuffer* buffer, unsigned int startOffset, unsigned int endOffset) :
m_buffer(buffer),
m_vertexDeclaration(vertexDeclaration),
m_endOffset(endOffset),
m_startOffset(startOffset)
{
	#ifdef NAZARA_DEBUG
	if (!m_vertexDeclaration)
	{
		NazaraError("Vertex declaration is invalid");
		throw std::invalid_argument("Invalid vertex declaration");
	}

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

	m_vertexCount = (endOffset - startOffset) / m_vertexDeclaration->GetStride();
}

NzVertexBuffer::NzVertexBuffer(const NzVertexDeclaration* vertexDeclaration, unsigned int length, nzBufferStorage storage, nzBufferUsage usage) :
m_vertexDeclaration(vertexDeclaration),
m_startOffset(0),
m_vertexCount(length)
{
	#ifdef NAZARA_DEBUG
	if (!m_vertexDeclaration)
	{
		NazaraError("Vertex declaration is invalid");
		throw std::invalid_argument("Invalid vertex declaration");
	}
	#endif

	m_endOffset = length*vertexDeclaration->GetStride();

	m_buffer = new NzBuffer(nzBufferType_Vertex, m_endOffset, storage, usage);
	m_buffer->SetPersistent(false);
}

NzVertexBuffer::NzVertexBuffer(const NzVertexBuffer& vertexBuffer) :
NzResource(),
m_buffer(vertexBuffer.m_buffer),
m_vertexDeclaration(vertexBuffer.m_vertexDeclaration),
m_endOffset(vertexBuffer.m_endOffset),
m_startOffset(vertexBuffer.m_startOffset),
m_vertexCount(vertexBuffer.m_vertexCount)
{
}

bool NzVertexBuffer::Fill(const void* data, unsigned int offset, unsigned int size, bool forceDiscard)
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

bool NzVertexBuffer::FillVertices(const void* data, unsigned int startVertex, unsigned int length, bool forceDiscard)
{
	unsigned int stride = m_vertexDeclaration->GetStride();

	return Fill(data, startVertex*stride, length*stride, forceDiscard);
}

NzBuffer* NzVertexBuffer::GetBuffer() const
{
	return m_buffer;
}

unsigned int NzVertexBuffer::GetEndOffset() const
{
	return m_endOffset;
}

unsigned int NzVertexBuffer::GetStartOffset() const
{
	return m_startOffset;
}

unsigned int NzVertexBuffer::GetStride() const
{
	return m_vertexDeclaration->GetStride();
}

unsigned int NzVertexBuffer::GetVertexCount() const
{
	return m_vertexCount;
}

const NzVertexDeclaration* NzVertexBuffer::GetVertexDeclaration() const
{
	return m_vertexDeclaration;
}

bool NzVertexBuffer::IsHardware() const
{
	return m_buffer->IsHardware();
}

void* NzVertexBuffer::Map(nzBufferAccess access, unsigned int offset, unsigned int size)
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

void* NzVertexBuffer::Map(nzBufferAccess access, unsigned int offset, unsigned int size) const
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

void* NzVertexBuffer::MapVertices(nzBufferAccess access, unsigned int startVertex, unsigned int length)
{
	unsigned int stride = m_vertexDeclaration->GetStride();

	return Map(access, startVertex*stride, length*stride);
}

void* NzVertexBuffer::MapVertices(nzBufferAccess access, unsigned int startVertex, unsigned int length) const
{
	unsigned int stride = m_vertexDeclaration->GetStride();

	return Map(access, startVertex*stride, length*stride);
}

bool NzVertexBuffer::SetStorage(nzBufferStorage storage)
{
	return m_buffer->SetStorage(storage);
}

void NzVertexBuffer::SetVertexDeclaration(const NzVertexDeclaration* vertexDeclaration)
{
	#if NAZARA_UTILITY_SAFE
	if (!vertexDeclaration)
	{
		NazaraError("Vertex declaration is invalid");
		return;
	}
	#endif

	m_vertexCount = (m_endOffset - m_startOffset)/vertexDeclaration->GetStride();
	m_vertexDeclaration = vertexDeclaration;
}

void NzVertexBuffer::Unmap() const
{
	m_buffer->Unmap();
}
