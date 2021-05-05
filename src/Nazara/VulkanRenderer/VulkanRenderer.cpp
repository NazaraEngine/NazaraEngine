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
		const auto& physDevices = Vulkan::GetPhysicalDevices();

		assert(deviceIndex < physDevices.size());
		return Vulkan::SelectDevice(physDevices[deviceIndex]);
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
		const auto& physDevices = Vulkan::GetPhysicalDevices();

		std::vector<RenderDeviceInfo> devices;
		devices.reserve(physDevices.size());

		for (const Vk::PhysicalDevice& physDevice : physDevices)
			devices.push_back(Vulkan::BuildRenderDeviceInfo(physDevice));

		return devices;
	}
}