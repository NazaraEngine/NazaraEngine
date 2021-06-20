// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanDevice.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	inline VulkanDevice::VulkanDevice(Vk::Instance& instance, RenderDeviceInfo renderDeviceInfo) :
	Device(instance),
	m_renderDeviceInfo(std::move(renderDeviceInfo))
	{
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
