// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/RenderBuffer.hpp>
#include <Nazara/Renderer/RenderDevice.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	RenderBuffer::~RenderBuffer() = default;

	BufferFactory GetRenderBufferFactory(std::shared_ptr<RenderDevice> device)
	{
		return [device = std::move(device)](BufferType type, UInt64 size, BufferUsageFlags usage, const void* initialData) -> std::shared_ptr<Buffer>
		{
			return device->InstantiateBuffer(type, size, usage, initialData);
		};
	}
}
