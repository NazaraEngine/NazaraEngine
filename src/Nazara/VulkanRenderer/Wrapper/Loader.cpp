// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/Wrapper/Loader.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
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
			s_lastErrorCode = vkEnumerateInstanceExtensionProperties(layerName, &propertyCount, nullptr);
			if (s_lastErrorCode != VkResult::VK_SUCCESS)
			{
				NazaraError("failed to get instance extension properties count: {0}", TranslateVulkanError(s_lastErrorCode));
				return false;
			}

			// Now we can get the list of the available physical device
			properties->resize(propertyCount);
			s_lastErrorCode = vkEnumerateInstanceExtensionProperties(layerName, &propertyCount, properties->data());
			if (s_lastErrorCode != VkResult::VK_SUCCESS)
			{
				NazaraError("failed to enumerate instance extension properties: {0}", TranslateVulkanError(s_lastErrorCode));
				return false;
			}

			return true;
		}

		bool Loader::EnumerateInstanceLayerProperties(std::vector<VkLayerProperties>* properties)
		{
			NazaraAssert(properties, "Invalid device vector");

			// First, query physical device count
			UInt32 propertyCount = 0; // Remember, Nz::UInt32 is a typedef on uint32_t
			s_lastErrorCode = vkEnumerateInstanceLayerProperties(&propertyCount, nullptr);
			if (s_lastErrorCode != VkResult::VK_SUCCESS)
			{
				NazaraError("failed to get instance layer properties count: {0}", TranslateVulkanError(s_lastErrorCode));
				return false;
			}

			// Now we can get the list of the available physical device
			properties->resize(propertyCount);
			s_lastErrorCode = vkEnumerateInstanceLayerProperties(&propertyCount, properties->data());
			if (s_lastErrorCode != VkResult::VK_SUCCESS)
			{
				NazaraError("failed to enumerate instance layer properties: {0}", TranslateVulkanError(s_lastErrorCode));
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
			#elif defined(NAZARA_PLATFORM_MACOS)
			s_vulkanLib.Load("libMoltenVK.dylib");
			#else
			#error Unhandled platform
			#endif

			if (!s_vulkanLib.IsLoaded())
			{
				NazaraError("failed to open vulkan library: {0}", s_vulkanLib.GetLastError());
				return false;
			}

			// vkGetInstanceProcAddr is the only function that's guarantee to be exported
			vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(s_vulkanLib.GetSymbol("vkGetInstanceProcAddr"));
			if (!vkGetInstanceProcAddr)
			{
				NazaraError("Failed to get symbol \"vkGetInstanceProcAddr\": {0}", s_vulkanLib.GetLastError());
				return false;
			}

			auto GetProcAddr = [&](const char* name, bool opt)
			{
				PFN_vkVoidFunction func = vkGetInstanceProcAddr(nullptr, name);
				if (!func && !opt)
					NazaraError("Failed to get {0} address", name);

				return func;
			};

			// all other functions should be loaded using vkGetInstanceProcAddr
			#define NAZARA_VULKANRENDERER_LOAD_GLOBAL(func) func = reinterpret_cast<PFN_##func>(GetProcAddr(#func))
			try
			{
				ErrorFlags flags(ErrorMode::ThrowException);

#define NAZARA_VULKANRENDERER_GLOBAL_FUNCTION(func) func = reinterpret_cast<PFN_##func>(GetProcAddr(#func, false));
#define NAZARA_VULKANRENDERER_GLOBAL_FUNCTION_OPT(func) func = reinterpret_cast<PFN_##func>(GetProcAddr(#func, true));

#include <Nazara/VulkanRenderer/Wrapper/GlobalFunctions.hpp>

			}
			catch (const std::exception& e)
			{
				NazaraError("Failed to query device function: {0}", e.what());
				return false;
			}

			s_lastErrorCode = VkResult::VK_SUCCESS;

			return true;
		}

		PFN_vkGetInstanceProcAddr Loader::vkGetInstanceProcAddr = nullptr;

#define NAZARA_VULKANRENDERER_GLOBAL_FUNCTION(func) PFN_##func Loader::func = nullptr;
#define NAZARA_VULKANRENDERER_GLOBAL_FUNCTION_OPT NAZARA_VULKANRENDERER_GLOBAL_FUNCTION

#include <Nazara/VulkanRenderer/Wrapper/GlobalFunctions.hpp>

		DynLib Loader::s_vulkanLib;
		VkResult Loader::s_lastErrorCode;

		void Loader::Uninitialize()
		{
			s_vulkanLib.Unload();
		}
	}
}
