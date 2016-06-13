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
				Swapchain() = default;
				Swapchain(const Swapchain&) = delete;
				Swapchain(Swapchain&&) = default;
				~Swapchain() = default;

				inline bool AcquireNextImage(Nz::UInt64 timeout, VkSemaphore semaphore, VkFence fence, UInt32* imageIndex);

				inline bool Create(const DeviceHandle& device, const VkSwapchainCreateInfoKHR& createInfo, const VkAllocationCallbacks* allocator = nullptr);

				inline VkImage GetImage(UInt32 index) const;
				inline const std::vector<VkImage>& GetImages() const;
				inline UInt32 GetImageCount() const;

				inline bool IsSupported() const;

				Swapchain& operator=(const Swapchain&) = delete;
				Swapchain& operator=(Swapchain&&) = delete;

			private:
				static VkResult CreateHelper(const DeviceHandle& device, const VkSwapchainCreateInfoKHR* createInfo, const VkAllocationCallbacks* allocator, VkSwapchainKHR* handle);
				static void DestroyHelper(const DeviceHandle& device, VkSwapchainKHR handle, const VkAllocationCallbacks* allocator);

				std::vector<VkImage> m_images;
		};
	}
}

#include <Nazara/Vulkan/VkSwapchain.inl>

#endif // NAZARA_VULKAN_VKSWAPCHAIN_HPP
