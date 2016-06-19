// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKAN_VKPHYSICALDEVICE_HPP
#define NAZARA_VULKAN_VKPHYSICALDEVICE_HPP

#include <vulkan/vulkan.h>
#include <vector>

namespace Nz 
{
	namespace Vk
	{
		struct PhysicalDevice
		{
			VkPhysicalDevice device;
			VkPhysicalDeviceFeatures features;
			VkPhysicalDeviceMemoryProperties memoryProperties;
			VkPhysicalDeviceProperties properties;
			std::vector<VkQueueFamilyProperties> queues;
		};
	}
}

#endif // NAZARA_VULKAN_VKPHYSICALDEVICE_HPP
