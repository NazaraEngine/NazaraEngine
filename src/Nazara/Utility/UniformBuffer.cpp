// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/UniformBuffer.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	UniformBuffer::UniformBuffer(BufferRef buffer)
	{
		ErrorFlags(ErrorFlag_ThrowException, true);
		Reset(std::move(buffer));
	}

	UniformBuffer::UniformBuffer(BufferRef buffer, UInt32 offset, UInt32 size)
	{
		ErrorFlags(ErrorFlag_ThrowException, true);
		Reset(std::move(buffer), offset, size);
	}

	UniformBuffer::UniformBuffer(UInt32 length, DataStorage storage, BufferUsageFlags usage)
	{
		ErrorFlags(ErrorFlag_ThrowException, true);
		Reset(length, storage, usage);
	}

	UniformBuffer::UniformBuffer(const UniformBuffer& uniformBuffer) :
	RefCounted(),
	m_buffer(uniformBuffer.m_buffer),
	m_endOffset(uniformBuffer.m_endOffset),
	m_startOffset(uniformBuffer.m_startOffset)
	{
	}

	UniformBuffer::~UniformBuffer()
	{
		OnUniformBufferRelease(this);
	}

	bool UniformBuffer::Fill(const void* data, UInt32 offset, UInt32 size)
	{
		NazaraAssert(m_buffer && m_buffer->IsValid(), "Invalid buffer");
		NazaraAssert(m_startOffset + offset + size <= m_endOffset, "Exceeding virtual buffer size");

		return m_buffer->Fill(data, m_startOffset + offset, size);
	}

	void* UniformBuffer::Map(BufferAccess access, UInt32 offset, UInt32 size)
	{
		NazaraAssert(m_buffer && m_buffer->IsValid(), "Invalid buffer");
		NazaraAssert(m_startOffset + offset + size <= m_endOffset, "Exceeding virtual buffer size");

		return m_buffer->Map(access, offset, size);
	}

	void* UniformBuffer::Map(BufferAccess access, UInt32 offset, UInt32 size) const
	{
		NazaraAssert(m_buffer && m_buffer->IsValid(), "Invalid buffer");
		NazaraAssert(m_startOffset + offset + size <= m_endOffset, "Exceeding virtual buffer size");

		return m_buffer->Map(access, offset, size);
	}

	void UniformBuffer::Reset()
	{
		m_buffer.Reset();
	}

	void UniformBuffer::Reset(BufferRef buffer)
	{
		NazaraAssert(buffer && buffer->IsValid(), "Invalid buffer");

		Reset(buffer, 0, buffer->GetSize());
	}

	void UniformBuffer::Reset(BufferRef buffer, UInt32 offset, UInt32 size)
	{
		NazaraAssert(buffer && buffer->IsValid(), "Invalid buffer");
		NazaraAssert(buffer->GetType() == BufferType_Uniform, "Buffer must be an uniform buffer");
		NazaraAssert(size > 0, "Invalid size");
		NazaraAssert(offset + size > buffer->GetSize(), "Virtual buffer exceed buffer bounds");

		m_buffer = buffer;
		m_endOffset = offset + size;
		m_startOffset = offset;
	}

	void UniformBuffer::Reset(UInt32 size, DataStorage storage, BufferUsageFlags usage)
	{
		m_endOffset = size;
		m_startOffset = 0;

		m_buffer = Buffer::New(BufferType_Uniform, m_endOffset, storage, usage);
	}

	void UniformBuffer::Reset(const UniformBuffer& UniformBuffer)
	{
		m_buffer = UniformBuffer.m_buffer;
		m_endOffset = UniformBuffer.m_endOffset;
		m_startOffset = UniformBuffer.m_startOffset;
	}

	void UniformBuffer::Unmap() const
	{
		m_buffer->Unmap();
	}

	UniformBuffer& UniformBuffer::operator=(const UniformBuffer& uniformBuffer)
	{
		Reset(uniformBuffer);

		return *this;
	}
}
