// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/UniformBuffer.hpp>
#include <memory>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	inline const BufferRef& UniformBuffer::GetBuffer() const
	{
		return m_buffer;
	}

	inline UInt32 UniformBuffer::GetEndOffset() const
	{
		return m_endOffset;
	}

	inline UInt32 UniformBuffer::GetStartOffset() const
	{
		return m_startOffset;
	}

	inline bool UniformBuffer::IsValid() const
	{
		return m_buffer.IsValid();
	}

	template<typename... Args>
	UniformBufferRef UniformBuffer::New(Args&&... args)
	{
		std::unique_ptr<UniformBuffer> object(new UniformBuffer(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}
}

#include <Nazara/Utility/DebugOff.hpp>
