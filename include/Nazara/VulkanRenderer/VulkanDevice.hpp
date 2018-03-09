// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VULKANDEVICE_HPP
#define NAZARA_VULKANRENDERER_VULKANDEVICE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/RenderDevice.hpp>
#include <Nazara/VulkanRenderer/VulkanBuffer.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Device.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_VULKANRENDERER_API VulkanDevice : public RenderDevice, public Vk::Device
	{
		public:
			using Device::Device;
			VulkanDevice(const VulkanDevice&) = delete;
			VulkanDevice(VulkanDevice&&) = delete; ///TODO?
			~VulkanDevice();

			std::unique_ptr<AbstractBuffer> InstantiateBuffer(Buffer* parent, BufferType type) override;

			VulkanDevice& operator=(const VulkanDevice&) = delete;
			VulkanDevice& operator=(VulkanDevice&&) = delete; ///TODO?
	};
}

#include <Nazara/VulkanRenderer/VulkanDevice.inl>

#endif // NAZARA_VULKANRENDERER_VULKANDEVICE_HPP
