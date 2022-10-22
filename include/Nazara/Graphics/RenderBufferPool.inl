// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/RenderBufferPool.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline std::size_t Nz::RenderBufferPool::GetBufferAlignedSize() const
	{
		return m_bufferAlignedSize;
	}

	inline std::size_t RenderBufferPool::GetBufferPerBlock() const
	{
		return m_bufferPerBlock;
	}

	inline std::size_t RenderBufferPool::GetBufferSize() const
	{
		return m_bufferSize;
	}

	inline BufferType RenderBufferPool::GetBufferType() const
	{
		return m_bufferType;
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
