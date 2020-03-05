// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <memory>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	inline const BufferRef& VertexBuffer::GetBuffer() const
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

	inline const VertexDeclarationConstRef& VertexBuffer::GetVertexDeclaration() const
	{
		return m_vertexDeclaration;
	}

	inline bool VertexBuffer::IsValid() const
	{
		return m_buffer.IsValid() && m_vertexDeclaration.IsValid();
	}

	template<typename... Args>
	VertexBufferRef VertexBuffer::New(Args&&... args)
	{
		std::unique_ptr<VertexBuffer> object(new VertexBuffer(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}
}

#include <Nazara/Utility/DebugOff.hpp>
