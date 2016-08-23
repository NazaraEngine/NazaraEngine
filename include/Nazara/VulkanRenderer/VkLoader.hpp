// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VKLOADER_HPP
#define NAZARA_VULKANRENDERER_VKLOADER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <Nazara/VulkanRenderer/Config.hpp>
#include <vulkan/vulkan.h>

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
				#define NAZARA_VULKANRENDERER_GLOBAL_FUNCTION(func) static PFN_##func func

				NAZARA_VULKANRENDERER_GLOBAL_FUNCTION(vkCreateInstance);
				NAZARA_VULKANRENDERER_GLOBAL_FUNCTION(vkEnumerateInstanceExtensionProperties);
				NAZARA_VULKANRENDERER_GLOBAL_FUNCTION(vkEnumerateInstanceLayerProperties);
				NAZARA_VULKANRENDERER_GLOBAL_FUNCTION(vkGetInstanceProcAddr);

				#undef NAZARA_VULKANRENDERER_GLOBAL_FUNCTION

			private:
				static DynLib s_vulkanLib;
				static VkResult s_lastErrorCode;
		};
	}
}

#include <Nazara/VulkanRenderer/VkLoader.inl>

#endif // NAZARA_VULKANRENDERER_VKLOADER_HPP
