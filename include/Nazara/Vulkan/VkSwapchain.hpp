// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKAN_VKSWAPCHAIN_HPP
#define NAZARA_VULKAN_VKSWAPCHAIN_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Vulkan/VkDeviceObject.hpp>

namespace Nz 
{
	namespace Vk
	{
		class Swapchain : public DeviceObject<Swapchain, VkSwapchainKHR, VkSwapchainCreateInfoKHR>
		{
			friend DeviceObject;

			public:
				inline Swapchain(Device& instance);
				Swapchain(const Swapchain&) = delete;
				Swapchain(Swapchain&&) = delete;
				~Swapchain() = default;

				inline bool IsSupported() const;

				Swapchain& operator=(const Swapchain&) = delete;
				Swapchain& operator=(Swapchain&&) = delete;

			private:
				static VkResult CreateHelper(Device& device, const VkSwapchainCreateInfoKHR* createInfo, const VkAllocationCallbacks* allocator, VkSwapchainKHR* handle);
				static void DestroyHelper(Device& device, VkSwapchainKHR handle, const VkAllocationCallbacks* allocator);
		};
	}
}

#include <Nazara/Vulkan/VkSwapchain.inl>

#endif // NAZARA_VULKAN_VKSWAPCHAIN_HPP
