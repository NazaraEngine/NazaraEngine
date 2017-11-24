// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanSurface.hpp>
#include <Nazara/VulkanRenderer/Vulkan.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

#if defined(VK_USE_PLATFORM_XCB_KHR)
#include <Nazara/Platform/X11/ScopedXCB.hpp>
#endif

namespace Nz
{
	VulkanSurface::VulkanSurface() :
	m_surface(Vulkan::GetInstance())
	{
	}

	VulkanSurface::~VulkanSurface() = default;

	bool VulkanSurface::Create(WindowHandle handle)
	{
		bool success = false;
		#if defined(NAZARA_PLATFORM_WINDOWS)
		{
			HWND winHandle = reinterpret_cast<HWND>(handle);
			HINSTANCE instance = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(winHandle, GWLP_HINSTANCE));

			success = m_surface.Create(instance, winHandle);
		}
		#elif defined(VK_USE_PLATFORM_XCB_KHR)
		{
			ScopedXCBConnection connection;
			success = m_surface.Create(connection, handle);
		}
		#else
		#error This OS is not supported by Vulkan
		#endif

		if (!success)
		{
			NazaraError("Failed to create Vulkan surface: " + TranslateVulkanError(m_surface.GetLastErrorCode()));
			return false;
		}

		return true;
	}

	void VulkanSurface::Destroy()
	{
		m_surface.Destroy();
	}
}
