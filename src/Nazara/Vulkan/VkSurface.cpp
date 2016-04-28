// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Vulkan/VkSurface.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Vulkan/Debug.hpp>

namespace Nz
{
	namespace Vk
	{
		#ifdef VK_USE_PLATFORM_ANDROID_KHR
		bool Surface::Create(const VkAndroidSurfaceCreateInfoKHR& createInfo, const VkAllocationCallbacks* allocator)
		{
			m_lastErrorCode = m_instance.PFN_vkCreateAndroidSurfaceKHR(m_instance, &createInfo, allocator, &m_surface);
			return Create(allocator);
		}
		#endif

		#ifdef VK_USE_PLATFORM_MIR_KHR
		bool Surface::Create(const VkMirSurfaceCreateInfoKHR& createInfo, const VkAllocationCallbacks* allocator)
		{
			m_lastErrorCode = m_instance.PFN_vkCreateMirSurfaceKHR(m_instance, &createInfo, allocator, &m_surface);
			return Create(allocator);
		}
		#endif

		#ifdef VK_USE_PLATFORM_XCB_KHR
		bool Surface::Create(const VkXcbSurfaceCreateInfoKHR& createInfo, const VkAllocationCallbacks* allocator)
		{
			m_lastErrorCode = m_instance.PFN_vkCreateXcbSurfaceKHR(m_instance, &createInfo, allocator, &m_surface);
			return Create(allocator);
		}
		#endif

		#ifdef VK_USE_PLATFORM_XLIB_KHR
		bool Surface::Create(const VkXlibSurfaceCreateInfoKHR& createInfo, const VkAllocationCallbacks* allocator)
		{
			m_lastErrorCode = m_instance.PFN_vkCreateXlibSurfaceKHR(m_instance, &createInfo, allocator, &m_surface);
			return Create(allocator);
		}
		#endif

		#ifdef VK_USE_PLATFORM_WAYLAND_KHR
		bool Surface::Create(const VkWaylandSurfaceCreateInfoKHR& createInfo, const VkAllocationCallbacks* allocator)
		{
			m_lastErrorCode = m_instance.vkCreateWaylandSurfaceKHR(m_instance, &createInfo, allocator, &m_surface);
			return Create(allocator);
		}
		#endif

		#ifdef VK_USE_PLATFORM_WIN32_KHR
		bool Surface::Create(const VkWin32SurfaceCreateInfoKHR& createInfo, const VkAllocationCallbacks* allocator)
		{
			m_lastErrorCode = m_instance.vkCreateWin32SurfaceKHR(m_instance, &createInfo, allocator, &m_surface);
			return Create(allocator);
		}
		#endif

		bool Surface::Create(const VkAllocationCallbacks* allocator)
		{
			if (m_lastErrorCode != VkResult::VK_SUCCESS)
			{
				NazaraError("Failed to create Vulkan surface");
				return false;
			}

			// Store the allocator to access them when needed
			if (allocator)
				m_allocator = *allocator;
			else
				m_allocator.pfnAllocation = nullptr;

			return true;
		}
	}
}
