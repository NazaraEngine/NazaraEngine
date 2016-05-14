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
		DeviceObject(device)
		{
		}

		inline bool Swapchain::IsSupported() const
		{
			if (!m_device.IsExtensionLoaded("VK_KHR_swapchain"))
				return false;
		}
	}
}

#include <Nazara/Vulkan/DebugOff.hpp>
