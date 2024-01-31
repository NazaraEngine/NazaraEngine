// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

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

	inline VkImage VulkanSwapchain::GetImage(std::size_t imageIndex) const
	{
		return m_swapchain.GetImage(SafeCast<UInt32>(imageIndex)).image;
	}

	inline const Vk::Swapchain& VulkanSwapchain::GetSwapchain() const
	{
		return m_swapchain;
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
