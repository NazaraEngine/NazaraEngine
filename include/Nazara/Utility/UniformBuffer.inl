// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <memory>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	inline const std::shared_ptr<Buffer>& UniformBuffer::GetBuffer() const
	{
		return m_buffer;
	}

	inline UInt64 UniformBuffer::GetEndOffset() const
	{
		return m_endOffset;
	}

	inline UInt64 UniformBuffer::GetStartOffset() const
	{
		return m_startOffset;
	}
}

#include <Nazara/Utility/DebugOff.hpp>
