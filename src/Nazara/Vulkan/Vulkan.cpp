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

		bool bParam;
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
		float priority = 1.f;

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		for (UInt32 queueFamily : usedQueueFamilies)
		{
			VkDeviceQueueCreateInfo createInfo = {
				VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				nullptr,
				0,
				queueFamily,
				1,
				&priority
			};

			queueCreateInfos.emplace_back(createInfo);
		}

		std::array<const char*, 1> enabledExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
		std::array<const char*, 0> enabledLayers;

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
	Vk::Instance Vulkan::s_instance;
	ParameterList Vulkan::s_initializationParameters;
	unsigned int Vulkan::s_moduleReferenceCounter = 0;	
}

