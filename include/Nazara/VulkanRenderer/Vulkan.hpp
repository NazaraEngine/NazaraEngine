// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKAN_HPP
#define NAZARA_VULKAN_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Initializer.hpp>
#include <Nazara/Core/ParameterList.hpp>
#include <Nazara/Renderer/RenderDeviceInfo.hpp>
#include <Nazara/VulkanRenderer/Config.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Device.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Instance.hpp>
#include <Nazara/VulkanRenderer/Wrapper/PhysicalDevice.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Surface.hpp>
#include <list>
#include <memory>
#include <vector>

namespace Nz
{
	class VulkanDevice;

	class NAZARA_VULKANRENDERER_API Vulkan
	{
		public:
			struct QueueFamily
			{
				UInt32 familyIndex;
				float priority;
			};

			Vulkan() = delete;
			~Vulkan() = delete;

			static RenderDeviceInfo BuildRenderDeviceInfo(const Vk::PhysicalDevice& physDevice);

			static std::shared_ptr<VulkanDevice> CreateDevice(const Vk::PhysicalDevice& deviceInfo, const RenderDeviceFeatures& enabledFeatures);
			static std::shared_ptr<VulkanDevice> CreateDevice(const Vk::PhysicalDevice& deviceInfo, const RenderDeviceFeatures& enabledFeatures, const Vk::Surface& surface, UInt32* graphicsFamilyIndex, UInt32* presentableFamilyIndex, UInt32* transferFamilyIndex);
			static std::shared_ptr<VulkanDevice> CreateDevice(const Vk::PhysicalDevice& deviceInfo, const RenderDeviceFeatures& enabledFeatures, const QueueFamily* queueFamilies, std::size_t queueFamilyCount);

			static Vk::Instance& GetInstance();

			static const std::vector<Vk::PhysicalDevice>& GetPhysicalDevices();
			static const Vk::PhysicalDevice& GetPhysicalDeviceInfo(VkPhysicalDevice physDevice);

			static bool Initialize(UInt32 targetApiVersion, const ParameterList& parameters);

			static bool IsInitialized();

			static void Uninitialize();

		private:
			static std::vector<Vk::PhysicalDevice> s_physDevices;
			static Vk::Instance s_instance;
			static ParameterList s_initializationParameters;
	};	
}

#endif // NAZARA_VULKAN_HPP
