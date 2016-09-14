// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanRenderer.hpp>
#include <Nazara/VulkanRenderer/VkLoader.hpp>
#include <Nazara/VulkanRenderer/VkRenderWindow.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	std::unique_ptr<RenderWindowImpl> VulkanRenderer::CreateRenderWindowImpl()
	{
		return std::make_unique<VkRenderWindow>();
	}

	bool VulkanRenderer::IsBetterThan(const RendererImpl* other) const
	{
		if (other->QueryAPI() == RenderAPI_Vulkan && QueryAPIVersion() < other->QueryAPIVersion())
			return false;

		return true; //< Vulkan FTW
	}

	bool VulkanRenderer::Prepare(const ParameterList& parameters)
	{
		if (!Vk::Loader::Initialize())
		{
			NazaraError("Failed to load Vulkan API, it may be not installed on your system");
			return false;
		}

		String appName = "Another application made with Nazara Engine";
		String engineName = "Nazara Engine - Vulkan Renderer";

		UInt32 apiVersion = APIVersion;
		UInt32 appVersion = VK_MAKE_VERSION(1, 0, 0);
		UInt32 engineVersion = VK_MAKE_VERSION(1, 0, 0);

		parameters.GetStringParameter("VkAppInfo_OverrideApplicationName", &appName);
		parameters.GetStringParameter("VkAppInfo_OverrideEngineName", &engineName);

		bool bParam;
		int iParam;

		if (parameters.GetIntegerParameter("VkAppInfo_OverrideAPIVersion", &iParam))
			apiVersion = iParam;

		if (parameters.GetIntegerParameter("VkAppInfo_OverrideApplicationVersion", &iParam))
			appVersion = iParam;

		if (parameters.GetIntegerParameter("VkAppInfo_OverrideEngineVersion", &iParam))
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
			for (int i = 0; i < iParam; ++i)
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

		VkInstanceCreateInfo instanceInfo = 
		{
			VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, // VkStructureType             sType;
			nullptr,                                // const void*                 pNext;
			createFlags,                            // VkInstanceCreateFlags       flags;
			&appInfo,                               // const VkApplicationInfo*    pApplicationInfo;
			UInt32(enabledLayers.size()),           // uint32_t                    enabledLayerCount;
			enabledLayers.data(),                   // const char* const*          ppEnabledLayerNames;
			UInt32(enabledExtensions.size()),       // uint32_t                    enabledExtensionCount;
			enabledExtensions.data()                // const char* const*          ppEnabledExtensionNames;
		};

		if (!m_instance.Create(instanceInfo))
		{
			NazaraError("Failed to create instance");
			return false;
		}

		m_apiVersion = apiVersion;

		std::vector<VkPhysicalDevice> physDevices;
		if (!m_instance.EnumeratePhysicalDevices(&physDevices))
		{
			NazaraError("Failed to enumerate physical devices");
			return false;
		}

		m_physDevices.reserve(physDevices.size());
		for (std::size_t i = 0; i < physDevices.size(); ++i)
		{
			VkPhysicalDevice physDevice = physDevices[i];

			Vk::PhysicalDevice deviceInfo;
			if (!m_instance.GetPhysicalDeviceQueueFamilyProperties(physDevice, &deviceInfo.queues))
			{
				NazaraWarning("Failed to query physical device queue family properties for " + String(deviceInfo.properties.deviceName) + " (0x" + String::Number(deviceInfo.properties.deviceID, 16) + ')');
				continue;
			}

			deviceInfo.device = physDevice;

			deviceInfo.features         = m_instance.GetPhysicalDeviceFeatures(physDevice);
			deviceInfo.memoryProperties = m_instance.GetPhysicalDeviceMemoryProperties(physDevice);
			deviceInfo.properties       = m_instance.GetPhysicalDeviceProperties(physDevice);

			m_physDevices.emplace_back(std::move(deviceInfo));
		}

		if (m_physDevices.empty())
		{
			NazaraError("No valid physical device found");
			return false;
		}

		return true;
	}

	RenderAPI VulkanRenderer::QueryAPI() const
	{
		return RenderAPI_Vulkan;
	}

	String VulkanRenderer::QueryAPIString() const
	{
		StringStream ss;
		ss << "Vulkan renderer " << VK_VERSION_MAJOR(m_apiVersion) << '.' << VK_VERSION_MINOR(m_apiVersion) << '.' << VK_VERSION_PATCH(m_apiVersion);

		return ss;
	}

	UInt32 VulkanRenderer::QueryAPIVersion() const
	{
		return m_apiVersion;
	}

	std::vector<RenderDevice> VulkanRenderer::QueryRenderDevices() const
	{
		std::vector<RenderDevice> devices;
		devices.reserve(m_physDevices.size());

		for (const Vk::PhysicalDevice& physDevice : m_physDevices)
		{
			RenderDevice device;
			device.name = physDevice.properties.deviceName;
			
			switch (physDevice.properties.deviceType)
			{
				case VK_PHYSICAL_DEVICE_TYPE_CPU:
					device.type = RenderDeviceType_Software;
					break;

				case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
					device.type = RenderDeviceType_Dedicated;
					break;

				case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
					device.type = RenderDeviceType_Integrated;
					break;

				case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
					device.type = RenderDeviceType_Virtual;
					break;

				default:
					NazaraWarning("Device " + device.name + " has handled device type (0x" + String::Number(physDevice.properties.deviceType, 16) + ')');
				case VK_PHYSICAL_DEVICE_TYPE_OTHER:
					device.type = RenderDeviceType_Unknown;
					break;
			}

			devices.emplace_back(device);
		}

		return devices;
	}
}
