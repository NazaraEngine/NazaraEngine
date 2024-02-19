// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/VulkanRenderer/Wrapper/Loader.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/VulkanRenderer/Utils.hpp>
#include <NazaraUtils/PathUtils.hpp>
#include <array>

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
				NazaraErrorFmt("failed to get instance extension properties count: {0}", TranslateVulkanError(s_lastErrorCode));
				return false;
			}

			// Now we can get the list of the available physical device
			properties->resize(propertyCount);
			s_lastErrorCode = vkEnumerateInstanceExtensionProperties(layerName, &propertyCount, properties->data());
			if (s_lastErrorCode != VkResult::VK_SUCCESS)
			{
				NazaraErrorFmt("failed to enumerate instance extension properties: {0}", TranslateVulkanError(s_lastErrorCode));
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
				NazaraErrorFmt("failed to get instance layer properties count: {0}", TranslateVulkanError(s_lastErrorCode));
				return false;
			}

			// Now we can get the list of the available physical device
			properties->resize(propertyCount);
			s_lastErrorCode = vkEnumerateInstanceLayerProperties(&propertyCount, properties->data());
			if (s_lastErrorCode != VkResult::VK_SUCCESS)
			{
				NazaraErrorFmt("failed to enumerate instance layer properties: {0}", TranslateVulkanError(s_lastErrorCode));
				return false;
			}

			return true;
		}

		bool Loader::Initialize()
		{
#if defined(NAZARA_PLATFORM_WINDOWS)
			std::array libs{
				"vulkan-1.dll"
			};
#elif defined(NAZARA_PLATFORM_LINUX) || defined(NAZARA_PLATFORM_ANDROID)
			std::array libs{
				"libvulkan.so.1",
				"libvulkan.so"
			};
#elif defined(NAZARA_PLATFORM_MACOS) || defined(NAZARA_PLATFORM_IOS)
			std::array libs{
				"libvulkan.dylib",
				"libvulkan.1.dylib",
				"libMoltenVK.dylib",
			};
#else
			NazaraError("unhandled OS");
			return false;
#endif

			for (const char* libname : libs)
			{
				ErrorFlags errorFlags(ErrorMode::Silent, ErrorMode::ThrowException);

				if (!s_vulkanLib.Load(Utf8Path(libname)))
					continue;

				Error::ApplyFlags({}, ErrorMode::Silent);

				// vkGetInstanceProcAddr is the only function that's guarantee to be exported
				vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(s_vulkanLib.GetSymbol("vkGetInstanceProcAddr"));
				if (!vkGetInstanceProcAddr)
				{
					NazaraErrorFmt("Failed to get symbol \"vkGetInstanceProcAddr\": {0}", s_vulkanLib.GetLastError());
					continue;
				}

				auto GetProcAddr = [&](const char* name, bool opt)
				{
					PFN_vkVoidFunction func = vkGetInstanceProcAddr(nullptr, name);
					if (!func && !opt)
						NazaraErrorFmt("Failed to get {0} address", name);

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
					NazaraErrorFmt("Failed to query device function: {0}", e.what());
					return false;
				}
			}

			if (!s_vulkanLib.IsLoaded())
			{
				NazaraError("failed to open vulkan library");
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
