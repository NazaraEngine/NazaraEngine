// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanCommandPool.hpp>
#include <stdexcept>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	inline VulkanCommandPool::VulkanCommandPool(Vk::Device& device, QueueType queueType)
	{
		UInt32 queueFamilyIndex = device.GetDefaultFamilyIndex(queueType);
		if (queueFamilyIndex == Vk::Device::InvalidQueue)
			throw std::runtime_error("QueueType " + std::to_string(UnderlyingCast(queueType)) + " is not supported");

		if (!m_commandPool.Create(device, queueFamilyIndex))
			throw std::runtime_error("Failed to create command pool: " + TranslateVulkanError(m_commandPool.GetLastErrorCode()));
	}

	inline VulkanCommandPool::VulkanCommandPool(Vk::Device& device, UInt32 queueFamilyIndex)
	{
		if (!m_commandPool.Create(device, queueFamilyIndex))
			throw std::runtime_error("Failed to create command pool: " + TranslateVulkanError(m_commandPool.GetLastErrorCode()));
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
