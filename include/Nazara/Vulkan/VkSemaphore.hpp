// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKAN_VKSEMAPHORE_HPP
#define NAZARA_VULKAN_VKSEMAPHORE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Vulkan/Config.hpp>
#include <Nazara/Vulkan/VkLoader.hpp>
#include <vulkan/vulkan.h>

namespace Nz 
{
	namespace Vk
	{
		class Device;

		class NAZARA_VULKAN_API Semaphore
		{
			public:
				inline Semaphore(Device& instance);
				Semaphore(const Semaphore&) = delete;
				Semaphore(Semaphore&&) = delete;
				inline ~Semaphore();

				inline bool Create(const VkSemaphoreCreateInfo& createInfo, const VkAllocationCallbacks* allocator = nullptr);
				inline bool Create(VkSemaphoreCreateFlags flags = 0, const VkAllocationCallbacks* allocator = nullptr);
				inline void Destroy();

				inline VkResult GetLastErrorCode() const;

				Semaphore& operator=(const Semaphore&) = delete;
				Semaphore& operator=(Semaphore&&) = delete;

				inline operator VkSemaphore();

			private:
				Device& m_device;
				VkAllocationCallbacks m_allocator;
				VkSemaphore m_semaphore;
				VkResult m_lastErrorCode;
		};
	}
}

#include <Nazara/Vulkan/VkSemaphore.inl>

#endif // NAZARA_VULKAN_VKSEMAPHORE_HPP
