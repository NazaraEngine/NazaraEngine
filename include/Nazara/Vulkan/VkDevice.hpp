// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKAN_VKDEVICE_HPP
#define NAZARA_VULKAN_VKDEVICE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Vulkan/Config.hpp>
#include <Nazara/Vulkan/VkLoader.hpp>
#include <vulkan/vulkan.h>

namespace Nz 
{
	namespace Vk
	{
		class Instance;

		class NAZARA_VULKAN_API Device
		{
			public:
				inline Device(Instance& instance);
				Device(const Device&) = delete;
				Device(Device&&) = delete;
				inline ~Device();

				bool Create(VkPhysicalDevice device, const VkDeviceCreateInfo& createInfo, const VkAllocationCallbacks* allocator = nullptr);
				inline void Destroy();

				inline void GetDeviceQueue(UInt32 queueFamilyIndex, UInt32 queueIndex, VkQueue* queue);
				inline VkResult GetLastErrorCode() const;

				inline bool WaitForIdle();

				Device& operator=(const Device&) = delete;
				Device& operator=(Device&&) = delete;

				// Vulkan functions
				#define NAZARA_VULKAN_DEVICE_FUNCTION(func) PFN_##func func

				NAZARA_VULKAN_DEVICE_FUNCTION(vkDestroyDevice);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkDeviceWaitIdle);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkGetDeviceQueue);

				#undef NAZARA_VULKAN_DEVICE_FUNCTION

			private:
				inline PFN_vkVoidFunction GetProcAddr(const char* name);

				Instance& m_instance;
				VkAllocationCallbacks m_allocator;
				VkDevice m_device;
				VkResult m_lastErrorCode;
		};
	}
}

#include <Nazara/Vulkan/VkDevice.inl>

#endif // NAZARA_VULKAN_VKDEVICE_HPP
