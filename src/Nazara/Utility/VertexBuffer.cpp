// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/VertexBuffer.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <stdexcept>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	VertexBuffer::VertexBuffer(const VertexDeclaration* vertexDeclaration, Buffer* buffer)
	{
		ErrorFlags(ErrorFlag_ThrowException, true);
		Reset(vertexDeclaration, buffer);
	}

	VertexBuffer::VertexBuffer(const VertexDeclaration* vertexDeclaration, Buffer* buffer, unsigned int startOffset, unsigned int endOffset)
	{
		ErrorFlags(ErrorFlag_ThrowException, true);
		Reset(vertexDeclaration, buffer, startOffset, endOffset);
	}

	VertexBuffer::VertexBuffer(const VertexDeclaration* vertexDeclaration, unsigned int length, UInt32 storage, BufferUsage usage)
	{
		ErrorFlags(ErrorFlag_ThrowException, true);
		Reset(vertexDeclaration, length, storage, usage);
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

	bool VertexBuffer::Fill(const void* data, unsigned int startVertex, unsigned int length, bool forceDiscard)
	{
		std::size_t stride = m_vertexDeclaration->GetStride();
		return FillRaw(data, startVertex*stride, length*stride, forceDiscard);
	}

	bool VertexBuffer::FillRaw(const void* data, unsigned int offset, unsigned int size, bool forceDiscard)
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_buffer)
		{
			NazaraError("No buffer");
			return false;
		}

		if (m_startOffset + offset + size > m_endOffset)
		{
			NazaraError("Exceeding virtual buffer size");
			return false;
		}
		#endif

		return m_buffer->Fill(data, m_startOffset+offset, size, forceDiscard);
	}

	Buffer* VertexBuffer::GetBuffer() const
	{
		return m_buffer;
	}

	unsigned int VertexBuffer::GetEndOffset() const
	{
		return m_endOffset;
	}

	unsigned int VertexBuffer::GetStartOffset() const
	{
		return m_startOffset;
	}

	unsigned int VertexBuffer::GetStride() const
	{
		return m_vertexDeclaration->GetStride();
	}

	unsigned int VertexBuffer::GetVertexCount() const
	{
		return m_vertexCount;
	}

	const VertexDeclaration* VertexBuffer::GetVertexDeclaration() const
	{
		return m_vertexDeclaration;
	}

	bool VertexBuffer::IsHardware() const
	{
		return m_buffer->IsHardware();
	}

	bool VertexBuffer::IsValid() const
	{
		return m_buffer && m_vertexDeclaration;
	}

	void* VertexBuffer::Map(BufferAccess access, unsigned int startVertex, unsigned int length)
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

	void* VertexBuffer::Map(BufferAccess access, unsigned int startVertex, unsigned int length) const
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

	void* VertexBuffer::MapRaw(BufferAccess access, unsigned int offset, unsigned int size)
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

	void* VertexBuffer::MapRaw(BufferAccess access, unsigned int offset, unsigned int size) const
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

	void VertexBuffer::Reset()
	{
		m_buffer.Reset();
		m_vertexDeclaration.Reset();
	}

	void VertexBuffer::Reset(const VertexDeclaration* vertexDeclaration, Buffer* buffer)
	{
		Reset(vertexDeclaration, buffer, 0, buffer->GetSize()-1);
	}

	void VertexBuffer::Reset(const VertexDeclaration* vertexDeclaration, Buffer* buffer, unsigned int startOffset, unsigned int endOffset)
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

	void VertexBuffer::Reset(const VertexDeclaration* vertexDeclaration, unsigned int length, UInt32 storage, BufferUsage usage)
	{
		m_endOffset = length * ((vertexDeclaration) ? vertexDeclaration->GetStride() : 1);
		m_startOffset = 0;
		m_vertexCount = length;

		m_buffer = Buffer::New(BufferType_Vertex, m_endOffset, storage, usage);
		m_vertexDeclaration = vertexDeclaration;
	}

	void VertexBuffer::Reset(const VertexBuffer& vertexBuffer)
	{
		m_buffer = vertexBuffer.m_buffer;
		m_endOffset = vertexBuffer.m_endOffset;
		m_startOffset = vertexBuffer.m_startOffset;
		m_vertexCount = vertexBuffer.m_vertexCount;
		m_vertexDeclaration = vertexBuffer.m_vertexDeclaration;
	}

	bool VertexBuffer::SetStorage(UInt32 storage)
	{
		return m_buffer->SetStorage(storage);
	}

	void VertexBuffer::SetVertexDeclaration(const VertexDeclaration* vertexDeclaration)
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
