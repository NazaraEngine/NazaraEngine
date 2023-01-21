// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanSwapchain.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	inline VulkanDevice& VulkanSwapchain::GetDevice()
	{
		return m_device;
	}

	inline const VulkanDevice& VulkanSwapchain::GetDevice() const
	{
		return m_device;
	}

	inline Vk::QueueHandle& VulkanSwapchain::GetGraphicsQueue()
	{
		return m_graphicsQueue;
	}

	inline const Vk::Swapchain& VulkanSwapchain::GetSwapchain() const
	{
		return m_swapchain;
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
