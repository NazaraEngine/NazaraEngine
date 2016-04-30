// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Vulkan/VkSwapchain.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Vulkan/VkDevice.hpp>
#include <Nazara/Vulkan/Debug.hpp>

namespace Nz
{
	namespace Vk
	{
		inline Swapchain::Swapchain(Device& device) :
		m_device(device),
		m_swapchain(VK_NULL_HANDLE)
		{
		}

		inline Swapchain::~Swapchain()
		{
			Destroy();
		}

		inline bool Swapchain::Create(const VkSwapchainCreateInfoKHR& createInfo, const VkAllocationCallbacks* allocator)
		{
			m_lastErrorCode = m_device.vkCreateSwapchainKHR(m_device, &createInfo, allocator, &m_swapchain);
			if (m_lastErrorCode != VkResult::VK_SUCCESS)
			{
				NazaraError("Failed to create Vulkan swapchain");
				return false;
			}

			// Store the allocator to access them when needed
			if (allocator)
				m_allocator = *allocator;
			else
				m_allocator.pfnAllocation = nullptr;

			return true;
		}

		inline void Swapchain::Destroy()
		{
			if (m_swapchain != VK_NULL_HANDLE)
				m_device.vkDestroySwapchainKHR(m_device, m_swapchain, (m_allocator.pfnAllocation) ? &m_allocator : nullptr);
		}

		inline VkResult Swapchain::GetLastErrorCode() const
		{
			return m_lastErrorCode;
		}

		inline bool Swapchain::IsSupported() const
		{
			if (!m_device.IsExtensionLoaded("VK_KHR_swapchain"))
				return false;
		}

		inline Swapchain::operator VkSwapchainKHR()
		{
			return m_swapchain;
		}
	}
}

#include <Nazara/Vulkan/DebugOff.hpp>
