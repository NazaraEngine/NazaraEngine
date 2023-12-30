// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_WRAPPER_PHYSICALDEVICE_HPP
#define NAZARA_VULKANRENDERER_WRAPPER_PHYSICALDEVICE_HPP

#include <NazaraUtils/StringHash.hpp>
#include <vulkan/vulkan_core.h>
#include <string>
#include <unordered_set>
#include <vector>

namespace Nz::Vk
{
	struct PhysicalDevice
	{
		VkPhysicalDevice physDevice;
		VkPhysicalDeviceFeatures features;
		VkPhysicalDeviceMemoryProperties memoryProperties;
		VkPhysicalDeviceProperties properties;
		std::unordered_set<std::string, StringHash<>, std::equal_to<>> extensions;
		std::vector<VkQueueFamilyProperties> queueFamilies;
	};
}

#endif // NAZARA_VULKANRENDERER_WRAPPER_PHYSICALDEVICE_HPP
