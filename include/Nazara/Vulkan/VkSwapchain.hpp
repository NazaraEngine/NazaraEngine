// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKAN_VKSWAPCHAIN_HPP
#define NAZARA_VULKAN_VKSWAPCHAIN_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Vulkan/Config.hpp>
#include <Nazara/Vulkan/VkDeviceObject.hpp>

namespace Nz 
{
	namespace Vk
	{
		class Swapchain : public DeviceObject<Swapchain, VkSwapchainKHR, VkSwapchainCreateInfoKHR>
		{
			public:
				inline Swapchain(Device& instance);
				Swapchain(const Swapchain&) = delete;
				Swapchain(Swapchain&&) = delete;
				~Swapchain() = default;

				inline bool IsSupported() const;

				Swapchain& operator=(const Swapchain&) = delete;
				Swapchain& operator=(Swapchain&&) = delete;
		};
	}
}

#include <Nazara/Vulkan/VkSwapchain.inl>

#endif // NAZARA_VULKAN_VKSWAPCHAIN_HPP
