// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/UniformBuffer.hpp>
#include <memory>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	inline const std::shared_ptr<Buffer>& UniformBuffer::GetBuffer() const
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
		return m_buffer != nullptr;
	}
}

#include <Nazara/Utility/DebugOff.hpp>
