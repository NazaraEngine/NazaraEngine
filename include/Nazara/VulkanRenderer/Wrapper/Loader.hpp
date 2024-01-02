// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_WRAPPER_LOADER_HPP
#define NAZARA_VULKANRENDERER_WRAPPER_LOADER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <Nazara/VulkanRenderer/Config.hpp>
#include <vulkan/vulkan_core.h>
#include <vector>

namespace Nz
{
	namespace Vk
	{
		class NAZARA_VULKANRENDERER_API Loader
		{
			public:
				Loader() = delete;
				~Loader() = delete;
				
				static bool EnumerateInstanceExtensionProperties(std::vector<VkExtensionProperties>* properties, const char* layerName = nullptr);
				static bool EnumerateInstanceLayerProperties(std::vector<VkLayerProperties>* properties);

				static inline PFN_vkVoidFunction GetInstanceProcAddr(VkInstance instance, const char* name);

				static bool Initialize();
				static void Uninitialize();

				// Vulkan functions
				static PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;

#define NAZARA_VULKANRENDERER_GLOBAL_FUNCTION(func) static PFN_##func func;
#define NAZARA_VULKANRENDERER_GLOBAL_FUNCTION_OPT NAZARA_VULKANRENDERER_GLOBAL_FUNCTION

#include <Nazara/VulkanRenderer/Wrapper/GlobalFunctions.hpp>

			private:
				static DynLib s_vulkanLib;
				static VkResult s_lastErrorCode;
		};
	}
}

#include <Nazara/VulkanRenderer/Wrapper/Loader.inl>

#endif // NAZARA_VULKANRENDERER_WRAPPER_LOADER_HPP
