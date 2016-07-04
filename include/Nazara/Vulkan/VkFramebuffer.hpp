// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKAN_VKFRAMEBUFFER_HPP
#define NAZARA_VULKAN_VKFRAMEBUFFER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Vulkan/VkDeviceObject.hpp>

namespace Nz 
{
	namespace Vk
	{
		class Framebuffer : public DeviceObject<Framebuffer, VkFramebuffer, VkFramebufferCreateInfo>
		{
			friend DeviceObject;

			public:
				Framebuffer() = default;
				Framebuffer(const Framebuffer&) = delete;
				Framebuffer(Framebuffer&&) = default;
				~Framebuffer() = default;

				Framebuffer& operator=(const Framebuffer&) = delete;
				Framebuffer& operator=(Framebuffer&&) = delete;

			private:
				static inline VkResult CreateHelper(const DeviceHandle& device, const VkFramebufferCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkFramebuffer* handle);
				static inline void DestroyHelper(const DeviceHandle& device, VkFramebuffer handle, const VkAllocationCallbacks* allocator);
		};
	}
}

#include <Nazara/Vulkan/VkFramebuffer.inl>

#endif // NAZARA_VULKAN_VKFRAMEBUFFER_HPP
