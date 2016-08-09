// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Vulkan/Vulkan.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Utility/Utility.hpp>
#include <Nazara/Vulkan/Config.hpp>
#include <array>
#include <Nazara/Vulkan/Debug.hpp>

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
			if (info.device == physDevice)
				return info;
		}

		// This cannot happen if physDevice is valid, as we retrieved every physical device
		NazaraInternalError("Invalid physical device: " + String::Pointer(physDevice));

		static Vk::PhysicalDevice dummy;
		return dummy;
	}

	bool Vulkan::Initialize()
	{
		if (s_moduleReferenceCounter > 0)
		{
			s_moduleReferenceCounter++;
			return true; // Already initialized
		}

		// Initialize module dependencies
		if (!Utility::Initialize())
		{
			NazaraError("Failed to initialize utility module");
			return false;
		}

		s_moduleReferenceCounter++;

		CallOnExit onExit(Vulkan::Uninitialize);

		// Initialize module here
		if (!Vk::Loader::Initialize())
		{
			NazaraError("Failed to load Vulkan API, it may be not installed on your system");
			return false;
		}

		String appName = "Another application made with Nazara Engine";
		String engineName = "Nazara Engine - Vulkan Renderer";
		UInt32 apiVersion = VK_MAKE_VERSION(1, 0, 8);
		UInt32 appVersion = VK_MAKE_VERSION(1, 0, 0);
		UInt32 engineVersion = VK_MAKE_VERSION(1, 0, 0);

		s_initializationParameters.GetStringParameter("VkAppInfo_OverrideApplicationName", &appName);
		s_initializationParameters.GetStringParameter("VkAppInfo_OverrideEngineName", &engineName);

		bool bParam;
		int iParam;

		if (s_initializationParameters.GetIntegerParameter("VkAppInfo_OverrideAPIVersion", &iParam))
			apiVersion = iParam;

		if (s_initializationParameters.GetIntegerParameter("VkAppInfo_OverrideApplicationVersion", &iParam))
			appVersion = iParam;

		if (s_initializationParameters.GetIntegerParameter("VkAppInfo_OverrideEngineVersion", &iParam))
			engineVersion = iParam;

		VkApplicationInfo appInfo = {
			VK_STRUCTURE_TYPE_APPLICATION_INFO,
			nullptr,
			appName.GetConstBuffer(),
			appVersion,
			engineName.GetConstBuffer(),
			engineVersion,
			apiVersion
		};

		VkInstanceCreateFlags createFlags = 0;

		if (s_initializationParameters.GetIntegerParameter("VkInstanceInfo_OverrideCreateFlags", &iParam))
			createFlags = static_cast<VkInstanceCreateFlags>(iParam);

		std::vector<const char*> enabledLayers;
		std::vector<const char*> enabledExtensions;

		if (!s_initializationParameters.GetBooleanParameter("VkInstanceInfo_OverrideEnabledLayers", &bParam) || !bParam)
		{
			//< Nazara default layers goes here
		}

		std::vector<String> additionalLayers; // Just to keep the String alive
		if (s_initializationParameters.GetIntegerParameter("VkInstanceInfo_EnabledLayerCount", &iParam))
		{
			additionalLayers.reserve(iParam);
			for (int i = 0; i < iParam; ++i)
			{
				Nz::String parameterName = "VkInstanceInfo_EnabledLayer" + String::Number(i);
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

		if (!s_initializationParameters.GetBooleanParameter("VkInstanceInfo_OverrideEnabledExtensions", &bParam) || !bParam)
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
		}

		std::vector<String> additionalExtensions; // Just to keep the String alive
		if (s_initializationParameters.GetIntegerParameter("VkInstanceInfo_EnabledExtensionCount", &iParam))
		{
			additionalExtensions.reserve(iParam);
			for (int i = 0; i < iParam; ++i)
			{
				Nz::String parameterName = "VkInstanceInfo_EnabledExtension" + String::Number(i);
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
			NazaraError("Failed to create instance");
			return false;
		}

		std::vector<VkPhysicalDevice> physDevices;
		if (!s_instance.EnumeratePhysicalDevices(&physDevices))
		{
			NazaraError("Failed to enumerate physical devices");
			return false;
		}

		s_physDevices.reserve(physDevices.size());

		for (std::size_t i = 0; i < physDevices.size(); ++i)
		{
			VkPhysicalDevice physDevice = physDevices[i];

			Vk::PhysicalDevice deviceInfo;
			if (!s_instance.GetPhysicalDeviceQueueFamilyProperties(physDevice, &deviceInfo.queues))
			{
				NazaraWarning("Failed to query physical device queue family properties");
				continue;
			}

			deviceInfo.device = physDevice;

			deviceInfo.features = s_instance.GetPhysicalDeviceFeatures(physDevice);
			deviceInfo.memoryProperties = s_instance.GetPhysicalDeviceMemoryProperties(physDevice);
			deviceInfo.properties = s_instance.GetPhysicalDeviceProperties(physDevice);

			s_physDevices.emplace_back(std::move(deviceInfo));
		}

		if (s_physDevices.empty())
		{
			NazaraError("No valid physical device found");
			return false;
		}

		onExit.Reset();

		NazaraNotice("Initialized: Vulkan module");
		return true;
	}

	bool Vulkan::IsInitialized()
	{
		return s_moduleReferenceCounter != 0;
	}

	Vk::DeviceHandle Vulkan::CreateDevice(VkPhysicalDevice gpu, const Vk::Surface& surface, UInt32* presentableFamilyQueue)
	{
		Nz::ErrorFlags errFlags(ErrorFlag_ThrowException, true);

		std::vector<VkQueueFamilyProperties> queueFamilies;
		s_instance.GetPhysicalDeviceQueueFamilyProperties(gpu, &queueFamilies);

		// Find a queue that supports graphics operations
		UInt32 graphicsQueueNodeIndex = UINT32_MAX;
		UInt32 presentQueueNodeIndex = UINT32_MAX;
		UInt32 transfertQueueNodeFamily = UINT32_MAX;
		for (UInt32 i = 0; i < queueFamilies.size(); i++)
		{
			bool supportPresentation = false;
			if (!surface.GetSupportPresentation(gpu, i, &supportPresentation))
				NazaraWarning("Failed to get presentation support of queue family #" + String::Number(i));

			if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				if (graphicsQueueNodeIndex == UINT32_MAX)
					graphicsQueueNodeIndex = i;

				if (supportPresentation)
				{
					graphicsQueueNodeIndex = i;
					presentQueueNodeIndex = i;
					break;
				}
			}
			else if (supportPresentation)
				presentQueueNodeIndex = i;
		}

		for (UInt32 i = 0; i < queueFamilies.size(); i++)
		{
			if (queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
			{
				transfertQueueNodeFamily = i;
				if (transfertQueueNodeFamily != graphicsQueueNodeIndex)
					break;
			}
		}

		std::array<UInt32, 3> usedQueueFamilies = {graphicsQueueNodeIndex, presentQueueNodeIndex, transfertQueueNodeFamily};
		std::array<float, 3>  priorities = {1.f, 1.f, 1.f};

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		for (UInt32 queueFamily : usedQueueFamilies)
		{
			auto it = std::find_if(queueCreateInfos.begin(), queueCreateInfos.end(), [queueFamily] (const VkDeviceQueueCreateInfo& createInfo)
			{
				return createInfo.queueFamilyIndex == queueFamily;
			});

			if (it == queueCreateInfos.end())
			{
				VkDeviceQueueCreateInfo createInfo = {
								VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, // VkStructureType             sType;
								nullptr,                                    // const void*                 pNext;
								0,                                          // VkDeviceQueueCreateFlags    flags;
								queueFamily,                                // uint32_t                    queueFamilyIndex;
								1,                                          // uint32_t                    queueCount;
								priorities.data()                           // const float*                pQueuePriorities;
				};

				queueCreateInfos.emplace_back(createInfo);
			}
		}


		std::vector<const char*> enabledLayers;
		std::vector<const char*> enabledExtensions;

		bool bParam;
		int iParam;

		if (!s_initializationParameters.GetBooleanParameter("VkDeviceInfo_OverrideEnabledLayers", &bParam) || !bParam)
		{
			//< Nazara default layers goes here
		}

		std::vector<String> additionalLayers; // Just to keep the String alive
		if (s_initializationParameters.GetIntegerParameter("VkDeviceInfo_EnabledLayerCount", &iParam))
		{
			additionalLayers.reserve(iParam);
			for (int i = 0; i < iParam; ++i)
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
			enabledExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

		std::vector<String> additionalExtensions; // Just to keep the String alive
		if (s_initializationParameters.GetIntegerParameter("VkDeviceInfo_EnabledExtensionCount", &iParam))
		{
			for (int i = 0; i < iParam; ++i)
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

		///TODO: First create then move
		s_devices.emplace_back(s_instance);

		Vk::Device& device = s_devices.back();
		device.Create(gpu, createInfo);

		*presentableFamilyQueue = presentQueueNodeIndex;

		return device.CreateHandle();
	}

	Vk::DeviceHandle Vulkan::SelectDevice(VkPhysicalDevice gpu, const Vk::Surface& surface, UInt32* presentableFamilyQueue)
	{
		// First, try to find a device compatible with that surface
		for (Vk::Device& device : s_devices)
		{
			if (device.GetPhysicalDevice() == gpu)
			{
				const std::vector<Vk::Device::QueueFamilyInfo>& queueFamilyInfo = device.GetEnabledQueues();
				UInt32 presentableQueueFamilyIndex = UINT32_MAX;
				for (Vk::Device::QueueFamilyInfo queueInfo : queueFamilyInfo)
				{
					bool supported = false;
					if (surface.GetSupportPresentation(gpu, queueInfo.familyIndex, &supported) && supported)
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
					*presentableFamilyQueue = presentableQueueFamilyIndex;
			}
		}

		// No device had support for that surface, create one
		return CreateDevice(gpu, surface, presentableFamilyQueue);
	}

	void Vulkan::SetParameters(const ParameterList& parameters)
	{
		s_initializationParameters = parameters;
	}

	void Vulkan::Uninitialize()
	{
		if (s_moduleReferenceCounter != 1)
		{
			// Either the module is not initialized, either it was initialized multiple times
			if (s_moduleReferenceCounter > 1)
				s_moduleReferenceCounter--;

			return;
		}

		s_moduleReferenceCounter = 0;

		// Uninitialize module here
		s_devices.clear();
		s_instance.Destroy();

		Vk::Loader::Uninitialize();

		NazaraNotice("Uninitialized: Vulkan module");

		// Free module dependencies
		Utility::Uninitialize();
	}

	std::list<Vk::Device> Vulkan::s_devices;
	std::vector<Vk::PhysicalDevice> Vulkan::s_physDevices;
	Vk::Instance Vulkan::s_instance;
	ParameterList Vulkan::s_initializationParameters;
	unsigned int Vulkan::s_moduleReferenceCounter = 0;	
}

