// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	inline VulkanDevice::VulkanDevice(Vk::Instance& instance, const RenderDeviceFeatures& enabledFeatures, RenderDeviceInfo renderDeviceInfo) :
	Device(instance),
	m_enabledFeatures(enabledFeatures),
	m_renderDeviceInfo(std::move(renderDeviceInfo))
	{
	}
}
