// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	inline VulkanUploadPool::VulkanUploadPool(Vk::Device& device, UInt64 blockSize) :
	m_blockSize(blockSize),
	m_device(device),
	m_nextAllocationIndex(0)
	{
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
