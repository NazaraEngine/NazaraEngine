// Copyright (C) 2017 Jérôme Leclercq
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

	inline UInt32 VertexBuffer::GetEndOffset() const
	{
		return m_endOffset;
	}

	inline UInt32 VertexBuffer::GetStride() const
	{
		return static_cast<UInt32>(m_vertexDeclaration->GetStride());
	}

	inline UInt32 VertexBuffer::GetStartOffset() const
	{
		return m_startOffset;
	}

	inline UInt32 VertexBuffer::GetVertexCount() const
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
