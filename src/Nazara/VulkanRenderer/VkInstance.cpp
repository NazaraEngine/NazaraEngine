// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VkInstance.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	namespace Vk
	{
		bool Instance::Create(const VkInstanceCreateInfo& createInfo, const VkAllocationCallbacks* allocator)
		{
			m_lastErrorCode = Loader::vkCreateInstance(&createInfo, allocator, &m_instance);
			if (m_lastErrorCode != VkResult::VK_SUCCESS)
			{
				NazaraError("Failed to create Vulkan instance");
				return false;
			}

			// Store the allocator to access them when needed
			if (allocator)
				m_allocator = *allocator;
			else
				m_allocator.pfnAllocation = nullptr;

			// Parse extensions and layers
			for (UInt32 i = 0; i < createInfo.enabledExtensionCount; ++i)
				m_loadedExtensions.insert(createInfo.ppEnabledExtensionNames[i]);

			for (UInt32 i = 0; i < createInfo.enabledLayerCount; ++i)
				m_loadedLayers.insert(createInfo.ppEnabledLayerNames[i]);

			// And now load everything
			#define NAZARA_VULKANRENDERER_LOAD_INSTANCE(func) func = reinterpret_cast<PFN_##func>(GetProcAddr(#func))

			try
			{
				ErrorFlags flags(ErrorFlag_ThrowException, true);

				// Vulkan core
				NAZARA_VULKANRENDERER_LOAD_INSTANCE(vkCreateDevice);
				NAZARA_VULKANRENDERER_LOAD_INSTANCE(vkDestroyInstance);
				NAZARA_VULKANRENDERER_LOAD_INSTANCE(vkEnumeratePhysicalDevices);
				NAZARA_VULKANRENDERER_LOAD_INSTANCE(vkGetDeviceProcAddr);
				NAZARA_VULKANRENDERER_LOAD_INSTANCE(vkGetPhysicalDeviceFeatures);
				NAZARA_VULKANRENDERER_LOAD_INSTANCE(vkGetPhysicalDeviceFormatProperties);
				NAZARA_VULKANRENDERER_LOAD_INSTANCE(vkGetPhysicalDeviceImageFormatProperties);
				NAZARA_VULKANRENDERER_LOAD_INSTANCE(vkGetPhysicalDeviceMemoryProperties);
				NAZARA_VULKANRENDERER_LOAD_INSTANCE(vkGetPhysicalDeviceProperties);
				NAZARA_VULKANRENDERER_LOAD_INSTANCE(vkGetPhysicalDeviceQueueFamilyProperties);

				// VK_KHR_display
				if (IsExtensionLoaded("VK_KHR_display"))
				{
					NAZARA_VULKANRENDERER_LOAD_INSTANCE(vkCreateDisplayModeKHR);
					NAZARA_VULKANRENDERER_LOAD_INSTANCE(vkCreateDisplayPlaneSurfaceKHR);
					NAZARA_VULKANRENDERER_LOAD_INSTANCE(vkGetDisplayModePropertiesKHR);
					NAZARA_VULKANRENDERER_LOAD_INSTANCE(vkGetDisplayPlaneCapabilitiesKHR);
					NAZARA_VULKANRENDERER_LOAD_INSTANCE(vkGetDisplayPlaneSupportedDisplaysKHR);
					NAZARA_VULKANRENDERER_LOAD_INSTANCE(vkGetPhysicalDeviceDisplayPlanePropertiesKHR);
					NAZARA_VULKANRENDERER_LOAD_INSTANCE(vkGetPhysicalDeviceDisplayPropertiesKHR);
				}

				// VK_KHR_surface
				if (IsExtensionLoaded("VK_KHR_surface"))
				{
					NAZARA_VULKANRENDERER_LOAD_INSTANCE(vkDestroySurfaceKHR);
					NAZARA_VULKANRENDERER_LOAD_INSTANCE(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
					NAZARA_VULKANRENDERER_LOAD_INSTANCE(vkGetPhysicalDeviceSurfaceFormatsKHR);
					NAZARA_VULKANRENDERER_LOAD_INSTANCE(vkGetPhysicalDeviceSurfacePresentModesKHR);
					NAZARA_VULKANRENDERER_LOAD_INSTANCE(vkGetPhysicalDeviceSurfaceSupportKHR);
				}

				// VK_EXT_debug_report
				if (IsExtensionLoaded("VK_EXT_debug_report"))
				{
					NAZARA_VULKANRENDERER_LOAD_INSTANCE(vkCreateDebugReportCallbackEXT);
					NAZARA_VULKANRENDERER_LOAD_INSTANCE(vkDestroyDebugReportCallbackEXT);
					NAZARA_VULKANRENDERER_LOAD_INSTANCE(vkDebugReportMessageEXT);
				}

				#ifdef VK_USE_PLATFORM_ANDROID_KHR
				// VK_KHR_android_surface
				if (IsExtensionLoaded("VK_KHR_android_surface"))
					NAZARA_VULKANRENDERER_LOAD_INSTANCE(vkCreateAndroidSurfaceKHR);
				#endif

				#ifdef VK_USE_PLATFORM_MIR_KHR
				// VK_KHR_mir_surface
				if (IsExtensionLoaded("VK_KHR_mir_surface"))
				{
					NAZARA_VULKANRENDERER_LOAD_INSTANCE(vkCreateMirSurfaceKHR);
					NAZARA_VULKANRENDERER_LOAD_INSTANCE(vkGetPhysicalDeviceMirPresentationSupportKHR);
				}
				#endif

				#ifdef VK_USE_PLATFORM_XCB_KHR
				// VK_KHR_xcb_surface
				if (IsExtensionLoaded("VK_KHR_xcb_surface"))
				{
					NAZARA_VULKANRENDERER_LOAD_INSTANCE(vkCreateXcbSurfaceKHR);
					NAZARA_VULKANRENDERER_LOAD_INSTANCE(vkGetPhysicalDeviceXcbPresentationSupportKHR);
				}
				#endif

				#ifdef VK_USE_PLATFORM_XLIB_KHR
				// VK_KHR_xlib_surface
				if (IsExtensionLoaded("VK_KHR_xlib_surface"))
				{
					NAZARA_VULKANRENDERER_LOAD_INSTANCE(vkCreateXlibSurfaceKHR);
					NAZARA_VULKANRENDERER_LOAD_INSTANCE(vkGetPhysicalDeviceXlibPresentationSupportKHR);
				}
				#endif

				#ifdef VK_USE_PLATFORM_WAYLAND_KHR
				// VK_KHR_wayland_surface
				if (IsExtensionLoaded("VK_KHR_wayland_surface"))
				{
					NAZARA_VULKANRENDERER_LOAD_INSTANCE(vkCreateWaylandSurfaceKHR);
					NAZARA_VULKANRENDERER_LOAD_INSTANCE(vkGetPhysicalDeviceWaylandPresentationSupportKHR);
				}
				#endif

				#ifdef VK_USE_PLATFORM_WIN32_KHR
				// VK_KHR_win32_surface
				if (IsExtensionLoaded("VK_KHR_win32_surface"))
				{
					NAZARA_VULKANRENDERER_LOAD_INSTANCE(vkCreateWin32SurfaceKHR);
					NAZARA_VULKANRENDERER_LOAD_INSTANCE(vkGetPhysicalDeviceWin32PresentationSupportKHR);
				}
				#endif
			}
			catch (const std::exception& e)
			{
				NazaraError(String("Failed to query instance function: ") + e.what());
				return false;
			}

			#undef NAZARA_VULKANRENDERER_LOAD_INSTANCE

			return true;
		}

		bool Instance::EnumeratePhysicalDevices(std::vector<VkPhysicalDevice>* devices)
		{
			NazaraAssert(devices, "Invalid device vector");

			// First, query physical device count
			UInt32 deviceCount = 0; // Remember, Nz::UInt32 is a typedef on uint32_t
			m_lastErrorCode = vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
			if (m_lastErrorCode != VkResult::VK_SUCCESS || deviceCount == 0)
			{
				NazaraError("Failed to query physical device count");
				return false;
			}

			// Now we can get the list of the available physical device
			devices->resize(deviceCount);
			m_lastErrorCode = vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices->data());
			if (m_lastErrorCode != VkResult::VK_SUCCESS)
			{
				NazaraError("Failed to query physical devices");
				return false;
			}

			return true;
		}

		bool Instance::GetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice device, std::vector<VkQueueFamilyProperties>* queueFamilyProperties)
		{
			NazaraAssert(queueFamilyProperties, "Invalid device vector");

			// First, query physical device count
			UInt32 queueFamiliesCount = 0; // Remember, Nz::UInt32 is a typedef on uint32_t
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamiliesCount, nullptr);
			if (queueFamiliesCount == 0)
			{
				NazaraError("Failed to query physical device count");
				return false;
			}

			// Now we can get the list of the available physical device
			queueFamilyProperties->resize(queueFamiliesCount);
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamiliesCount, queueFamilyProperties->data());

			return true;
		}

	}
}
