// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/VertexBuffer.hpp>
#include <Nazara/Core/Error.hpp>
#include <stdexcept>
#include <Nazara/Utility/Debug.hpp>

NzVertexBuffer::NzVertexBuffer(const NzVertexDeclaration* vertexDeclaration, NzBuffer* buffer)
{
	Reset(vertexDeclaration, buffer);
}

NzVertexBuffer::NzVertexBuffer(const NzVertexDeclaration* vertexDeclaration, NzBuffer* buffer, unsigned int startOffset, unsigned int endOffset)
{
	Reset(vertexDeclaration, buffer, startOffset, endOffset);
}

NzVertexBuffer::NzVertexBuffer(const NzVertexDeclaration* vertexDeclaration, unsigned int length, nzBufferStorage storage, nzBufferUsage usage)
{
	Reset(vertexDeclaration, length, storage, usage);
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

NzVertexBuffer::NzVertexBuffer(NzVertexBuffer&& vertexBuffer) noexcept :
m_buffer(std::move(vertexBuffer.m_buffer)),
m_vertexDeclaration(std::move(vertexBuffer.m_vertexDeclaration)),
m_endOffset(vertexBuffer.m_endOffset),
m_startOffset(vertexBuffer.m_startOffset),
m_vertexCount(vertexBuffer.m_vertexCount)
{
}

NzVertexBuffer::~NzVertexBuffer()
{
	NotifyDestroy();
}

bool NzVertexBuffer::Fill(const void* data, unsigned int startVertex, unsigned int length, bool forceDiscard)
{
	unsigned int stride = m_vertexDeclaration->GetStride();
	return FillRaw(data, startVertex*stride, length*stride, forceDiscard);
}

bool NzVertexBuffer::FillRaw(const void* data, unsigned int offset, unsigned int size, bool forceDiscard)
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
		return false;
	}
	#endif

	return m_buffer->Fill(data, m_startOffset+offset, size, forceDiscard);
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

bool NzVertexBuffer::IsValid() const
{
	return m_buffer && m_vertexDeclaration;
}

void* NzVertexBuffer::Map(nzBufferAccess access, unsigned int startVertex, unsigned int length)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_vertexDeclaration)
	{
		NazaraError("No vertex declaration");
		return nullptr;
	}
	#endif

	unsigned int stride = m_vertexDeclaration->GetStride();

	return MapRaw(access, startVertex*stride, length*stride);
}

void* NzVertexBuffer::Map(nzBufferAccess access, unsigned int startVertex, unsigned int length) const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_buffer)
	{
		NazaraError("No buffer");
		return nullptr;
	}

	if (!m_vertexDeclaration)
	{
		NazaraError("No vertex declaration");
		return nullptr;
	}
	#endif

	unsigned int stride = m_vertexDeclaration->GetStride();

	return MapRaw(access, startVertex*stride, length*stride);
}

void* NzVertexBuffer::MapRaw(nzBufferAccess access, unsigned int offset, unsigned int size)
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

void* NzVertexBuffer::MapRaw(nzBufferAccess access, unsigned int offset, unsigned int size) const
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

void NzVertexBuffer::Reset()
{
	m_buffer.Reset();
	m_vertexDeclaration.Reset();
}

void NzVertexBuffer::Reset(const NzVertexDeclaration* vertexDeclaration, NzBuffer* buffer)
{
	Reset(vertexDeclaration, buffer, 0, buffer->GetSize()-1);
}

void NzVertexBuffer::Reset(const NzVertexDeclaration* vertexDeclaration, NzBuffer* buffer, unsigned int startOffset, unsigned int endOffset)
{
	#if NAZARA_UTILITY_SAFE
	if (!buffer || !buffer->IsValid())
	{
		NazaraError("Invalid buffer");
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

	m_buffer = buffer;
	m_endOffset = endOffset;
	m_startOffset = startOffset;
	m_vertexCount = (vertexDeclaration) ? ((endOffset - startOffset) / vertexDeclaration->GetStride()) : 0;
	m_vertexDeclaration = vertexDeclaration;
}

void NzVertexBuffer::Reset(const NzVertexDeclaration* vertexDeclaration, unsigned int length, nzBufferStorage storage, nzBufferUsage usage)
{
	m_endOffset = length * ((vertexDeclaration) ? vertexDeclaration->GetStride() : 1);
	m_startOffset = 0;
	m_vertexCount = length;

	m_buffer = new NzBuffer(nzBufferType_Vertex, m_endOffset, storage, usage);
	m_buffer->SetPersistent(false);
	m_vertexDeclaration = vertexDeclaration;
}

void NzVertexBuffer::Reset(const NzVertexBuffer& vertexBuffer)
{
	m_buffer = vertexBuffer.m_buffer;
	m_endOffset = vertexBuffer.m_endOffset;
	m_startOffset = vertexBuffer.m_startOffset;
	m_vertexCount = vertexBuffer.m_vertexCount;
	m_vertexDeclaration = vertexBuffer.m_vertexDeclaration;
}

void NzVertexBuffer::Reset(NzVertexBuffer&& vertexBuffer) noexcept
{
	m_buffer = std::move(vertexBuffer.m_buffer);
	m_endOffset = vertexBuffer.m_endOffset;
	m_startOffset = vertexBuffer.m_startOffset;
	m_vertexCount = vertexBuffer.m_vertexCount;
	m_vertexDeclaration = std::move(vertexBuffer.m_vertexDeclaration);
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

NzVertexBuffer& NzVertexBuffer::operator=(const NzVertexBuffer& vertexBuffer)
{
	Reset(vertexBuffer);

	return *this;
}

NzVertexBuffer& NzVertexBuffer::operator=(NzVertexBuffer&& vertexBuffer) noexcept
{
	Reset(vertexBuffer);

	return *this;
}
