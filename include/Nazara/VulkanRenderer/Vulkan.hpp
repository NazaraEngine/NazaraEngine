// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKAN_HPP
#define NAZARA_VULKAN_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Initializer.hpp>
#include <Nazara/Core/ParameterList.hpp>
#include <Nazara/VulkanRenderer/Config.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Device.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Instance.hpp>
#include <Nazara/VulkanRenderer/Wrapper/PhysicalDevice.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Surface.hpp>
#include <list>
#include <vector>

namespace Nz
{
	class NAZARA_VULKANRENDERER_API Vulkan
	{
		public:
			Vulkan() = delete;
			~Vulkan() = delete;

			static Vk::DeviceHandle CreateDevice(VkPhysicalDevice gpu, const Vk::Surface& surface, UInt32* presentableFamilyQueue);

			static Vk::Instance& GetInstance();

			static const std::vector<Vk::PhysicalDevice>& GetPhysicalDevices();
			static const Vk::PhysicalDevice& GetPhysicalDeviceInfo(VkPhysicalDevice physDevice);
			
			static bool Initialize(UInt32 apiVersion, const ParameterList& parameters);

			static bool IsInitialized();

			static Vk::DeviceHandle SelectDevice(VkPhysicalDevice gpu, const Vk::Surface& surface, UInt32* presentableFamilyQueue);

			static void Uninitialize();

		private:
			static std::list<Vk::Device> s_devices;
			static std::vector<Vk::PhysicalDevice> s_physDevices;
			static Vk::Instance s_instance;
			static ParameterList s_initializationParameters;
	};	
}

#endif // NAZARA_VULKAN_HPP
