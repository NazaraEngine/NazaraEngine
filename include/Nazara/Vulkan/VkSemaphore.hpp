// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKAN_VKSEMAPHORE_HPP
#define NAZARA_VULKAN_VKSEMAPHORE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Vulkan/VkDeviceObject.hpp>

namespace Nz 
{
	namespace Vk
	{
		class Semaphore : public DeviceObject<Semaphore, VkSemaphore, VkSemaphoreCreateInfo>
		{
			friend DeviceObject;

			public:
				inline Semaphore(Device& instance);
				Semaphore(const Semaphore&) = delete;
				Semaphore(Semaphore&&) = default;
				~Semaphore() = default;

				using DeviceObject::Create;
				inline bool Create(VkSemaphoreCreateFlags flags = 0, const VkAllocationCallbacks* allocator = nullptr);

				Semaphore& operator=(const Semaphore&) = delete;
				Semaphore& operator=(Semaphore&&) = delete;

			private:
				static VkResult CreateHelper(Device& device, const VkSemaphoreCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkSemaphore* handle);
				static void DestroyHelper(Device& device, VkSemaphore handle, const VkAllocationCallbacks* allocator);
		};
	}
}

#include <Nazara/Vulkan/VkSemaphore.inl>

#endif // NAZARA_VULKAN_VKSEMAPHORE_HPP
