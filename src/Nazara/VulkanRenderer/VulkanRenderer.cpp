// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/VulkanRenderer/VulkanRenderer.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Renderer/RenderDevice.hpp>
#include <Nazara/VulkanRenderer/VulkanBuffer.hpp>
#include <Nazara/VulkanRenderer/VulkanSwapchain.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Loader.hpp>
#include <cassert>
#include <sstream>

namespace Nz
{
	VulkanRenderer::~VulkanRenderer()
	{
		Vulkan::Uninitialize();
	}

	std::shared_ptr<RenderDevice> VulkanRenderer::InstanciateRenderDevice(std::size_t deviceIndex, const RenderDeviceFeatures& enabledFeatures)
	{
		const auto& physDevices = Vulkan::GetPhysicalDevices();

		assert(deviceIndex < physDevices.size());
		return Vulkan::CreateDevice(physDevices[deviceIndex], enabledFeatures);
	}

	bool VulkanRenderer::Prepare(const Renderer::Config& config)
	{
		if (!Vulkan::Initialize(APIVersion, config.validationLevel, config.customParameters))
			return false;

		const auto& physDevices = Vulkan::GetPhysicalDevices();

		m_deviceInfos.reserve(physDevices.size());
		for (const Vk::PhysicalDevice& physDevice : physDevices)
			m_deviceInfos.push_back(Vulkan::BuildRenderDeviceInfo(physDevice));

		return true;
	}

	RenderAPI VulkanRenderer::QueryAPI() const
	{
		return RenderAPI::Vulkan;
	}

	std::string VulkanRenderer::QueryAPIString() const
	{
		std::ostringstream ss;
		ss << "Vulkan renderer " << VK_API_VERSION_MAJOR(APIVersion) << '.' << VK_API_VERSION_MINOR(APIVersion) << '.' << VK_API_VERSION_PATCH(APIVersion);

		return ss.str();
	}

	UInt32 VulkanRenderer::QueryAPIVersion() const
	{
		return APIVersion;
	}

	const std::vector<RenderDeviceInfo>& VulkanRenderer::QueryRenderDevices() const
	{
		return m_deviceInfos;
	}
}
