// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/VertexBuffer.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Renderer/Debug.hpp>

NzVertexBuffer::NzVertexBuffer(NzBuffer* buffer, unsigned int startVertex, unsigned int vertexCount) :
m_buffer(buffer),
m_ownsBuffer(false),
m_startVertex(startVertex),
m_vertexCount(vertexCount)
{
	m_buffer->AddResourceReference();
}

NzVertexBuffer::NzVertexBuffer(unsigned int length, nzUInt8 typeSize, nzBufferUsage usage) :
m_ownsBuffer(true),
m_startVertex(0),
m_vertexCount(length)
{
	m_buffer = new NzBuffer(nzBufferType_Vertex, length, typeSize, usage);
	m_buffer->AddResourceReference();
	m_buffer->SetPersistent(false);
}

NzVertexBuffer::NzVertexBuffer(const NzVertexBuffer& vertexBuffer) :
m_ownsBuffer(vertexBuffer.m_ownsBuffer),
m_startVertex(vertexBuffer.m_startVertex),
m_vertexCount(vertexBuffer.m_vertexCount)
{
	if (m_ownsBuffer)
	{
		m_buffer = new NzBuffer(nzBufferType_Vertex, vertexBuffer.m_buffer->GetLength(), vertexBuffer.m_buffer->GetSize(), vertexBuffer.m_buffer->GetUsage());
		m_buffer->AddResourceReference();
		m_buffer->SetPersistent(false);
		m_buffer->CopyContent(*vertexBuffer.m_buffer);
	}
	else
	{
		m_buffer = vertexBuffer.m_buffer;
		m_buffer->AddResourceReference();
	}
}

NzVertexBuffer::~NzVertexBuffer()
{
	m_buffer->RemoveResourceReference();
}

bool NzVertexBuffer::Fill(const void* data, unsigned int offset, unsigned int length)
{
	#if NAZARA_RENDERER_SAFE
	if (offset+length > m_vertexCount)
	{
		NazaraError("Exceeding virtual buffer size");
		return false;
	}
	#endif

	return m_buffer->Fill(data, m_startVertex+offset, length);
}

NzBuffer* NzVertexBuffer::GetBuffer() const
{
	return m_buffer;
}

void* NzVertexBuffer::GetBufferPtr()
{
	return reinterpret_cast<nzUInt8*>(m_buffer->GetBufferPtr()) + m_startVertex*m_buffer->GetTypeSize();
}

const void* NzVertexBuffer::GetBufferPtr() const
{
	return reinterpret_cast<const nzUInt8*>(m_buffer->GetBufferPtr()) + m_startVertex*m_buffer->GetTypeSize();
}

unsigned int NzVertexBuffer::GetStartVertex() const
{
	return m_startVertex;
}

nzUInt8 NzVertexBuffer::GetTypeSize() const
{
	return m_buffer->GetTypeSize();
}

unsigned int NzVertexBuffer::GetVertexCount() const
{
	return m_vertexCount;
}

bool NzVertexBuffer::IsHardware() const
{
	return m_buffer->IsHardware();
}

void* NzVertexBuffer::Map(nzBufferAccess access, unsigned int offset, unsigned int length)
{
	#if NAZARA_RENDERER_SAFE
	if (offset+length > m_vertexCount)
	{
		NazaraError("Exceeding virtual buffer size");
		return nullptr;
	}
	#endif

	return m_buffer->Map(access, m_startVertex+offset, (length) ? length : m_vertexCount-offset);
}

bool NzVertexBuffer::Unmap()
{
	return m_buffer->Unmap();
}
