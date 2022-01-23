// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/IndexBuffer.hpp>
#include <memory>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	inline const std::shared_ptr<Buffer>& IndexBuffer::GetBuffer() const
	{
		return m_buffer;
	}

	inline UInt64 IndexBuffer::GetEndOffset() const
	{
		return m_endOffset;
	}

	inline UInt64 IndexBuffer::GetIndexCount() const
	{
		return m_indexCount;
	}

	inline UInt64 IndexBuffer::GetStride() const
	{
		return (m_largeIndices) ? sizeof(UInt32) : sizeof(UInt16);
	}

	inline UInt64 IndexBuffer::GetStartOffset() const
	{
		return m_startOffset;
	}

	inline bool IndexBuffer::HasLargeIndices() const
	{
		return m_largeIndices;
	}

	inline bool IndexBuffer::IsValid() const
	{
		return m_buffer != nullptr;
	}

	inline void* IndexBuffer::Map(UInt64 startIndex, UInt64 length)
	{
		UInt64 stride = GetStride();
		return MapRaw(startIndex * stride, length * stride);
	}

	inline void* IndexBuffer::Map(UInt64 startIndex, UInt64 length) const
	{
		UInt64 stride = GetStride();
		return MapRaw(startIndex * stride, length * stride);
	}
}

#include <Nazara/Utility/DebugOff.hpp>
