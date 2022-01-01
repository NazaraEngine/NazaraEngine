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
		return static_cast<std::size_t>((m_largeIndices) ? sizeof(UInt32) : sizeof(UInt16));
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
		return m_buffer != nullptr;
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
}

#include <Nazara/Utility/DebugOff.hpp>
