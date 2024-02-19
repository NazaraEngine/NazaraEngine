// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/VulkanRenderer/Vulkan.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/VulkanRenderer/Export.hpp>
#include <Nazara/VulkanRenderer/VulkanDevice.hpp>
#include <NazaraUtils/Algorithm.hpp>
#include <NazaraUtils/CallOnExit.hpp>
#include <NazaraUtils/StringHash.hpp>
#include <array>
#include <unordered_set>

namespace Nz
{
	namespace
	{
		struct AvailableVulkanLayer
		{
			VkLayerProperties layerProperties;
			std::unordered_map<std::string, std::size_t, StringHash<>, std::equal_to<>> extensionByName;
			std::vector<VkExtensionProperties> extensionList;
		};

		void EnumerateVulkanLayers(std::vector<AvailableVulkanLayer>& availableLayers, std::unordered_map<std::string, std::size_t, StringHash<>, std::equal_to<>>& layerByName)
		{
			std::vector<VkLayerProperties> layerList;
			if (Vk::Loader::EnumerateInstanceLayerProperties(&layerList))
			{
				for (VkLayerProperties& layerProperties : layerList)
				{
					std::size_t layerIndex = availableLayers.size();
					auto& layerData = availableLayers.emplace_back();
					layerData.layerProperties = layerProperties;

					if (Vk::Loader::EnumerateInstanceExtensionProperties(&layerData.extensionList, layerProperties.layerName))
					{
						for (VkExtensionProperties& extProperty : layerData.extensionList)
							layerData.extensionByName.emplace(extProperty.extensionName, layerData.extensionByName.size());
					}

					layerByName.emplace(layerProperties.layerName, layerIndex);
				}
			}
		}
	}

	RenderDeviceInfo Vulkan::BuildRenderDeviceInfo(const Vk::PhysicalDevice& physDevice)
	{
		RenderDeviceInfo deviceInfo;
		deviceInfo.name = physDevice.properties.deviceName;

		deviceInfo.features.anisotropicFiltering = physDevice.features.samplerAnisotropy;
		deviceInfo.features.computeShaders = true;
		deviceInfo.features.depthClamping = physDevice.features.depthClamp;
		deviceInfo.features.nonSolidFaceFilling = physDevice.features.fillModeNonSolid;
		deviceInfo.features.storageBuffers = true;
		deviceInfo.features.textureReadWithoutFormat = physDevice.features.shaderStorageImageReadWithoutFormat;
		deviceInfo.features.textureReadWrite = true;
		deviceInfo.features.textureWriteWithoutFormat = physDevice.features.shaderStorageImageWriteWithoutFormat;
		deviceInfo.features.unrestrictedTextureViews = true;

		deviceInfo.limits.maxComputeSharedMemorySize = physDevice.properties.limits.maxComputeSharedMemorySize;
		deviceInfo.limits.maxComputeWorkGroupCount = { physDevice.properties.limits.maxComputeWorkGroupCount[0], physDevice.properties.limits.maxComputeWorkGroupCount[1], physDevice.properties.limits.maxComputeWorkGroupCount[2] };
		deviceInfo.limits.maxComputeWorkGroupSize = { physDevice.properties.limits.maxComputeWorkGroupSize[0], physDevice.properties.limits.maxComputeWorkGroupSize[1], physDevice.properties.limits.maxComputeWorkGroupSize[2] };
		deviceInfo.limits.maxComputeWorkGroupInvocations = physDevice.properties.limits.maxComputeWorkGroupInvocations;
		deviceInfo.limits.maxStorageBufferSize = physDevice.properties.limits.maxStorageBufferRange;
		deviceInfo.limits.maxUniformBufferSize = physDevice.properties.limits.maxUniformBufferRange;
		deviceInfo.limits.minStorageBufferOffsetAlignment = RoundToPow2(physDevice.properties.limits.minStorageBufferOffsetAlignment);
		deviceInfo.limits.minUniformBufferOffsetAlignment = RoundToPow2(physDevice.properties.limits.minUniformBufferOffsetAlignment);

		switch (physDevice.properties.deviceType)
		{
			case VK_PHYSICAL_DEVICE_TYPE_CPU:
				deviceInfo.type = RenderDeviceType::Software;
				break;

			case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
				deviceInfo.type = RenderDeviceType::Dedicated;
				break;

			case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
				deviceInfo.type = RenderDeviceType::Integrated;
				break;

			case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
				deviceInfo.type = RenderDeviceType::Virtual;
				break;

			default:
				NazaraWarningFmt("Device {0} has handled device type ({1:#x})", deviceInfo.name, UnderlyingCast(physDevice.properties.deviceType));
				[[fallthrough]];
			case VK_PHYSICAL_DEVICE_TYPE_OTHER:
				deviceInfo.type = RenderDeviceType::Unknown;
				break;
		}

		return deviceInfo;
	}

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
		NazaraInternalErrorFmt("Invalid physical device: {0}", static_cast<void*>(physDevice));

		static Vk::PhysicalDevice dummy;
		return dummy;
	}

	bool Vulkan::Initialize(UInt32 targetApiVersion, RenderAPIValidationLevel validationLevel, const ParameterList& parameters)
	{
		NazaraAssert(!IsInitialized(), "Vulkan is already initialized");

		// Initialize module here
		if (!Vk::Loader::Initialize())
		{
			NazaraError("failed to load Vulkan API, it may be not installed on your system");
			return false;
		}

		CallOnExit onExit(Vulkan::Uninitialize);

		std::string appName = parameters.GetStringParameter("VkAppInfo_OverrideApplicationName").GetValueOr("Another application made with Nazara Engine");
		std::string engineName = parameters.GetStringParameter("VkAppInfo_OverrideEngineName").GetValueOr("Nazara Engine - Vulkan Renderer");

		UInt32 appVersion = SafeCaster(parameters.GetIntegerParameter("VkAppInfo_OverrideApplicationVersion").GetValueOr(VK_MAKE_API_VERSION(0, 1, 0, 0)));
		UInt32 engineVersion = SafeCaster(parameters.GetIntegerParameter("VkAppInfo_OverrideEngineVersion").GetValueOr(VK_MAKE_API_VERSION(0, 1, 0, 0)));

		if (auto result = parameters.GetIntegerParameter("VkAppInfo_OverrideAPIVersion"))
			targetApiVersion = SafeCaster(result.GetValue());

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
			appName.c_str(),
			appVersion,
			engineName.c_str(),
			engineVersion,
			targetApiVersion
		};

		VkInstanceCreateFlags createFlags = SafeCaster(parameters.GetIntegerParameter("VkInstanceInfo_OverrideCreateFlags").GetValueOr(0));

		std::vector<const char*> enabledLayers;

		std::vector<AvailableVulkanLayer> availableLayers;
		std::unordered_map<std::string, std::size_t, StringHash<>, std::equal_to<>> availableLayerByName;
		EnumerateVulkanLayers(availableLayers, availableLayerByName);

		if (auto result = parameters.GetBooleanParameter("VkInstanceInfo_OverrideEnabledLayers"); !result.GetValueOr(false))
		{
			//< Nazara default layers goes here

			if (validationLevel != RenderAPIValidationLevel::None)
			{
				// Enable Vulkan validation if available in debug mode
				if (availableLayerByName.count("VK_LAYER_KHRONOS_validation"))
					enabledLayers.push_back("VK_LAYER_KHRONOS_validation");
				else if (availableLayerByName.count("VK_LAYER_LUNARG_standard_validation"))
					enabledLayers.push_back("VK_LAYER_LUNARG_standard_validation");
			}
		}

		std::vector<const char*> enabledExtensions;
		std::vector<std::string> additionalLayers; // Just to keep the String alive
		if (long long customLayerCount = parameters.GetIntegerParameter("VkInstanceInfo_EnabledLayerCount").GetValueOr(0) > 0)
		{
			additionalLayers.reserve(customLayerCount);
			for (long long i = 0; i < customLayerCount; ++i)
			{
				std::string parameterName = "VkInstanceInfo_EnabledLayer" + NumberToString(i);
				if (auto result = parameters.GetStringParameter(parameterName))
				{
					additionalLayers.emplace_back(std::move(result).GetValue());
					enabledLayers.push_back(additionalLayers.back().c_str());
				}
				else
					NazaraWarningFmt("missing parameter {0}", parameterName);
			}
		}

		// Get supported extension list
		std::unordered_set<std::string, StringHash<>, std::equal_to<>> availableExtensions;
		std::vector<VkExtensionProperties> extensionList;
		if (Vk::Loader::EnumerateInstanceExtensionProperties(&extensionList))
		{
			for (VkExtensionProperties& extProperty : extensionList)
				availableExtensions.emplace(extProperty.extensionName);
		}

		if (auto result = parameters.GetBooleanParameter("VkInstanceInfo_OverrideEnabledExtensions"); !result.GetValueOr(false))
		{
			enabledExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

			#ifdef VK_USE_PLATFORM_ANDROID_KHR
			enabledExtensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
			#endif

			#ifdef VK_USE_PLATFORM_XCB_KHR
			enabledExtensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
			#endif

			#ifdef VK_USE_PLATFORM_XLIB_KHR
			enabledExtensions.push_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
			#endif

			#ifdef VK_USE_PLATFORM_WAYLAND_KHR
			enabledExtensions.push_back(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
			#endif

			#ifdef VK_USE_PLATFORM_WIN32_KHR
			enabledExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
			#endif

			#ifdef VK_USE_PLATFORM_METAL_EXT
			enabledExtensions.push_back(VK_EXT_METAL_SURFACE_EXTENSION_NAME);
			#endif

			if (availableExtensions.count(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME))
				enabledExtensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

			if (availableExtensions.count(VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
				enabledExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			else if (availableExtensions.count(VK_EXT_DEBUG_REPORT_EXTENSION_NAME))
				enabledExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
		}

		std::vector<std::string> additionalExtensions; // Just to keep the String alive
		if (long long customLayerCount = parameters.GetIntegerParameter("VkInstanceInfo_EnabledExtensionCount").GetValueOr(0) > 0)
		{
			additionalExtensions.reserve(customLayerCount);
			for (int i = 0; i < customLayerCount; ++i)
			{
				std::string parameterName = "VkInstanceInfo_EnabledExtension" + NumberToString(i);
				if (auto result = parameters.GetStringParameter(parameterName))
				{
					additionalLayers.emplace_back(std::move(result).GetValue());
					enabledExtensions.push_back(additionalExtensions.back().c_str());
				}
				else
					NazaraWarningFmt("missing parameter {0}", parameterName);
			}
		}

		VkInstanceCreateInfo instanceInfo = {};
		instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

#ifdef NAZARA_DEBUG
		// Handle VK_LAYER_KHRONOS_validation extended features

		VkValidationFeaturesEXT features = {};
		features.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;

		std::array enabledFeatures = {
			//VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT,
			//VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_RESERVE_BINDING_SLOT_EXT,
			//VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT,
			VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT
		};

		auto validationIt = std::find_if(enabledLayers.begin(), enabledLayers.end(), [&](const char* layerName)
		{
			return std::strcmp(layerName, "VK_LAYER_KHRONOS_validation") == 0;
		});
		if (validationIt != enabledLayers.end())
		{
			auto layerIt = availableLayerByName.find("VK_LAYER_KHRONOS_validation");
			assert(layerIt != availableLayerByName.end());

			auto& validationLayer = availableLayers[layerIt->second];
			if (validationLayer.extensionByName.find(VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME) != validationLayer.extensionByName.end())
			{
				enabledExtensions.push_back(VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME);

				features.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
				features.enabledValidationFeatureCount = UInt32(enabledFeatures.size());
				features.pEnabledValidationFeatures = enabledFeatures.data();

				instanceInfo.pNext = &features;
			}
		}
#endif

		instanceInfo.flags = createFlags;
		instanceInfo.pApplicationInfo = &appInfo;

		instanceInfo.enabledExtensionCount = UInt32(enabledExtensions.size());
		instanceInfo.ppEnabledExtensionNames = enabledExtensions.data();

		instanceInfo.enabledLayerCount = UInt32(enabledLayers.size());
		instanceInfo.ppEnabledLayerNames = enabledLayers.data();

		if (!s_instance.Create(validationLevel, instanceInfo))
		{
			NazaraErrorFmt("failed to create instance: {0}", TranslateVulkanError(s_instance.GetLastErrorCode()));
			return false;
		}

		std::vector<VkPhysicalDevice> physDevices;
		if (!s_instance.EnumeratePhysicalDevices(&physDevices))
		{
			NazaraError("failed to enumerate physical devices");
			return false;
		}

		s_physDevices.reserve(physDevices.size());
		for (VkPhysicalDevice physDevice : physDevices)
		{
			Vk::PhysicalDevice deviceInfo;
			if (!s_instance.GetPhysicalDeviceQueueFamilyProperties(physDevice, &deviceInfo.queueFamilies))
			{
				NazaraWarningFmt("failed to query physical device queue family properties for {0} ({1:#x})", deviceInfo.properties.deviceName, deviceInfo.properties.deviceID);
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
				NazaraWarningFmt("failed to query physical device extensions for {0} ({1:#x})", deviceInfo.properties.deviceName, deviceInfo.properties.deviceID);

			s_physDevices.emplace_back(std::move(deviceInfo));
		}

		if (s_physDevices.empty())
		{
			NazaraError("no valid physical device found");
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

	std::shared_ptr<VulkanDevice> Vulkan::CreateDevice(const Vk::PhysicalDevice& deviceInfo, const RenderDeviceFeatures& enabledFeatures)
	{
		Nz::ErrorFlags errFlags(ErrorMode::ThrowException, true);

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

		return CreateDevice(deviceInfo, enabledFeatures, queuesFamilies.data(), queuesFamilies.size());
	}

	std::shared_ptr<VulkanDevice> Vulkan::CreateDevice(const Vk::PhysicalDevice& deviceInfo, const RenderDeviceFeatures& enabledFeatures, const Vk::Surface& surface, UInt32* graphicsFamilyIndex, UInt32* presentableFamilyIndex, UInt32* transferFamilyIndex)
	{
		Nz::ErrorFlags errFlags(ErrorMode::ThrowException, true);

		// Find a queue that supports graphics operations
		UInt32 graphicsQueueNodeIndex = UINT32_MAX;
		UInt32 presentQueueNodeIndex = UINT32_MAX;
		UInt32 transferQueueNodeFamily = UINT32_MAX;
		for (UInt32 i = 0; i < deviceInfo.queueFamilies.size(); i++)
		{
			bool supportPresentation = false;
			if (!surface.GetSupportPresentation(deviceInfo.physDevice, i, &supportPresentation))
				NazaraWarningFmt("failed to get presentation support of queue family #{0}", i);

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
			NazaraError("device does not support graphics operations");
			return {};
		}

		if (presentQueueNodeIndex == UINT32_MAX)
		{
			// On multi-GPU systems, it's very possible to have surfaces unsupported by some
			NazaraError("device cannot present this surface");
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

		*graphicsFamilyIndex = graphicsQueueNodeIndex;
		*presentableFamilyIndex = presentQueueNodeIndex;
		*transferFamilyIndex = transferQueueNodeFamily;

		return CreateDevice(deviceInfo, enabledFeatures, queuesFamilies.data(), queuesFamilies.size());
	}

	std::shared_ptr<VulkanDevice> Vulkan::CreateDevice(const Vk::PhysicalDevice& deviceInfo, const RenderDeviceFeatures& enabledFeatures, const QueueFamily* queueFamilies, std::size_t queueFamilyCount)
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

		if (auto result = s_initializationParameters.GetBooleanParameter("VkDeviceInfo_OverrideEnabledLayers"); !result.GetValueOr(false))
		{
			//< Nazara default layers goes here
		}

		std::vector<std::string> additionalLayers; // Just to keep the string alive
		if (long long customLayerCount = s_initializationParameters.GetIntegerParameter("VkDeviceInfo_EnabledLayerCount").GetValueOr(0))
		{
			additionalLayers.reserve(customLayerCount);
			for (long long i = 0; i < customLayerCount; ++i)
			{
				std::string parameterName = "VkDeviceInfo_EnabledLayer" + NumberToString(i);
				if (auto value = s_initializationParameters.GetStringViewParameter(parameterName))
				{
					additionalLayers.emplace_back(std::move(value).GetValue());
					enabledLayers.push_back(additionalLayers.back().c_str());
				}
				else
					NazaraWarningFmt("missing parameter {0}", parameterName);
			}
		}

		if (auto result = s_initializationParameters.GetBooleanParameter("VkDeviceInfo_OverrideEnabledExtensions"); !result.GetValueOr(false))
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

			if (deviceInfo.properties.apiVersion < VK_API_VERSION_1_1)
			{
				EnableIfSupported(VK_KHR_BIND_MEMORY_2_EXTENSION_NAME);
				EnableIfSupported(VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME);
			}
		}

		std::vector<std::string> additionalExtensions; // Just to keep the String alive
		if (long long customExtCount = s_initializationParameters.GetIntegerParameter("VkDeviceInfo_EnabledExtensionCount").GetValueOr(0))
		{
			for (long long i = 0; i < customExtCount; ++i)
			{
				std::string parameterName = "VkDeviceInfo_EnabledExtension" + NumberToString(i);
				if (auto value = s_initializationParameters.GetStringViewParameter(parameterName))
				{
					additionalExtensions.emplace_back(std::move(value).GetValue());
					enabledExtensions.push_back(additionalExtensions.back().c_str());
				}
				else
					NazaraWarningFmt("missing parameter {0}", parameterName);
			}
		}

		VkPhysicalDeviceFeatures deviceFeatures = {};
		if (enabledFeatures.anisotropicFiltering)
			deviceFeatures.samplerAnisotropy = VK_TRUE;

		if (enabledFeatures.depthClamping)
			deviceFeatures.depthClamp = VK_TRUE;

		if (enabledFeatures.nonSolidFaceFilling)
			deviceFeatures.fillModeNonSolid = VK_TRUE;

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
			&deviceFeatures
		};

		std::shared_ptr<VulkanDevice> device = std::make_shared<VulkanDevice>(s_instance, enabledFeatures, BuildRenderDeviceInfo(deviceInfo));
		if (!device->Create(deviceInfo, createInfo))
		{
			NazaraErrorFmt("failed to create Vulkan Device: {0}", TranslateVulkanError(device->GetLastErrorCode()));
			return {};
		}

		return device;
	}

	void Vulkan::Uninitialize()
	{
		// Uninitialize module here
		s_instance.Destroy();

		Vk::Loader::Uninitialize();
	}

	std::vector<Vk::PhysicalDevice> Vulkan::s_physDevices;
	Vk::Instance Vulkan::s_instance;
	ParameterList Vulkan::s_initializationParameters;
}
