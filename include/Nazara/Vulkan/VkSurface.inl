// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Vulkan/VkSurface.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Vulkan/VkInstance.hpp>
#include <Nazara/Vulkan/Debug.hpp>

namespace Nz
{
	namespace Vk
	{
		inline Surface::Surface(Instance& instance) :
		m_instance(instance),
		m_surface(VK_NULL_HANDLE)
		{
		}

		inline Surface::Surface(Surface&& surface) :
		m_instance(surface.m_instance),
		m_allocator(surface.m_allocator),
		m_surface(surface.m_surface),
		m_lastErrorCode(surface.m_lastErrorCode)
		{
			surface.m_surface = VK_NULL_HANDLE;
		}

		inline Surface::~Surface()
		{
			Destroy();
		}

		#ifdef VK_USE_PLATFORM_ANDROID_KHR
		inline bool Surface::Create(const VkAndroidSurfaceCreateInfoKHR& createInfo, const VkAllocationCallbacks* allocator)
		{
			m_lastErrorCode = m_instance.vkCreateAndroidSurfaceKHR(m_instance, &createInfo, allocator, &m_surface);
			return Create(allocator);
		}

		inline bool Surface::Create(ANativeWindow* window, VkAndroidSurfaceCreateFlagsKHR flags, const VkAllocationCallbacks* allocator)
		{
			VkAndroidSurfaceCreateInfoKHR createInfo = 
			{
				VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR,
				nullptr,
				flags,
				window
			};

			return Create(createInfo, allocator);
		}
		#endif

		#ifdef VK_USE_PLATFORM_MIR_KHR
		inline bool Surface::Create(const VkMirSurfaceCreateInfoKHR& createInfo, const VkAllocationCallbacks* allocator)
		{
			m_lastErrorCode = m_instance.vkCreateMirSurfaceKHR(m_instance, &createInfo, allocator, &m_surface);
			return Create(allocator);
		}

		inline bool Surface::Create(MirConnection* connection, MirSurface* surface, VkMirSurfaceCreateFlagsKHR flags, const VkAllocationCallbacks* allocator)
		{
			VkMirSurfaceCreateInfoKHR createInfo =
			{
				VK_STRUCTURE_TYPE_MIR_SURFACE_CREATE_INFO_KHR,
				nullptr,
				flags,
				connection,
				surface
			};

			return Create(createInfo, allocator);
		}
		#endif

		#ifdef VK_USE_PLATFORM_XCB_KHR
		inline bool Surface::Create(const VkXcbSurfaceCreateInfoKHR& createInfo, const VkAllocationCallbacks* allocator)
		{
			m_lastErrorCode = m_instance.vkCreateXcbSurfaceKHR(m_instance, &createInfo, allocator, &m_surface);
			return Create(allocator);
		}

		inline bool Surface::Create(xcb_connection_t* connection, xcb_window_t window, VkXcbSurfaceCreateFlagsKHR flags, const VkAllocationCallbacks* allocator)
		{
			VkXcbSurfaceCreateInfoKHR createInfo =
			{
				VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR,
				nullptr,
				flags,
				connection,
				window
			};

			return Create(createInfo, allocator);
		}
		#endif

		#ifdef VK_USE_PLATFORM_XLIB_KHR
		inline bool Surface::Create(const VkXlibSurfaceCreateInfoKHR& createInfo, const VkAllocationCallbacks* allocator)
		{
			m_lastErrorCode = m_instance.vkCreateXlibSurfaceKHR(m_instance, &createInfo, allocator, &m_surface);
			return Create(allocator);
		}

		inline bool Surface::Create(Display* display, Window window, VkXlibSurfaceCreateFlagsKHR flags, const VkAllocationCallbacks* allocator)
		{
			VkXlibSurfaceCreateInfoKHR createInfo =
			{
				VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR,
				nullptr,
				flags,
				display,
				window
			};

			return Create(createInfo, allocator);
		}
		#endif

		#ifdef VK_USE_PLATFORM_WAYLAND_KHR
		inline bool Surface::Create(const VkWaylandSurfaceCreateInfoKHR& createInfo, const VkAllocationCallbacks* allocator)
		{
			m_lastErrorCode = m_instance.vkCreateWaylandSurfaceKHR(m_instance, &createInfo, allocator, &m_surface);
			return Create(allocator);
		}

		inline bool Surface::Create(wl_display* display, wl_surface* surface, VkWaylandSurfaceCreateFlagsKHR flags, const VkAllocationCallbacks* allocator)
		{
			VkWaylandSurfaceCreateInfoKHR createInfo =
			{
				VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR,
				nullptr,
				flags,
				display,
				surface
			};

			return Create(createInfo, allocator);
		}
		#endif

		#ifdef VK_USE_PLATFORM_WIN32_KHR
		inline bool Surface::Create(const VkWin32SurfaceCreateInfoKHR& createInfo, const VkAllocationCallbacks* allocator)
		{
			m_lastErrorCode = m_instance.vkCreateWin32SurfaceKHR(m_instance, &createInfo, allocator, &m_surface);
			return Create(allocator);
		}

		inline bool Surface::Create(HINSTANCE instance, HWND handle, VkWin32SurfaceCreateFlagsKHR flags, const VkAllocationCallbacks* allocator)
		{
			VkWin32SurfaceCreateInfoKHR createInfo =
			{
				VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
				nullptr,
				flags,
				instance,
				handle
			};

			return Create(createInfo, allocator);
		}
		#endif

		inline void Surface::Destroy()
		{
			if (m_surface != VK_NULL_HANDLE)
			{
				m_instance.vkDestroySurfaceKHR(m_instance, m_surface, (m_allocator.pfnAllocation) ? &m_allocator : nullptr);
				m_surface = VK_NULL_HANDLE;
			}
		}
		
		inline VkResult Surface::GetLastErrorCode() const
		{
			return m_lastErrorCode;
		}

		inline bool Surface::GetCapabilities(VkPhysicalDevice physicalDevice, VkSurfaceCapabilitiesKHR* surfaceCapabilities) const
		{
			m_lastErrorCode = m_instance.vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, m_surface, surfaceCapabilities);
			if (m_lastErrorCode != VkResult::VK_SUCCESS)
			{
				NazaraError("Failed to query surface capabilities");
				return false;
			}

			return true;
		}

		inline bool Surface::GetFormats(VkPhysicalDevice physicalDevice, std::vector<VkSurfaceFormatKHR>* surfaceFormats) const
		{
			// First, query format count
			UInt32 surfaceCount = 0; // Remember, Nz::UInt32 is a typedef on uint32_t
			m_lastErrorCode = m_instance.vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_surface, &surfaceCount, nullptr);
			if (m_lastErrorCode != VkResult::VK_SUCCESS || surfaceCount == 0)
			{
				NazaraError("Failed to query format count");
				return false;
			}

			// Now we can get the list of the available physical device
			surfaceFormats->resize(surfaceCount);
			m_lastErrorCode = m_instance.vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_surface, &surfaceCount, surfaceFormats->data());
			if (m_lastErrorCode != VkResult::VK_SUCCESS)
			{
				NazaraError("Failed to query formats");
				return false;
			}

			return true;
		}

		inline bool Surface::GetPresentModes(VkPhysicalDevice physicalDevice, std::vector<VkPresentModeKHR>* presentModes) const
		{
			// First, query present modes count
			UInt32 presentModeCount = 0; // Remember, Nz::UInt32 is a typedef on uint32_t
			m_lastErrorCode = m_instance.vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_surface, &presentModeCount, nullptr);
			if (m_lastErrorCode != VkResult::VK_SUCCESS || presentModeCount == 0)
			{
				NazaraError("Failed to query present mode count");
				return false;
			}

			// Now we can get the list of the available physical device
			presentModes->resize(presentModeCount);
			m_lastErrorCode = m_instance.vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_surface, &presentModeCount, presentModes->data());
			if (m_lastErrorCode != VkResult::VK_SUCCESS)
			{
				NazaraError("Failed to query present modes");
				return false;
			}

			return true;
		}

		inline bool Surface::GetSupportPresentation(VkPhysicalDevice physicalDevice, UInt32 queueFamilyIndex, bool* supported) const
		{
			VkBool32 presentationSupported = VK_FALSE;
			m_lastErrorCode = m_instance.vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueFamilyIndex, m_surface, &presentationSupported);
			if (m_lastErrorCode != VkResult::VK_SUCCESS)
			{
				NazaraError("Failed to query surface capabilities");
				return false;
			}

			*supported = (presentationSupported == VK_TRUE);

			return true;
		}

		inline bool Surface::IsSupported() const
		{
			if (!m_instance.IsExtensionLoaded("VK_KHR_surface"))
				return false;

			#ifdef VK_USE_PLATFORM_ANDROID_KHR
			if (m_instance.IsExtensionLoaded("VK_KHR_android_surface"))
				return true;
			#endif

			#ifdef VK_USE_PLATFORM_MIR_KHR
			if (m_instance.IsExtensionLoaded("VK_KHR_mir_surface"))
				return true;
			#endif

			#ifdef VK_USE_PLATFORM_XCB_KHR
			if (m_instance.IsExtensionLoaded("VK_KHR_xcb_surface"))
				return true;
			#endif

			#ifdef VK_USE_PLATFORM_XLIB_KHR
			if (m_instance.IsExtensionLoaded("VK_KHR_xlib_surface"))
				return true;
			#endif

			#ifdef VK_USE_PLATFORM_WAYLAND_KHR
			if (m_instance.IsExtensionLoaded("VK_KHR_wayland_surface"))
				return true;
			#endif

			#ifdef VK_USE_PLATFORM_WIN32_KHR
			if (m_instance.IsExtensionLoaded("VK_KHR_win32_surface"))
				return true;
			#endif

			return false;
		}

		inline Surface::operator VkSurfaceKHR()
		{
			return m_surface;
		}

		inline bool Surface::Create(const VkAllocationCallbacks* allocator)
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

#include <Nazara/Vulkan/DebugOff.hpp>
