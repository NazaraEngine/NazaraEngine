// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanDevice.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	VulkanDevice::~VulkanDevice() = default;

	std::unique_ptr<AbstractBuffer> VulkanDevice::InstantiateBuffer(Buffer* parent, BufferType type)
	{
		return std::make_unique<VulkanBuffer>(CreateHandle(), parent, type);
	}
}
