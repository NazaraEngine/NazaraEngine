// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKAN_VKLOADER_HPP
#define NAZARA_VULKAN_VKLOADER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <Nazara/Vulkan/Config.hpp>
#include <vulkan/vulkan.h>

namespace Nz
{
	namespace Vk
	{
		class NAZARA_VULKAN_API Loader
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
				#define NAZARA_VULKAN_GLOBAL_FUNCTION(func) static PFN_##func func

				NAZARA_VULKAN_GLOBAL_FUNCTION(vkCreateInstance);
				NAZARA_VULKAN_GLOBAL_FUNCTION(vkEnumerateInstanceExtensionProperties);
				NAZARA_VULKAN_GLOBAL_FUNCTION(vkEnumerateInstanceLayerProperties);
				NAZARA_VULKAN_GLOBAL_FUNCTION(vkGetInstanceProcAddr);

				#undef NAZARA_VULKAN_GLOBAL_FUNCTION

			private:
				static DynLib s_vulkanLib;
				static VkResult s_lastErrorCode;
		};
	}
}

#include <Nazara/Vulkan/VkLoader.inl>

#endif // NAZARA_VULKAN_VKLOADER_HPP
