// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/Wrapper/Instance.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/VulkanRenderer/Utils.hpp>
#include <Nazara/VulkanRenderer/Wrapper/DebugReportCallbackEXT.hpp>
#include <Nazara/VulkanRenderer/Wrapper/DebugUtilsMessengerEXT.hpp>
#include <sstream>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz::Vk
{
	namespace
	{
		VKAPI_ATTR VkBool32 VKAPI_CALL DebugMessengerCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT             messageTypes,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void*                                       /*pUserData*/)
		{

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

		VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(
			VkDebugReportFlagsEXT flags,
			VkDebugReportObjectTypeEXT /*objectType*/,
			uint64_t /*object*/,
			size_t /*location*/,
			int32_t messageCode,
			const char* pLayerPrefix,
			const char* pMessage,
			void* /*pUserData*/)
		{
			std::stringstream ss;
			ss << "Vulkan log: ";

			if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
				ss << "[Info]";

			if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
				ss << "[Warning]";

			if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
				ss << "[Error]";

			if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
				ss << "[Performance]";

			if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
				ss << "[Debug]";

			ss << "[" << messageCode << "]";
			if (pLayerPrefix)
				ss << "[layer " << pLayerPrefix << "]";

			ss << ": " << pMessage;

			if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
				NazaraError(ss.str());
			else if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
				NazaraWarning(ss.str());
			else
				NazaraNotice(ss.str());

			return VK_FALSE; //< Should the Vulkan call be aborted
		}
	}

	struct Instance::InternalData
	{
		DebugReportCallbackEXT debugCallback;
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
			NazaraErrorFmt("failed to create Vulkan instance: {0}", TranslateVulkanError(m_lastErrorCode));
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
			ErrorFlags flags(ErrorMode::ThrowException);

#define NAZARA_VULKANRENDERER_INSTANCE_EXT_BEGIN(ext) if (IsExtensionLoaded(#ext)) { \
			NazaraDebug(#ext " extension is supported");
#define NAZARA_VULKANRENDERER_INSTANCE_EXT_END() }
#define NAZARA_VULKANRENDERER_INSTANCE_FUNCTION(func) func = reinterpret_cast<PFN_##func>(GetProcAddr(#func)); \
			if (!func) \
				NazaraWarning("failed to get a function pointer for " #func " despite its being reported as supported");

#define NAZARA_VULKANRENDERER_INSTANCE_CORE_EXT_FUNCTION(func, coreVersion, suffix, extName) \
			if (m_apiVersion >= coreVersion)                                            \
				func = reinterpret_cast<PFN_##func>(GetProcAddr(#func));                 \
			else if (IsExtensionLoaded("VK_" #suffix "_" #extName))                      \
				func = reinterpret_cast<PFN_##func##suffix>(GetProcAddr(#func #suffix));

#include <Nazara/VulkanRenderer/Wrapper/InstanceFunctions.hpp>
		}
		catch (const std::exception& e)
		{
			NazaraErrorFmt("Failed to query instance function: {0}", e.what());
			return false;
		}

		m_internalData = std::make_unique<InternalData>();
		InstallDebugMessageCallback(validationLevel);

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
			NazaraErrorFmt("failed to query physical device count: {0}", TranslateVulkanError(m_lastErrorCode));
			return false;
		}

		// Now we can get the list of the available physical device
		devices->resize(deviceCount);
		m_lastErrorCode = vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices->data());
		if (m_lastErrorCode != VkResult::VK_SUCCESS)
		{
			NazaraErrorFmt("failed to query physical devices: {0}", TranslateVulkanError(m_lastErrorCode));
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
			NazaraErrorFmt("failed to query extension properties count: {0}", TranslateVulkanError(m_lastErrorCode));
			return false;
		}

		if (extensionPropertyCount == 0)
			return true; //< No extension available

		// Now we can get the list of the available extensions
		extensionProperties->resize(extensionPropertyCount);
		m_lastErrorCode = vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionPropertyCount, extensionProperties->data());
		if (m_lastErrorCode != VkResult::VK_SUCCESS)
		{
			NazaraErrorFmt("failed to query extension properties count: {0}", TranslateVulkanError(m_lastErrorCode));
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
			NazaraError("failed to query physical device count");
			return false;
		}

		// Now we can get the list of the available physical device
		queueFamilyProperties->resize(queueFamiliesCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamiliesCount, queueFamilyProperties->data());

		return true;
	}

	void Instance::InstallDebugMessageCallback(RenderAPIValidationLevel validationLevel)
	{
		NazaraAssert(m_internalData, "Instance must be created before callbacks are installed");

		if (Vk::DebugUtilsMessengerEXT::IsSupported(*this))
		{
			VkDebugUtilsMessengerCreateInfoEXT callbackCreateInfo = {};
			callbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

			callbackCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			if (validationLevel >= RenderAPIValidationLevel::Debug)
				callbackCreateInfo.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;

			if (validationLevel >= RenderAPIValidationLevel::Verbose)
				callbackCreateInfo.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;

			if (validationLevel >= RenderAPIValidationLevel::Warnings)
				callbackCreateInfo.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;

			callbackCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			callbackCreateInfo.pfnUserCallback = &DebugMessengerCallback;
			callbackCreateInfo.pUserData = this;

			if (!m_internalData->debugMessenger.Create(*this, callbackCreateInfo))
			{
				NazaraWarning("failed to install debug message callback");
				return;
			}
		}
		else if (Vk::DebugReportCallbackEXT::IsSupported(*this))
		{
			NazaraWarning(VK_EXT_DEBUG_UTILS_EXTENSION_NAME " is not supported, falling back on " VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

			VkDebugReportCallbackCreateInfoEXT callbackCreateInfo = {};
			callbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
			callbackCreateInfo.flags = VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_INFORMATION_BIT_EXT;
			callbackCreateInfo.pfnCallback = &DebugReportCallback;
			callbackCreateInfo.pUserData = this;

			if (!m_internalData->debugCallback.Create(*this, callbackCreateInfo))
			{
				NazaraWarning("failed to install debug message callback");
				return;
			}
		}
		else
		{
			NazaraWarning(VK_EXT_DEBUG_UTILS_EXTENSION_NAME " nor " VK_EXT_DEBUG_REPORT_EXTENSION_NAME " are not supported, cannot install debug message callback");
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
