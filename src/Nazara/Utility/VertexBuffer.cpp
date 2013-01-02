// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/VertexBuffer.hpp>
#include <Nazara/Core/Error.hpp>
#include <stdexcept>
#include <Nazara/Utility/Debug.hpp>

///FIXME: Gérer efficacement les erreurs de création du buffer

NzVertexBuffer::NzVertexBuffer(const NzVertexDeclaration* vertexDeclaration, NzBuffer* buffer, unsigned int startVertex, unsigned int vertexCount) :
m_buffer(buffer),
m_vertexDeclaration(vertexDeclaration),
m_ownsBuffer(false),
m_startVertex(startVertex),
m_vertexCount(vertexCount)
{
	#ifdef NAZARA_DEBUG
	if (!m_buffer || !m_buffer->IsValid())
	{
		NazaraError("Buffer is invalid");
		throw std::invalid_argument("Buffer must be valid");
	}

	if (!m_vertexDeclaration || !m_vertexDeclaration->IsValid())
	{
		NazaraError("Vertex declaration is invalid");
		throw std::invalid_argument("Invalid vertex declaration");
	}
	#endif

	m_buffer->AddResourceReference();
	m_vertexDeclaration->AddResourceReference();
}

NzVertexBuffer::NzVertexBuffer(const NzVertexDeclaration* vertexDeclaration, unsigned int length, nzBufferStorage storage, nzBufferUsage usage) :
m_vertexDeclaration(vertexDeclaration),
m_ownsBuffer(true),
m_startVertex(0),
m_vertexCount(length)
{
	#ifdef NAZARA_DEBUG
	if (!m_vertexDeclaration || !m_vertexDeclaration->IsValid())
	{
		NazaraError("Vertex declaration is invalid");
		throw std::invalid_argument("Invalid vertex declaration");
	}
	#endif

	m_buffer = new NzBuffer(nzBufferType_Vertex, length, vertexDeclaration->GetStride(nzElementStream_VertexData), storage, usage);
	m_buffer->AddResourceReference();
	m_buffer->SetPersistent(false);
	m_vertexDeclaration->AddResourceReference();
}

NzVertexBuffer::NzVertexBuffer(const NzVertexBuffer& vertexBuffer) :
NzResource(true),
m_vertexDeclaration(vertexBuffer.m_vertexDeclaration),
m_ownsBuffer(vertexBuffer.m_ownsBuffer),
m_startVertex(vertexBuffer.m_startVertex),
m_vertexCount(vertexBuffer.m_vertexCount)
{
	if (m_ownsBuffer)
	{
		NzBuffer* buffer = vertexBuffer.m_buffer;

		m_buffer = new NzBuffer(nzBufferType_Vertex, buffer->GetLength(), buffer->GetSize(), buffer->GetStorage(), buffer->GetUsage());
		m_buffer->AddResourceReference();
		m_buffer->SetPersistent(false);
		m_buffer->CopyContent(*vertexBuffer.m_buffer);
	}
	else
	{
		m_buffer = vertexBuffer.m_buffer;
		m_buffer->AddResourceReference();
	}

	m_vertexDeclaration->AddResourceReference();
}

NzVertexBuffer::~NzVertexBuffer()
{
	m_buffer->RemoveResourceReference();
	m_vertexDeclaration->RemoveResourceReference();
}

bool NzVertexBuffer::Fill(const void* data, unsigned int offset, unsigned int length)
{
	#if NAZARA_UTILITY_SAFE
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

void* NzVertexBuffer::GetPointer()
{
	return reinterpret_cast<nzUInt8*>(m_buffer->GetPointer()) + m_startVertex*m_buffer->GetTypeSize();
}

const void* NzVertexBuffer::GetPointer() const
{
	return reinterpret_cast<const nzUInt8*>(m_buffer->GetPointer()) + m_startVertex*m_buffer->GetTypeSize();
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

const NzVertexDeclaration* NzVertexBuffer::GetVertexDeclaration() const
{
	return m_vertexDeclaration;
}

bool NzVertexBuffer::IsHardware() const
{
	return m_buffer->IsHardware();
}

void* NzVertexBuffer::Map(nzBufferAccess access, unsigned int offset, unsigned int length)
{
	#if NAZARA_UTILITY_SAFE
	if (offset+length > m_vertexCount)
	{
		NazaraError("Exceeding virtual buffer size");
		return nullptr;
	}
	#endif

	return m_buffer->Map(access, m_startVertex+offset, (length) ? length : m_vertexCount-offset);
}

const void* NzVertexBuffer::Map(nzBufferAccess access, unsigned int offset, unsigned int length) const
{
	#if NAZARA_UTILITY_SAFE
	if (offset+length > m_vertexCount)
	{
		NazaraError("Exceeding virtual buffer size");
		return nullptr;
	}
	#endif

	return m_buffer->Map(access, m_startVertex+offset, (length) ? length : m_vertexCount-offset);
}

bool NzVertexBuffer::SetStorage(nzBufferStorage storage)
{
	return m_buffer->SetStorage(storage);
}

void NzVertexBuffer::Unmap() const
{
	m_buffer->Unmap();
}
