// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKAN_VKSURFACE_HPP
#define NAZARA_VULKAN_VKSURFACE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Vulkan/Config.hpp>
#include <Nazara/Vulkan/VkLoader.hpp>
#include <vulkan/vulkan.h>

namespace Nz 
{
	namespace Vk
	{
		class Instance;

		class Surface
		{
			public:
				inline Surface(Instance& instance);
				Surface(const Surface&) = delete;
				Surface(Surface&& surface);
				inline ~Surface();

				#ifdef VK_USE_PLATFORM_ANDROID_KHR
				// VK_KHR_android_surface
				inline bool Create(const VkAndroidSurfaceCreateInfoKHR& createInfo, const VkAllocationCallbacks* allocator = nullptr);
				inline bool Create(ANativeWindow* window, VkAndroidSurfaceCreateFlagsKHR flags = 0, const VkAllocationCallbacks* allocator = nullptr);
				#endif

				#ifdef VK_USE_PLATFORM_MIR_KHR
				// VK_KHR_mir_surface
				inline bool Create(const VkMirSurfaceCreateInfoKHR& createInfo, const VkAllocationCallbacks* allocator = nullptr);
				inline bool Create(MirConnection* connection, MirSurface* surface, VkMirSurfaceCreateFlagsKHR flags = 0, const VkAllocationCallbacks* allocator = nullptr);
				#endif

				#ifdef VK_USE_PLATFORM_XCB_KHR
				// VK_KHR_xcb_surface
				inline bool Create(const VkXcbSurfaceCreateInfoKHR& createInfo, const VkAllocationCallbacks* allocator = nullptr);
				inline bool Create(xcb_connection_t* connection, xcb_window_t window, VkXcbSurfaceCreateFlagsKHR flags = 0, const VkAllocationCallbacks* allocator = nullptr);
				#endif

				#ifdef VK_USE_PLATFORM_XLIB_KHR
				// VK_KHR_xlib_surface
				inline bool Create(const VkXlibSurfaceCreateInfoKHR& createInfo, const VkAllocationCallbacks* allocator = nullptr);
				inline bool Create(Display* display, Window window, VkXlibSurfaceCreateFlagsKHR flags = 0, const VkAllocationCallbacks* allocator = nullptr);
				#endif

				#ifdef VK_USE_PLATFORM_WAYLAND_KHR
				// VK_KHR_wayland_surface
				inline bool Create(const VkWaylandSurfaceCreateInfoKHR& createInfo, const VkAllocationCallbacks* allocator = nullptr);
				inline bool Create(wl_display* display, wl_surface* surface, VkWaylandSurfaceCreateFlagsKHR flags = 0, const VkAllocationCallbacks* allocator = nullptr);
				#endif

				#ifdef VK_USE_PLATFORM_WIN32_KHR
				// VK_KHR_win32_surface
				inline bool Create(const VkWin32SurfaceCreateInfoKHR& createInfo, const VkAllocationCallbacks* allocator = nullptr);
				inline bool Create(HINSTANCE instance, HWND handle, VkWin32SurfaceCreateFlagsKHR flags = 0, const VkAllocationCallbacks* allocator = nullptr);
				#endif

				inline void Destroy();

				bool GetCapabilities(VkPhysicalDevice physicalDevice, VkSurfaceCapabilitiesKHR* surfaceCapabilities) const;
				bool GetFormats(VkPhysicalDevice physicalDevice, std::vector<VkSurfaceFormatKHR>* surfaceFormats) const;
				bool GetPresentModes(VkPhysicalDevice physicalDevice, std::vector<VkPresentModeKHR>* presentModes) const;
				bool GetSupportPresentation(VkPhysicalDevice physicalDevice, UInt32 queueFamilyIndex, bool* supported) const;

				inline bool IsSupported() const;

				inline VkResult GetLastErrorCode() const;

				Surface& operator=(const Surface&) = delete;
				Surface& operator=(Surface&&) = delete;

				inline operator VkSurfaceKHR() const;

			private:
				inline bool Create(const VkAllocationCallbacks* allocator);

				Instance& m_instance;
				VkAllocationCallbacks m_allocator;
				VkSurfaceKHR m_surface;
				mutable VkResult m_lastErrorCode;
		};
	}
}

#include <Nazara/Vulkan/VkSurface.inl>

#endif // NAZARA_VULKAN_VKSURFACE_HPP
