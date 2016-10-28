// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VKSEMAPHORE_HPP
#define NAZARA_VULKANRENDERER_VKSEMAPHORE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/VulkanRenderer/Wrapper/DeviceObject.hpp>

namespace Nz 
{
	namespace Vk
	{
		class Semaphore : public DeviceObject<Semaphore, VkSemaphore, VkSemaphoreCreateInfo>
		{
			friend DeviceObject;

			public:
				Semaphore() = default;
				Semaphore(const Semaphore&) = delete;
				Semaphore(Semaphore&&) = default;
				~Semaphore() = default;

				using DeviceObject::Create;
				inline bool Create(const DeviceHandle& device, VkSemaphoreCreateFlags flags = 0, const VkAllocationCallbacks* allocator = nullptr);

				Semaphore& operator=(const Semaphore&) = delete;
				Semaphore& operator=(Semaphore&&) = delete;

			private:
				static inline VkResult CreateHelper(const DeviceHandle& device, const VkSemaphoreCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkSemaphore* handle);
				static inline void DestroyHelper(const DeviceHandle& device, VkSemaphore handle, const VkAllocationCallbacks* allocator);
		};
	}
}

#include <Nazara/VulkanRenderer/Wrapper/Semaphore.inl>

#endif // NAZARA_VULKANRENDERER_VKSEMAPHORE_HPP
