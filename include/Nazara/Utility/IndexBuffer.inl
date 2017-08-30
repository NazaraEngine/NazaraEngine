// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/IndexBuffer.hpp>
#include <memory>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	inline const BufferRef& IndexBuffer::GetBuffer() const
	{
		return m_buffer;
	}

	inline UInt32 IndexBuffer::GetEndOffset() const
	{
		return m_endOffset;
	}

	inline UInt32 IndexBuffer::GetIndexCount() const
	{
		return m_indexCount;
	}

	inline DataStorage IndexBuffer::GetStorage() const
	{
		return DataStorage();
	}

	inline UInt32 IndexBuffer::GetStride() const
	{
		return static_cast<UInt32>((m_largeIndices) ? sizeof(UInt32) : sizeof(UInt16));
	}

	inline UInt32 IndexBuffer::GetStartOffset() const
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

	inline void* IndexBuffer::Map(BufferAccess access, UInt32 startIndex, UInt32 length)
	{
		UInt32 stride = GetStride();
		return MapRaw(access, startIndex*stride, length*stride);
	}

	inline void* IndexBuffer::Map(BufferAccess access, UInt32 startIndex, UInt32 length) const
	{
		UInt32 stride = GetStride();
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
