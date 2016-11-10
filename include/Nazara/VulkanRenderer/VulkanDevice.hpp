// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VULKANDEVICE_HPP
#define NAZARA_VULKANRENDERER_VULKANDEVICE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Renderer/RenderDeviceInstance.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Device.hpp>
#include <vector>

namespace Nz
{
	//TODO: Move all the software stuff to the Renderer

	class NAZARA_VULKANRENDERER_API VulkanDevice : public RenderDeviceInstance
	{
		public:
			VulkanDevice(Vk::DeviceHandle device);
			~VulkanDevice();

			VulkanDevice& operator=(const VulkanDevice&) = delete;
			VulkanDevice& operator=(VulkanDevice&&) = delete; ///TODO

		private:
			Vk::DeviceHandle m_device;
	};
}

#include <Nazara/VulkanRenderer/VulkanDevice.inl>

#endif // NAZARA_VULKANRENDERER_VULKANDEVICE_HPP
