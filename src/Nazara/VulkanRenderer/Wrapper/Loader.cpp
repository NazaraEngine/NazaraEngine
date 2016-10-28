// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/Wrapper/Loader.hpp>
#include <Nazara/VulkanRenderer/Utils.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	namespace Vk
	{
		bool Loader::EnumerateInstanceExtensionProperties(std::vector<VkExtensionProperties>* properties, const char* layerName)
		{
			NazaraAssert(properties, "Invalid device vector");

			// First, query physical device count
			UInt32 propertyCount = 0; // Remember, Nz::UInt32 is a typedef on uint32_t
			s_lastErrorCode = vkEnumerateInstanceExtensionProperties(layerName, &propertyCount, properties->data());
			if (s_lastErrorCode != VkResult::VK_SUCCESS)
			{
				NazaraError("Failed to get instance extension properties count: " + TranslateVulkanError(s_lastErrorCode));
				return false;
			}

			// Now we can get the list of the available physical device
			properties->resize(propertyCount);
			s_lastErrorCode = vkEnumerateInstanceExtensionProperties(layerName, &propertyCount, properties->data());
			if (s_lastErrorCode != VkResult::VK_SUCCESS)
			{
				NazaraError("Failed to enumerate instance extension properties: " + TranslateVulkanError(s_lastErrorCode));
				return false;
			}

			return true;
		}

		bool Loader::EnumerateInstanceLayerProperties(std::vector<VkLayerProperties>* properties)
		{
			NazaraAssert(properties, "Invalid device vector");

			// First, query physical device count
			UInt32 propertyCount = 0; // Remember, Nz::UInt32 is a typedef on uint32_t
			s_lastErrorCode = vkEnumerateInstanceLayerProperties(&propertyCount, properties->data());
			if (s_lastErrorCode != VkResult::VK_SUCCESS)
			{
				NazaraError("Failed to get instance layer properties count: " + TranslateVulkanError(s_lastErrorCode));
				return false;
			}

			// Now we can get the list of the available physical device
			properties->resize(propertyCount);
			s_lastErrorCode = vkEnumerateInstanceLayerProperties(&propertyCount, properties->data());
			if (s_lastErrorCode != VkResult::VK_SUCCESS)
			{
				NazaraError("Failed to enumerate instance layer properties: " + TranslateVulkanError(s_lastErrorCode));
				return false;
			}

			return true;
		}

		bool Loader::Initialize()
		{
			#ifdef NAZARA_PLATFORM_WINDOWS
			s_vulkanLib.Load("vulkan-1.dll");
			#elif defined(NAZARA_PLATFORM_LINUX)
			s_vulkanLib.Load("libvulkan.so");
			#else
			#error Unhandled platform
			#endif

			if (!s_vulkanLib.IsLoaded())
			{
				NazaraError("Failed to open vulkan library: " + s_vulkanLib.GetLastError());
				return false;
			}

			// vkGetInstanceProcAddr is the only function that's garantee to be exported
			vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(s_vulkanLib.GetSymbol("vkGetInstanceProcAddr"));
			if (!vkGetInstanceProcAddr)
			{
				NazaraError("Failed to get symbol \"vkGetInstanceProcAddr\": " + s_vulkanLib.GetLastError());
				return false;
			}

			// all other functions should be loaded using vkGetInstanceProcAddr
			#define NAZARA_VULKANRENDERER_LOAD_GLOBAL(func) func = reinterpret_cast<PFN_##func>(vkGetInstanceProcAddr(nullptr, #func))

			NAZARA_VULKANRENDERER_LOAD_GLOBAL(vkCreateInstance);
			NAZARA_VULKANRENDERER_LOAD_GLOBAL(vkEnumerateInstanceExtensionProperties);
			NAZARA_VULKANRENDERER_LOAD_GLOBAL(vkEnumerateInstanceLayerProperties);

			#undef NAZARA_VULKANRENDERER_LOAD_GLOBAL

			s_lastErrorCode = VkResult::VK_SUCCESS;

			return true;
		}

		#define NAZARA_VULKANRENDERER_GLOBAL_FUNCTION_IMPL(func) PFN_##func Loader::func = nullptr

		NAZARA_VULKANRENDERER_GLOBAL_FUNCTION_IMPL(vkCreateInstance);
		NAZARA_VULKANRENDERER_GLOBAL_FUNCTION_IMPL(vkEnumerateInstanceExtensionProperties);
		NAZARA_VULKANRENDERER_GLOBAL_FUNCTION_IMPL(vkEnumerateInstanceLayerProperties);
		NAZARA_VULKANRENDERER_GLOBAL_FUNCTION_IMPL(vkGetInstanceProcAddr);

		#undef NAZARA_VULKANRENDERER_GLOBAL_FUNCTION_IMPL

		DynLib Loader::s_vulkanLib;
		VkResult Loader::s_lastErrorCode;

		void Loader::Uninitialize()
		{
			s_vulkanLib.Unload();
		}
	}
}
