// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/Vulkan.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Utility/Utility.hpp>
#include <Nazara/VulkanRenderer/Config.hpp>
#include <Nazara/VulkanRenderer/VulkanDevice.hpp>
#include <array>
#include <unordered_set>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	Vk::Instance& Vulkan::GetInstance()
	{
		return s_instance;
	}

	const std::vector<Vk::PhysicalDevice>& Vulkan::GetPhysicalDevices()
	{
		return s_physDevices;
	}

	const Vk::PhysicalDevice& Vulkan::GetPhysicalDeviceInfo(VkPhysicalDevice physDevice)
	{
		for (const Vk::PhysicalDevice& info : s_physDevices)
		{
			if (info.physDevice == physDevice)
				return info;
		}

		// This cannot happen if physDevice is valid, as we retrieved every physical device
		NazaraInternalError("Invalid physical device: " + String::Pointer(physDevice));

		static Vk::PhysicalDevice dummy;
		return dummy;
	}

	bool Vulkan::Initialize(UInt32 targetApiVersion, const ParameterList& parameters)
	{
		NazaraAssert(!IsInitialized(), "Vulkan is already initialized");

		// Initialize module here
		if (!Vk::Loader::Initialize())
		{
			NazaraError("Failed to load Vulkan API, it may be not installed on your system");
			return false;
		}

		CallOnExit onExit(Vulkan::Uninitialize);

		String appName = "Another application made with Nazara Engine";
		String engineName = "Nazara Engine - Vulkan Renderer";

		UInt32 appVersion = VK_MAKE_VERSION(1, 0, 0);
		UInt32 engineVersion = VK_MAKE_VERSION(1, 0, 0);

		parameters.GetStringParameter("VkAppInfo_OverrideApplicationName", &appName);
		parameters.GetStringParameter("VkAppInfo_OverrideEngineName", &engineName);

		bool bParam;
		long long iParam;

		if (parameters.GetIntegerParameter("VkAppInfo_OverrideAPIVersion", &iParam))
			targetApiVersion = static_cast<UInt32>(iParam);

		if (parameters.GetIntegerParameter("VkAppInfo_OverrideApplicationVersion", &iParam))
			appVersion = static_cast<UInt32>(iParam);

		if (parameters.GetIntegerParameter("VkAppInfo_OverrideEngineVersion", &iParam))
			engineVersion = static_cast<UInt32>(iParam);

		if (Vk::Loader::vkEnumerateInstanceVersion)
		{
			UInt32 supportedApiVersion;
			Vk::Loader::vkEnumerateInstanceVersion(&supportedApiVersion);

			targetApiVersion = std::min(targetApiVersion, supportedApiVersion);
		}
		else
			// vkEnumerateInstanceVersion is available from Vulkan 1.1, fallback to 1.0 if not supported
			targetApiVersion = VK_API_VERSION_1_0;

		VkApplicationInfo appInfo = {
			VK_STRUCTURE_TYPE_APPLICATION_INFO,
			nullptr,
			appName.GetConstBuffer(),
			appVersion,
			engineName.GetConstBuffer(),
			engineVersion,
			targetApiVersion
		};

		VkInstanceCreateFlags createFlags = 0;

		if (parameters.GetIntegerParameter("VkInstanceInfo_OverrideCreateFlags", &iParam))
			createFlags = static_cast<VkInstanceCreateFlags>(iParam);

		std::vector<const char*> enabledLayers;
		std::vector<const char*> enabledExtensions;

		if (!parameters.GetBooleanParameter("VkInstanceInfo_OverrideEnabledLayers", &bParam) || !bParam)
		{
			//< Nazara default layers goes here
		}

		std::vector<String> additionalLayers; // Just to keep the String alive
		if (parameters.GetIntegerParameter("VkInstanceInfo_EnabledLayerCount", &iParam))
		{
			additionalLayers.reserve(iParam);
			for (long long i = 0; i < iParam; ++i)
			{
				Nz::String parameterName = "VkInstanceInfo_EnabledLayer" + String::Number(i);
				Nz::String layer;
				if (parameters.GetStringParameter(parameterName, &layer))
				{
					additionalLayers.emplace_back(std::move(layer));
					enabledLayers.push_back(additionalLayers.back().GetConstBuffer());
				}
				else
					NazaraWarning("Parameter " + parameterName + " expected");
			}
		}

		// Get extension list
		std::unordered_set<std::string> availableExtensions;
		std::vector<VkExtensionProperties> extensionList;
		if (Vk::Loader::EnumerateInstanceExtensionProperties(&extensionList))
		{
			for (VkExtensionProperties& extProperty : extensionList)
				availableExtensions.insert(extProperty.extensionName);
		}

		if (!parameters.GetBooleanParameter("VkInstanceInfo_OverrideEnabledExtensions", &bParam) || !bParam)
		{
			enabledExtensions.push_back("VK_KHR_surface");

			#ifdef VK_USE_PLATFORM_ANDROID_KHR
			enabledExtensions.push_back("VK_KHR_android_surface");
			#endif

			#ifdef VK_USE_PLATFORM_MIR_KHR
			enabledExtensions.push_back("VK_KHR_mir_surface");
			#endif

			#ifdef VK_USE_PLATFORM_XCB_KHR
			enabledExtensions.push_back("VK_KHR_xcb_surface");
			#endif

			#ifdef VK_USE_PLATFORM_XLIB_KHR
			enabledExtensions.push_back("VK_KHR_xlib_surface");
			#endif

			#ifdef VK_USE_PLATFORM_WAYLAND_KHR
			enabledExtensions.push_back("VK_KHR_wayland_surface");
			#endif

			#ifdef VK_USE_PLATFORM_WIN32_KHR
			enabledExtensions.push_back("VK_KHR_win32_surface");
			#endif

			if (availableExtensions.count(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME))
				enabledExtensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

			if (availableExtensions.count(VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
				enabledExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		std::vector<String> additionalExtensions; // Just to keep the String alive
		if (parameters.GetIntegerParameter("VkInstanceInfo_EnabledExtensionCount", &iParam))
		{
			additionalExtensions.reserve(iParam);
			for (int i = 0; i < iParam; ++i)
			{
				Nz::String parameterName = "VkInstanceInfo_EnabledExtension" + String::Number(i);
				Nz::String extension;
				if (parameters.GetStringParameter(parameterName, &extension))
				{
					additionalExtensions.emplace_back(std::move(extension));
					enabledExtensions.push_back(additionalExtensions.back().GetConstBuffer());
				}
				else
					NazaraWarning("Parameter " + parameterName + " expected");
			}
		}

		VkInstanceCreateInfo instanceInfo = {
			VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			nullptr,
			createFlags,
			&appInfo,
			UInt32(enabledLayers.size()),
			enabledLayers.data(),
			UInt32(enabledExtensions.size()),
			enabledExtensions.data()
		};

		if (!s_instance.Create(instanceInfo))
		{
			NazaraError("Failed to create instance: " + TranslateVulkanError(s_instance.GetLastErrorCode()));
			return false;
		}

		std::vector<VkPhysicalDevice> physDevices;
		if (!s_instance.EnumeratePhysicalDevices(&physDevices))
		{
			NazaraError("Failed to enumerate physical devices");
			return false;
		}

		s_physDevices.reserve(physDevices.size());
		for (VkPhysicalDevice physDevice : physDevices)
		{
			Vk::PhysicalDevice deviceInfo;
			if (!s_instance.GetPhysicalDeviceQueueFamilyProperties(physDevice, &deviceInfo.queueFamilies))
			{
				NazaraWarning("Failed to query physical device queue family properties for " + String(deviceInfo.properties.deviceName) + " (0x" + String::Number(deviceInfo.properties.deviceID, 16) + ')');
				continue;
			}

			deviceInfo.physDevice = physDevice;

			deviceInfo.features         = s_instance.GetPhysicalDeviceFeatures(physDevice);
			deviceInfo.memoryProperties = s_instance.GetPhysicalDeviceMemoryProperties(physDevice);
			deviceInfo.properties       = s_instance.GetPhysicalDeviceProperties(physDevice);

			std::vector<VkExtensionProperties> extensions;
			if (s_instance.GetPhysicalDeviceExtensions(physDevice, &extensions))
			{
				for (auto& extProperty : extensions)
					deviceInfo.extensions.emplace(extProperty.extensionName);
			}
			else
				NazaraWarning("Failed to query physical device extensions for " + String(deviceInfo.properties.deviceName) + " (0x" + String::Number(deviceInfo.properties.deviceID, 16) + ')');

			s_physDevices.emplace_back(std::move(deviceInfo));
		}

		if (s_physDevices.empty())
		{
			NazaraError("No valid physical device found");
			return false;
		}

		s_initializationParameters = parameters;

		onExit.Reset();

		NazaraNotice("Initialized: Vulkan module");
		return true;
	}

	bool Vulkan::IsInitialized()
	{
		return s_instance.IsValid();
	}

	std::shared_ptr<VulkanDevice> Vulkan::CreateDevice(const Vk::PhysicalDevice& deviceInfo)
	{
		Nz::ErrorFlags errFlags(ErrorFlag_ThrowException, true);

		// Find a queue that supports graphics operations
		UInt32 graphicsQueueNodeIndex = UINT32_MAX;
		UInt32 transfertQueueNodeFamily = UINT32_MAX;

		for (UInt32 i = 0; i < deviceInfo.queueFamilies.size(); i++)
		{
			if (deviceInfo.queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				graphicsQueueNodeIndex = i;
				break;
			}
		}

		for (UInt32 i = 0; i < deviceInfo.queueFamilies.size(); i++)
		{
			if (deviceInfo.queueFamilies[i].queueFlags & (VK_QUEUE_COMPUTE_BIT | VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT)) //< Compute and graphics queue implicitly support transfer operations
			{
				transfertQueueNodeFamily = i;
				if (transfertQueueNodeFamily != graphicsQueueNodeIndex)
					break;
			}
		}

		std::array<QueueFamily, 2> queuesFamilies = {
			{
				{ graphicsQueueNodeIndex, 1.f },
				{ transfertQueueNodeFamily, 1.f }
			}
		};

		return CreateDevice(deviceInfo, queuesFamilies.data(), queuesFamilies.size());
	}

	std::shared_ptr<VulkanDevice> Vulkan::CreateDevice(const Vk::PhysicalDevice& deviceInfo, const Vk::Surface& surface, UInt32* presentableFamilyQueue)
	{
		Nz::ErrorFlags errFlags(ErrorFlag_ThrowException, true);

		// Find a queue that supports graphics operations
		UInt32 graphicsQueueNodeIndex = UINT32_MAX;
		UInt32 presentQueueNodeIndex = UINT32_MAX;
		UInt32 transferQueueNodeFamily = UINT32_MAX;
		for (UInt32 i = 0; i < deviceInfo.queueFamilies.size(); i++)
		{
			bool supportPresentation = false;
			if (!surface.GetSupportPresentation(deviceInfo.physDevice, i, &supportPresentation))
				NazaraWarning("Failed to get presentation support of queue family #" + String::Number(i));

			if (deviceInfo.queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				if (supportPresentation)
				{
					// Queue family support both graphics and presentation to our surface, choose it
					graphicsQueueNodeIndex = i;
					presentQueueNodeIndex = i;
					break;
				}
				else if (graphicsQueueNodeIndex == UINT32_MAX)
					graphicsQueueNodeIndex = i;
			}
			else if (supportPresentation)
				presentQueueNodeIndex = i;
		}

		if (graphicsQueueNodeIndex == UINT32_MAX)
		{
			// A Vulkan device without graphics support may technically exists but I've yet to see one
			NazaraError("Device does not support graphics operations");
			return {};
		}

		if (presentQueueNodeIndex == UINT32_MAX)
		{
			// On multi-GPU systems, it's very possible to have surfaces unsupported by some
			NazaraError("Device cannot present this surface");
			return {};
		}

		// Search for a transfer queue (first one being different to the graphics one)
		for (UInt32 i = 0; i < deviceInfo.queueFamilies.size(); i++)
		{
			// Transfer bit is not mandatory if compute and graphics bits are set (as they implicitly support transfer)
			if (deviceInfo.queueFamilies[i].queueFlags & (VK_QUEUE_COMPUTE_BIT | VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT))
			{
				transferQueueNodeFamily = i;
				if (transferQueueNodeFamily != graphicsQueueNodeIndex)
					break;
			}
		}
		assert(transferQueueNodeFamily != UINT32_MAX);

		std::array<QueueFamily, 3> queuesFamilies = {
			{
				{graphicsQueueNodeIndex, 1.f},
				{presentQueueNodeIndex, 1.f},
				{transferQueueNodeFamily, 1.f}
			}
		};

		*presentableFamilyQueue = presentQueueNodeIndex;

		return CreateDevice(deviceInfo, queuesFamilies.data(), queuesFamilies.size());
	}

	std::shared_ptr<VulkanDevice> Vulkan::CreateDevice(const Vk::PhysicalDevice& deviceInfo, const QueueFamily* queueFamilies, std::size_t queueFamilyCount)
	{
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		queueCreateInfos.reserve(queueFamilyCount);

		for (std::size_t i = 0; i < queueFamilyCount; ++i)
		{
			const QueueFamily& queueFamily = queueFamilies[i];

			auto it = std::find_if(queueCreateInfos.begin(), queueCreateInfos.end(), [&] (const VkDeviceQueueCreateInfo& createInfo)
			{
				return createInfo.queueFamilyIndex == queueFamily.familyIndex;
			});

			if (it == queueCreateInfos.end())
			{
				VkDeviceQueueCreateInfo createInfo = {
					VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, // VkStructureType             sType;
					nullptr,                                    // const void*                 pNext;
					0,                                          // VkDeviceQueueCreateFlags    flags;
					queueFamily.familyIndex,                    // uint32_t                    queueFamilyIndex;
					1,                                          // uint32_t                    queueCount;
					&queueFamily.priority                       // const float*                pQueuePriorities;
				};

				queueCreateInfos.emplace_back(createInfo);
			}
		}

		std::vector<const char*> enabledLayers;
		std::vector<const char*> enabledExtensions;

		bool bParam;
		long long iParam;

		if (!s_initializationParameters.GetBooleanParameter("VkDeviceInfo_OverrideEnabledLayers", &bParam) || !bParam)
		{
			//< Nazara default layers goes here
		}

		std::vector<String> additionalLayers; // Just to keep the String alive
		if (s_initializationParameters.GetIntegerParameter("VkDeviceInfo_EnabledLayerCount", &iParam))
		{
			additionalLayers.reserve(iParam);
			for (long long i = 0; i < iParam; ++i)
			{
				Nz::String parameterName = "VkDeviceInfo_EnabledLayer" + String::Number(i);
				Nz::String layer;
				if (s_initializationParameters.GetStringParameter(parameterName, &layer))
				{
					additionalLayers.emplace_back(std::move(layer));
					enabledLayers.push_back(additionalLayers.back().GetConstBuffer());
				}
				else
					NazaraWarning("Parameter " + parameterName + " expected");
			}
		}

		if (!s_initializationParameters.GetBooleanParameter("VkDeviceInfo_OverrideEnabledExtensions", &bParam) || !bParam)
		{
			// Swapchain extension is required for rendering
			enabledExtensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

			auto EnableIfSupported = [&](const char* extName)
			{
				if (deviceInfo.extensions.count(extName))
					enabledExtensions.emplace_back(extName);
			};

			// VMA extensions
			EnableIfSupported(VK_EXT_MEMORY_BUDGET_EXTENSION_NAME);
			EnableIfSupported(VK_KHR_BIND_MEMORY_2_EXTENSION_NAME);
			EnableIfSupported(VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME);
		}

		std::vector<String> additionalExtensions; // Just to keep the String alive
		if (s_initializationParameters.GetIntegerParameter("VkDeviceInfo_EnabledExtensionCount", &iParam))
		{
			for (long long i = 0; i < iParam; ++i)
			{
				Nz::String parameterName = "VkDeviceInfo_EnabledExtension" + String::Number(i);
				Nz::String extension;
				if (s_initializationParameters.GetStringParameter(parameterName, &extension))
				{
					additionalExtensions.emplace_back(std::move(extension));
					enabledExtensions.push_back(additionalExtensions.back().GetConstBuffer());
				}
				else
					NazaraWarning("Parameter " + parameterName + " expected");
			}
		}

		VkDeviceCreateInfo createInfo = {
			VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			nullptr,
			0,
			UInt32(queueCreateInfos.size()),
			queueCreateInfos.data(),
			UInt32(enabledLayers.size()),
			enabledLayers.data(),
			UInt32(enabledExtensions.size()),
			enabledExtensions.data(),
			nullptr
		};

		std::shared_ptr<VulkanDevice> device = std::make_shared<VulkanDevice>(s_instance);
		if (!device->Create(deviceInfo, createInfo))
		{
			NazaraError("Failed to create Vulkan Device: " + TranslateVulkanError(device->GetLastErrorCode()));
			return {};
		}

		s_devices.emplace_back(device);

		return device;
	}

	std::shared_ptr<VulkanDevice> Vulkan::SelectDevice(const Vk::PhysicalDevice& deviceInfo)
	{
		for (auto it = s_devices.begin(); it != s_devices.end();)
		{
			const auto& devicePtr = *it;
			if (devicePtr->GetPhysicalDevice() == deviceInfo.physDevice)
				return devicePtr;
		}

		return CreateDevice(deviceInfo);
	}

	std::shared_ptr<VulkanDevice> Vulkan::SelectDevice(const Vk::PhysicalDevice& deviceInfo, const Vk::Surface& surface, UInt32* presentableFamilyQueue)
	{
		// First, try to find a device compatible with that surface
		for (auto it = s_devices.begin(); it != s_devices.end();)
		{
			const auto& devicePtr = *it;
			if (devicePtr->GetPhysicalDevice() == deviceInfo.physDevice)
			{
				const std::vector<Vk::Device::QueueFamilyInfo>& queueFamilyInfo = devicePtr->GetEnabledQueues();
				UInt32 presentableQueueFamilyIndex = UINT32_MAX;
				for (const Vk::Device::QueueFamilyInfo& queueInfo : queueFamilyInfo)
				{
					bool supported = false;
					if (surface.GetSupportPresentation(deviceInfo.physDevice, queueInfo.familyIndex, &supported) && supported)
					{
						if (presentableQueueFamilyIndex == UINT32_MAX || queueInfo.flags & VK_QUEUE_GRAPHICS_BIT)
						{
							presentableQueueFamilyIndex = queueInfo.familyIndex;
							if (queueInfo.flags & VK_QUEUE_GRAPHICS_BIT)
								break;
						}
					}
				}

				if (presentableQueueFamilyIndex != UINT32_MAX)
				{
					*presentableFamilyQueue = presentableQueueFamilyIndex;
					return devicePtr;
				}
			}

			++it;
		}

		// No device had support for that surface, create one
		return CreateDevice(deviceInfo, surface, presentableFamilyQueue);
	}

	void Vulkan::Uninitialize()
	{
		// Uninitialize module here
		s_devices.clear();
		s_instance.Destroy();

		Vk::Loader::Uninitialize();
	}

	std::vector<std::shared_ptr<VulkanDevice>> Vulkan::s_devices;
	std::vector<Vk::PhysicalDevice> Vulkan::s_physDevices;
	Vk::Instance Vulkan::s_instance;
	ParameterList Vulkan::s_initializationParameters;
}

