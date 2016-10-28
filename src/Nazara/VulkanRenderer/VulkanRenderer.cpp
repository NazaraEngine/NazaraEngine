// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanRenderer.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/VulkanRenderer/VkRenderWindow.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Loader.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	VulkanRenderer::~VulkanRenderer()
	{
		Vulkan::Uninitialize();
	}

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
		return Vulkan::Initialize(APIVersion, parameters);
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
