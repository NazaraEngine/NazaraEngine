// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	inline RenderBuffer::RenderBuffer(RenderDevice& renderDevice, BufferType type, UInt64 size, BufferUsageFlags usage) :
	Buffer(DataStorage::Hardware, type, size, usage),
	m_renderDevice(renderDevice)
	{
	}

	inline RenderDevice& RenderBuffer::GetRenderDevice()
	{
		return m_renderDevice;
	}

	inline const RenderDevice& RenderBuffer::GetRenderDevice() const
	{
		return m_renderDevice;
	}
}

#include <Nazara/Renderer/DebugOff.hpp>
