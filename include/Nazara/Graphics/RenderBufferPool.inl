// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/RenderBufferPool.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline UInt64 Nz::RenderBufferPool::GetBufferAlignedSize() const
	{
		return m_bufferAlignedSize;
	}

	inline UInt64 RenderBufferPool::GetBufferPerBlock() const
	{
		return m_bufferPerBlock;
	}

	inline UInt64 RenderBufferPool::GetBufferSize() const
	{
		return m_bufferSize;
	}

	inline BufferType RenderBufferPool::GetBufferType() const
	{
		return m_bufferType;
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
