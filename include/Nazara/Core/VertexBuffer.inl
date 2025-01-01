// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <memory>

namespace Nz
{
	inline const std::shared_ptr<Buffer>& VertexBuffer::GetBuffer() const
	{
		return m_buffer;
	}

	inline UInt64 VertexBuffer::GetEndOffset() const
	{
		return m_endOffset;
	}

	inline UInt64 VertexBuffer::GetStride() const
	{
		return static_cast<UInt64>(m_vertexDeclaration->GetStride());
	}

	inline UInt64 VertexBuffer::GetStartOffset() const
	{
		return m_startOffset;
	}

	inline UInt32 VertexBuffer::GetVertexCount() const
	{
		return m_vertexCount;
	}

	inline const std::shared_ptr<const VertexDeclaration>& VertexBuffer::GetVertexDeclaration() const
	{
		return m_vertexDeclaration;
	}

	inline bool VertexBuffer::IsValid() const
	{
		return m_buffer && m_vertexDeclaration;
	}
}
