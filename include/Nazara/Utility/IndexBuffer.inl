// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <memory>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	inline const BufferRef& IndexBuffer::GetBuffer() const
	{
		return m_buffer;
	}

	inline std::size_t IndexBuffer::GetEndOffset() const
	{
		return m_endOffset;
	}

	inline std::size_t IndexBuffer::GetIndexCount() const
	{
		return m_indexCount;
	}

	inline std::size_t IndexBuffer::GetStride() const
	{
		return static_cast<std::size_t>((m_largeIndices) ? sizeof(std::size_t) : sizeof(UInt16));
	}

	inline std::size_t IndexBuffer::GetStartOffset() const
	{
		return m_startOffset;
	}

	inline bool IndexBuffer::HasLargeIndices() const
	{
		return m_largeIndices;
	}

	inline bool IndexBuffer::IsValid() const
	{
		return m_buffer.IsValid();
	}

	inline void* IndexBuffer::Map(BufferAccess access, std::size_t startIndex, std::size_t length)
	{
		std::size_t stride = GetStride();
		return MapRaw(access, startIndex*stride, length*stride);
	}

	inline void* IndexBuffer::Map(BufferAccess access, std::size_t startIndex, std::size_t length) const
	{
		std::size_t stride = GetStride();
		return MapRaw(access, startIndex*stride, length*stride);
	}

	template<typename... Args>
	IndexBufferRef IndexBuffer::New(Args&&... args)
	{
		std::unique_ptr<IndexBuffer> object(new IndexBuffer(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}
}

#include <Nazara/Utility/DebugOff.hpp>
