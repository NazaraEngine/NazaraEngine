// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_WRAPPER_SEMAPHORE_HPP
#define NAZARA_VULKANRENDERER_WRAPPER_SEMAPHORE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/VulkanRenderer/Wrapper/DeviceObject.hpp>

namespace Nz
{
	namespace Vk
	{
		class Semaphore : public DeviceObject<Semaphore, VkSemaphore, VkSemaphoreCreateInfo, VK_OBJECT_TYPE_SEMAPHORE>
		{
			friend DeviceObject;

			public:
				Semaphore() = default;
				Semaphore(const Semaphore&) = delete;
				Semaphore(Semaphore&&) = default;
				~Semaphore() = default;

				using DeviceObject::Create;
				inline bool Create(Device& device, VkSemaphoreCreateFlags flags = 0, const VkAllocationCallbacks* allocator = nullptr);

				Semaphore& operator=(const Semaphore&) = delete;
				Semaphore& operator=(Semaphore&&) = delete;

			private:
				static inline VkResult CreateHelper(Device& device, const VkSemaphoreCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkSemaphore* handle);
				static inline void DestroyHelper(Device& device, VkSemaphore handle, const VkAllocationCallbacks* allocator);
		};
	}
}

#include <Nazara/VulkanRenderer/Wrapper/Semaphore.inl>

#endif // NAZARA_VULKANRENDERER_WRAPPER_SEMAPHORE_HPP
