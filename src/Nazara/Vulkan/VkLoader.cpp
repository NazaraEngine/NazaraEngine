// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Vulkan/VkLoader.hpp>
#include <Nazara/Vulkan/Debug.hpp>

namespace Nz
{
	namespace Vk
	{
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
			#define NAZARA_VULKAN_LOAD_GLOBAL(func) func = reinterpret_cast<PFN_##func>(vkGetInstanceProcAddr(nullptr, #func))

			NAZARA_VULKAN_LOAD_GLOBAL(vkCreateInstance);
			NAZARA_VULKAN_LOAD_GLOBAL(vkEnumerateInstanceExtensionProperties);
			NAZARA_VULKAN_LOAD_GLOBAL(vkEnumerateInstanceLayerProperties);

			#undef NAZARA_VULKAN_LOAD_GLOBAL

			return true;
		}

		#define NAZARA_VULKAN_GLOBAL_FUNCTION_IMPL(func) PFN_##func Loader::func = nullptr

		NAZARA_VULKAN_GLOBAL_FUNCTION_IMPL(vkCreateInstance);
		NAZARA_VULKAN_GLOBAL_FUNCTION_IMPL(vkEnumerateInstanceExtensionProperties);
		NAZARA_VULKAN_GLOBAL_FUNCTION_IMPL(vkEnumerateInstanceLayerProperties);
		NAZARA_VULKAN_GLOBAL_FUNCTION_IMPL(vkGetInstanceProcAddr);

		#undef NAZARA_VULKAN_GLOBAL_FUNCTION_IMPL

		DynLib Loader::s_vulkanLib;

		void Loader::Uninitialize()
		{
			s_vulkanLib.Unload();
		}
	}
}
