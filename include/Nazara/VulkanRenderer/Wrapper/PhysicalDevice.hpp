// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VKPHYSICALDEVICE_HPP
#define NAZARA_VULKANRENDERER_VKPHYSICALDEVICE_HPP

#include <vulkan/vulkan.h>
#include <unordered_set>
#include <vector>

namespace Nz 
{
	namespace Vk
	{
		struct PhysicalDevice
		{
			VkPhysicalDevice physDevice;
			VkPhysicalDeviceFeatures features;
			VkPhysicalDeviceMemoryProperties memoryProperties;
			VkPhysicalDeviceProperties properties;
			std::unordered_set<std::string> extensions;
			std::vector<VkQueueFamilyProperties> queueFamilies;
		};
	}
}

#endif // NAZARA_VULKANRENDERER_VKPHYSICALDEVICE_HPP
