// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
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

	inline UInt32 IndexBuffer::GetIndexCount() const
	{
		return m_indexCount;
	}

	inline IndexType IndexBuffer::GetIndexType() const
	{
		return m_indexType;
	}

	inline UInt64 IndexBuffer::GetStride() const
	{
		switch (m_indexType)
		{
			case IndexType::U8:
				return sizeof(UInt8);

			case IndexType::U16:
				return sizeof(UInt16);

			case IndexType::U32:
				return sizeof(UInt32);
		}

		NazaraError("invalid index size");
		return 0;
	}

	inline UInt64 IndexBuffer::GetStartOffset() const
	{
		return m_startOffset;
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
