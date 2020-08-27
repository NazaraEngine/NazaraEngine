// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanRenderer.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Renderer/RenderDevice.hpp>
#include <Nazara/VulkanRenderer/VulkanBuffer.hpp>
#include <Nazara/VulkanRenderer/VulkanSurface.hpp>
#include <Nazara/VulkanRenderer/VkRenderWindow.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Loader.hpp>
#include <cassert>
#include <sstream>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	VulkanRenderer::~VulkanRenderer()
	{
		Vulkan::Uninitialize();
	}

	std::unique_ptr<RenderSurface> VulkanRenderer::CreateRenderSurfaceImpl()
	{
		return std::make_unique<VulkanSurface>();
	}

	std::unique_ptr<RenderWindowImpl> VulkanRenderer::CreateRenderWindowImpl(RenderWindow& owner)
	{
		return std::make_unique<VkRenderWindow>(owner);
	}

	std::shared_ptr<RenderDevice> VulkanRenderer::InstanciateRenderDevice(std::size_t deviceIndex)
	{
		assert(deviceIndex < m_physDevices.size());
		return Vulkan::SelectDevice(m_physDevices[deviceIndex]);
	}

	bool VulkanRenderer::Prepare(const ParameterList& parameters)
	{
		return Vulkan::Initialize(APIVersion, parameters);
	}

	RenderAPI VulkanRenderer::QueryAPI() const
	{
		return RenderAPI::Vulkan;
	}

	std::string VulkanRenderer::QueryAPIString() const
	{
		std::ostringstream ss;
		ss << "Vulkan renderer " << VK_VERSION_MAJOR(APIVersion) << '.' << VK_VERSION_MINOR(APIVersion) << '.' << VK_VERSION_PATCH(APIVersion);

		return ss.str();
	}

	UInt32 VulkanRenderer::QueryAPIVersion() const
	{
		return APIVersion;
	}

	std::vector<RenderDeviceInfo> VulkanRenderer::QueryRenderDevices() const
	{
		std::vector<RenderDeviceInfo> devices;
		devices.reserve(m_physDevices.size());

		for (const Vk::PhysicalDevice& physDevice : m_physDevices)
		{
			RenderDeviceInfo& device = devices.emplace_back();
			device.name = physDevice.properties.deviceName;

			switch (physDevice.properties.deviceType)
			{
				case VK_PHYSICAL_DEVICE_TYPE_CPU:
					device.type = RenderDeviceType::Software;
					break;

				case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
					device.type = RenderDeviceType::Dedicated;
					break;

				case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
					device.type = RenderDeviceType::Integrated;
					break;

				case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
					device.type = RenderDeviceType::Virtual;
					break;

				default:
					NazaraWarning("Device " + device.name + " has handled device type (0x" + String::Number(physDevice.properties.deviceType, 16) + ')');
				case VK_PHYSICAL_DEVICE_TYPE_OTHER:
					device.type = RenderDeviceType::Unknown;
					break;
			}
		}

		return devices;
	}
}
