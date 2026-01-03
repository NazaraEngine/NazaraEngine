// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp


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
