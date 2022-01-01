// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/VertexBuffer.hpp>
#include <memory>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	inline const std::shared_ptr<Buffer>& VertexBuffer::GetBuffer() const
	{
		return m_buffer;
	}

	inline std::size_t VertexBuffer::GetEndOffset() const
	{
		return m_endOffset;
	}

	inline std::size_t VertexBuffer::GetStride() const
	{
		return static_cast<std::size_t>(m_vertexDeclaration->GetStride());
	}

	inline std::size_t VertexBuffer::GetStartOffset() const
	{
		return m_startOffset;
	}

	inline std::size_t VertexBuffer::GetVertexCount() const
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

#include <Nazara/Utility/DebugOff.hpp>
