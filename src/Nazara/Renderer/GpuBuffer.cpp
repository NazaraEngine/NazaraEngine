// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Renderer/GpuBuffer.hpp>
#include <Nazara/Renderer/GpuDevice.hpp>

namespace Nz
{
	GpuBuffer::~GpuBuffer() = default;

	BufferFactory GetGpuBufferFactory(std::shared_ptr<GpuDevice> device)
	{
		return [device = std::move(device)](UInt64 size, BufferUsageFlags usage, const void* initialData) -> std::shared_ptr<Buffer>
		{
			return device->InstantiateBuffer(size, usage, initialData);
		};
	}
}
