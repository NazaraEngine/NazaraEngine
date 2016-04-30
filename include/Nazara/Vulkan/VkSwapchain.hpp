// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKAN_VKSWAPCHAIN_HPP
#define NAZARA_VULKAN_VKSWAPCHAIN_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Vulkan/Config.hpp>
#include <Nazara/Vulkan/VkLoader.hpp>
#include <vulkan/vulkan.h>

namespace Nz 
{
	namespace Vk
	{
		class Device;

		class NAZARA_VULKAN_API Swapchain
		{
			public:
				inline Swapchain(Device& instance);
				Swapchain(const Swapchain&) = delete;
				Swapchain(Swapchain&&) = delete;
				inline ~Swapchain();

				bool Create(const VkSwapchainCreateInfoKHR& createInfo, const VkAllocationCallbacks* allocator = nullptr);
				inline void Destroy();

				inline bool IsSupported() const;

				inline VkResult GetLastErrorCode() const;

				Swapchain& operator=(const Swapchain&) = delete;
				Swapchain& operator=(Swapchain&&) = delete;

				inline operator VkSwapchainKHR();

			private:
				Device& m_device;
				VkAllocationCallbacks m_allocator;
				VkSwapchainKHR m_swapchain;
				VkResult m_lastErrorCode;
		};
	}
}

#include <Nazara/Vulkan/VkSwapchain.inl>

#endif // NAZARA_VULKAN_VKSWAPCHAIN_HPP
