// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/Wrapper/Instance.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/VulkanRenderer/Utils.hpp>
#include <Nazara/VulkanRenderer/Wrapper/DebugUtilsMessengerEXT.hpp>
#include <sstream>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	namespace Vk
	{
		namespace
		{
			VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
				VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
				VkDebugUtilsMessageTypeFlagsEXT             messageTypes,
				const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
				void*                                       pUserData)
			{
				Instance& instance = *static_cast<Instance*>(pUserData);
				if (messageTypes & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
				{
					RenderAPIValidationLevel validationLevel = instance.GetValidationLevel();
					if ((messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) && validationLevel < RenderAPIValidationLevel::Debug)
						return VK_FALSE;

					if ((messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) && validationLevel < RenderAPIValidationLevel::Verbose)
						return VK_FALSE;

					if ((messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) && validationLevel < RenderAPIValidationLevel::Warnings)
						return VK_FALSE;
				}

				std::stringstream ss;
				ss << "Vulkan log: ";

				if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
					ss << "[Verbose]";

				if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
					ss << "[Info]";

				if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
					ss << "[Warning]";

				if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
					ss << "[Error]";


				if (messageTypes & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)
					ss << "[General]";

				if (messageTypes & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
					ss << "[Performance]";

				if (messageTypes & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
					ss << "[Validation]";

				ss << "[" << pCallbackData->messageIdNumber;
				if (pCallbackData->pMessageIdName)
					ss << ":" << pCallbackData->pMessageIdName;

				ss << "]: " << pCallbackData->pMessage;

				if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
					NazaraError(ss.str());
				else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
					NazaraWarning(ss.str());
				else
					NazaraNotice(ss.str());

				return VK_FALSE; //< Should the Vulkan call be aborted
			}
		}

		struct Instance::InternalData
		{
			DebugUtilsMessengerEXT debugMessenger;
		};

		Instance::Instance() :
		m_instance(nullptr)
		{
		}

		Instance::~Instance()
		{
			if (m_instance)
				DestroyInstance();
		}

		bool Instance::Create(RenderAPIValidationLevel validationLevel, const VkInstanceCreateInfo& createInfo, const VkAllocationCallbacks* allocator)
		{
			m_lastErrorCode = Loader::vkCreateInstance(&createInfo, allocator, &m_instance);
			if (m_lastErrorCode != VkResult::VK_SUCCESS)
			{
				NazaraError("Failed to create Vulkan instance: " + TranslateVulkanError(m_lastErrorCode));
				return false;
			}

			m_apiVersion = createInfo.pApplicationInfo->apiVersion;
			m_validationLevel = validationLevel;

			// Store the allocator to access them when needed
			if (allocator)
				m_allocator = *allocator;
			else
				m_allocator.pfnAllocation = nullptr;

			// Parse extensions and layers
			for (UInt32 i = 0; i < createInfo.enabledExtensionCount; ++i)
				m_loadedExtensions.insert(createInfo.ppEnabledExtensionNames[i]);

			for (UInt32 i = 0; i < createInfo.enabledLayerCount; ++i)
				m_loadedLayers.insert(createInfo.ppEnabledLayerNames[i]);

			// And now load everything
			try
			{
				ErrorFlags flags(ErrorMode::ThrowException, true);

#define NAZARA_VULKANRENDERER_INSTANCE_EXT_BEGIN(ext) if (IsExtensionLoaded(#ext)) {
#define NAZARA_VULKANRENDERER_INSTANCE_EXT_END() }
#define NAZARA_VULKANRENDERER_INSTANCE_FUNCTION(func) func = reinterpret_cast<PFN_##func>(GetProcAddr(#func));

#define NAZARA_VULKANRENDERER_INSTANCE_CORE_EXT_FUNCTION(func, coreVersion, suffix, extName) \
				if (m_apiVersion >= coreVersion)                                            \
					func = reinterpret_cast<PFN_##func>(GetProcAddr(#func));                 \
				else if (IsExtensionLoaded("VK_" #suffix "_" #extName))                      \
					func = reinterpret_cast<PFN_##func##suffix>(GetProcAddr(#func #suffix));

#include <Nazara/VulkanRenderer/Wrapper/InstanceFunctions.hpp>
			}
			catch (const std::exception& e)
			{
				NazaraError(std::string("Failed to query instance function: ") + e.what());
				return false;
			}

			m_internalData = std::make_unique<InternalData>();
			InstallDebugMessageCallback();

			return true;
		}

		bool Instance::EnumeratePhysicalDevices(std::vector<VkPhysicalDevice>* devices) const
		{
			NazaraAssert(devices, "Invalid device vector");

			// First, query physical device count
			UInt32 deviceCount = 0; // Remember, Nz::UInt32 is a typedef on uint32_t
			m_lastErrorCode = vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
			if (m_lastErrorCode != VkResult::VK_SUCCESS || deviceCount == 0)
			{
				NazaraError("Failed to query physical device count: " + TranslateVulkanError(m_lastErrorCode));
				return false;
			}

			// Now we can get the list of the available physical device
			devices->resize(deviceCount);
			m_lastErrorCode = vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices->data());
			if (m_lastErrorCode != VkResult::VK_SUCCESS)
			{
				NazaraError("Failed to query physical devices: " + TranslateVulkanError(m_lastErrorCode));
				return false;
			}

			return true;
		}

		bool Instance::GetPhysicalDeviceExtensions(VkPhysicalDevice device, std::vector<VkExtensionProperties>* extensionProperties) const
		{
			NazaraAssert(extensionProperties, "Invalid extension properties vector");

			// First, query extension count
			UInt32 extensionPropertyCount = 0;
			m_lastErrorCode = vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionPropertyCount, nullptr);
			if (m_lastErrorCode != VkResult::VK_SUCCESS)
			{
				NazaraError("Failed to query extension properties count: " + TranslateVulkanError(m_lastErrorCode));
				return false;
			}

			if (extensionPropertyCount == 0)
				return true; //< No extension available

			// Now we can get the list of the available extensions
			extensionProperties->resize(extensionPropertyCount);
			m_lastErrorCode = vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionPropertyCount, extensionProperties->data());
			if (m_lastErrorCode != VkResult::VK_SUCCESS)
			{
				NazaraError("Failed to query extension properties count: " + TranslateVulkanError(m_lastErrorCode));
				return false;
			}

			return true;
		}

		bool Instance::GetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice device, std::vector<VkQueueFamilyProperties>* queueFamilyProperties) const
		{
			NazaraAssert(queueFamilyProperties, "Invalid family properties vector");

			// First, query physical device count
			UInt32 queueFamiliesCount = 0; // Remember, Nz::UInt32 is a typedef on uint32_t
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamiliesCount, nullptr);
			if (queueFamiliesCount == 0)
			{
				NazaraError("Failed to query physical device count");
				return false;
			}

			// Now we can get the list of the available physical device
			queueFamilyProperties->resize(queueFamiliesCount);
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamiliesCount, queueFamilyProperties->data());

			return true;
		}

		void Instance::InstallDebugMessageCallback()
		{
			NazaraAssert(m_internalData, "Instance must be created before callbacks are installed");

			if (!Vk::DebugUtilsMessengerEXT::IsSupported(*this))
			{
				NazaraWarning(VK_EXT_DEBUG_UTILS_EXTENSION_NAME " is not supported, cannot install debug message callback");
				return;
			}

			VkDebugUtilsMessengerCreateInfoEXT callbackCreateInfo = {};
			callbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			callbackCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
			callbackCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			callbackCreateInfo.pfnUserCallback = &DebugCallback;
			callbackCreateInfo.pUserData = this;

			if (!m_internalData->debugMessenger.Create(*this, callbackCreateInfo))
			{
				NazaraWarning("failed to install debug message callback");
				return;
			}
		}

		void Instance::DestroyInstance()
		{
			assert(m_instance != VK_NULL_HANDLE);

			m_internalData.reset();

			if (vkDestroyInstance)
				vkDestroyInstance(m_instance, (m_allocator.pfnAllocation) ? &m_allocator : nullptr);
		}

		void Instance::ResetPointers()
		{
			assert(m_instance != VK_NULL_HANDLE);
			m_instance = VK_NULL_HANDLE;

#define NAZARA_VULKANRENDERER_INSTANCE_FUNCTION(func) func = nullptr;
#define NAZARA_VULKANRENDERER_INSTANCE_CORE_EXT_FUNCTION(func, ...) NAZARA_VULKANRENDERER_INSTANCE_FUNCTION(func)

#include <Nazara/VulkanRenderer/Wrapper/InstanceFunctions.hpp>
		}
	}
}

#if defined(NAZARA_PLATFORM_WINDOWS)
#include <Nazara/Core/AntiWindows.hpp>
#endif
